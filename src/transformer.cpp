/******************************************************************************
*                                                                             *
*  Copyright © 2024-2025 -- LIRMM/CNRS/UM                                     *
*                           (Laboratoire d'Informatique, de Robotique et de   *
*                           Microélectronique de Montpellier /                *
*                           Centre National de la Recherche Scientifique /    *
*                           Université de Montpellier)                        *
*                           CRIStAL/CNRS/UL                                   *
*                           (Centre de Recherche en Informatique, Signal et   *
*                           Automatique de Lille /                            *
*                           Centre National de la Recherche Scientifique /    *
*                           Université de Lille)                              *
*                                                                             *
*  Auteurs/Authors:                                                           *
*                   Clément AGRET      <cagret@mailo.com>                     *
*                   Annie   CHATEAU    <annie.chateau@lirmm.fr>               *
*                   Antoine LIMASSET   <antoine.limasset@univ-lille.fr>       *
*                   Alban   MANCHERON  <alban.mancheron@lirmm.fr>             *
*                   Camille MARCHET    <camille.marchet@univ-lille.fr>        *
*                                                                             *
*  Programmeurs/Programmers:                                                  *
*                   Clément AGRET      <cagret@mailo.com>                     *
*                   Alban   MANCHERON  <alban.mancheron@lirmm.fr>             *
*                                                                             *
*  -------------------------------------------------------------------------  *
*                                                                             *
*  This file is part of BijectHash.                                           *
*                                                                             *
*  BijectHash is free software: you can redistribute it and/or modify it      *
*  under the terms of the GNU General Public License as published by the      *
*  Free Software Foundation, either version 3 of the License, or (at your     *
*  option) any later version.                                                 *
*                                                                             *
*  BijectHash is distributed in the hope that it will be useful, but WITHOUT  *
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      *
*  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   *
*  more details.                                                              *
*                                                                             *
*  You should have received a copy of the GNU General Public License along    *
*  with BijectHash. If not, see <https://www.gnu.org/licenses/>.              *
*                                                                             *
******************************************************************************/

#include "transformer.hpp"

#include "common.hpp"
#include "exception.hpp"
#include "locker.hpp"

#include <iostream>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

///////////////////////////////////////////////
// Transformer static attributes and methods //
///////////////////////////////////////////////

const int Transformer::SPHINXPP_PLUGIN_TAG = 2005515862;

static sphinxpp::PluginHandler::Constraints _plugin_constraints;

list<Transformer::_TransformerInformations> Transformer::_available_transformers;

sphinxpp::PluginHandler Transformer::_plugin_handler;


shared_ptr<const Transformer> Transformer::string2transformer(size_t kmer_length, size_t prefix_length, const string &method) {
  assert(kmer_length > 0);
  assert(prefix_length < 14);
  assert(prefix_length < kmer_length);
  assert(kmer_length - prefix_length <= 64);
  _updateAvailableTransformers();
  shared_ptr<const Transformer> t(NULL);
  if (method.empty()) return t;

  list<_TransformerInformations>::const_iterator it = _available_transformers.begin();
  size_t pos = method.find_first_of("=(");
  string label, extra;
  label = method.substr(0, pos);
  if (pos != string::npos) {
    extra = method.substr(pos + (method[pos] == '='));
  }
  DEBUG_MSG("Method '" << method << "' => label='" << label << "' and extra='" << extra << "'");
  while ((it != _available_transformers.end()) && (label != it->label)) {
    DEBUG_MSG("Comparison with '" << it->label << "' failed");
    ++it;
  }
  if (it != _available_transformers.end()) {
    DEBUG_MSG("Comparison with '" << it->label << "' succeed");
    it->factory(kmer_length, prefix_length, label, extra, t);
  } else {
    Exception e;
    e << "Error: Unsupported transformation method '" << method << "'.\n";
    throw e;
  }
  return t;
}

void Transformer::_addTransformers(Transformer::TransformerFactory factory, const Transformer::TransformerInformations *informations) {
  while (informations && *informations && !(*informations)[0].empty()) {
    _TransformerInformations info = {
      (*informations)[0],
      (*informations)[1],
      (*informations)[2],
      factory
    };
    _available_transformers.push_back(info);
    ++informations;
  }
}

void Transformer::_updateAvailableTransformers() {
  _plugin_handler.loadAvailablePlugins(true, SPHINXPP_PLUGIN_TAG, _plugin_constraints, false);
  _available_transformers.clear();
  DEBUG_MSG("Load plugins from directories:" << endl;
            for (auto &p: _plugin_handler.searchPaths()) {
              cerr << "- '" << p << "'" << endl;
            };
            cerr);
  for (const std::string &plugin_name: _plugin_handler.getHandledPlugins()) {
    const sphinxpp::Plugin *plugin = _plugin_handler.get(plugin_name);
    assert(plugin);
    assert(plugin->exportedSymbols().find("transformerList") != plugin->exportedSymbols().end());
    TransformerFactory factory = plugin->getFunction<TransformerFactory>("transformerFactory");
    TransformerInformations *informations = plugin->getVariable<TransformerInformations *>("transformerList");
    _addTransformers(factory, informations);
  }
}

void Transformer::addPluginSearchPath(const std::string &path) {
  _plugin_handler.addSearchPath(path);
}

bool Transformer::addPlugin(const std::string &path) {
  return _plugin_handler.loadPlugin(path, SPHINXPP_PLUGIN_TAG, _plugin_constraints);
}

void _describeMethods(ostream &os,
                      const string &name, const string &summary,
                      const string &filename, const string &authors,
                      const string &version, const string &description) {
  os << "\n* " << name;
  if (!summary.empty()) {
    os << ": " << summary;
  }
  os << "\n";
  if (!filename.empty()) {
    os << "  File: '" << filename << "'\n";
  }
  if (!authors.empty()) {
    os << "  Authors: " << authors << "\n";
  }
  if (!version.empty()) {
    os << "  Version: " << version << "\n";
  }
  if (!description.empty()) {
    os << "  Description:\n    ";
    for (char c: description) {
      os << c;
      if (c == '\n') os << "    ";
    }
    os << "\n";
  }
}

void Transformer::toStream(ostream &os) {
  _updateAvailableTransformers();
  for (const std::string &plugin_name: _plugin_handler.getHandledPlugins()) {
    const sphinxpp::Plugin *plugin = _plugin_handler.get(plugin_name);
    assert(plugin);
    ostringstream version;
    version << plugin->version();
    _describeMethods(os,
                     plugin->name(), plugin->summary(),
                     plugin->filename(), plugin->authors(),
                     version.str(), plugin->description());
  }
}


////////////////////////////////////////
// Transformer abstract class methods //
////////////////////////////////////////

uint64_t Transformer::_encode(const char *dna_str, size_t n) {
  DEBUG_MSG("n = " << n);
  DEBUG_MSG("dna_str = '" << string(dna_str, n) << "'");
  assert(n <= (4 * sizeof(uint64_t)));
  uint64_t encoded = 0;
  for (size_t i = 0; i < n; ++i) {
    int val = 0;
    switch (dna_str[i]) {
    case 'A': val = 0; break;
    case 'C': val = 1; break;
    case 'G': val = 2; break;
    case 'T': val = 3; break;
    default:
      Exception e;
      e << "Error: unable to encode the given k-mer (" << string(dna_str, n) << ")"
        << " since it contains the symbol '" << dna_str[i] << "'"
        << " which is neither A nor C nor G nor T.\n";
      throw e;
    }
    encoded = (encoded << 2) | val;
  }
  DEBUG_MSG("encoded = " << encoded);
  return encoded;
}

string Transformer::_decode(uint64_t v, size_t n) {
  DEBUG_MSG("n = " << n);
  DEBUG_MSG("v = " << v);
  string decoded(n, '?');
  while (n--) {
    decoded[n] = "ACGT"[v & 3];
    v >>= 2;
  }
  DEBUG_MSG("decoded = '" << decoded << "'");
  return decoded;
}

Transformer::Transformer(const size_t kmer_length, const size_t prefix_length, const string &description):
  kmer_length(kmer_length),
  prefix_length(prefix_length),
  suffix_length(kmer_length - prefix_length),
  description(description)
{
  DEBUG_MSG("Transformer " << description << " for " << kmer_length << "-mers"
            << " with prefix length " << prefix_length
            << " and suffix length " << suffix_length);
  assert(kmer_length > 0);
  assert(prefix_length > 0);
  assert(kmer_length > prefix_length);
  assert(prefix_length <= (4 * sizeof(uint64_t)));
  assert((kmer_length - prefix_length) <= (4 * sizeof(uint64_t)));
}

string Transformer::getTransformedKmer(const Transformer::EncodedKmer &e) const {
  string kmer = _decode(e.prefix, prefix_length);
  kmer += _decode(e.suffix, suffix_length);
  return kmer;
}

END_BIJECTHASH_NAMESPACE
