/******************************************************************************
*                                                                             *
*  Copyright © 2024      -- LIRMM/CNRS/UM                                     *
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

#include "settings.hpp"

#include "bwt_transformer.hpp"
#include "common.hpp"
#include "canonical_transformer.hpp"
#include "composition_transformer.hpp"
#include "identity_transformer.hpp"
#include "inthash_transformer.hpp"
#include "gab_transformer.hpp"
#include "lyndon_transformer.hpp"
#include "minimizer_transformer.hpp"
#include "permutation_bit_transformer.hpp"
#include "permutation_transformer.hpp"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

Settings::Settings(size_t length, size_t prefix_length, const std::string &method,
                   const std::string &tag,
                   size_t nb_bins, size_t queue_size,
                   bool verbose):
  _transformer(), _method(method),
  length(length), prefix_length(prefix_length),
  tag(tag),
  nb_bins(nb_bins), queue_size(queue_size),
  verbose(verbose)
{
  assert(prefix_length > 0);
  assert(prefix_length < 14);
  assert(prefix_length < length);
  assert(length - prefix_length <= 64);
  DEBUG_MSG("ICI");
  _transformer = _string2transformer(method);
  DEBUG_MSG("ICI");
}

shared_ptr<const Transformer> Settings::_string2transformer(const string &name) const {
  DEBUG_MSG("ICI");
  shared_ptr<const Transformer> t(NULL);
  if (name == "identity") {
    t = make_shared<const IdentityTransformer>(length, prefix_length);
  } else if (name == "canonical") {
    t = make_shared<const CanonicalTransformer>(length, prefix_length);
  } else if (name == "inthash") {
    t = make_shared<const IntHashTransformer>(length, prefix_length);
  } else if (name == "minimizer") {
    t = make_shared<const MinimizerTransformer>(length, prefix_length);
  } else if (name == "bwt") {
    t = make_shared<const BwtTransformer>(length, prefix_length);
  } else if (name.substr(0,3) == "GAB") {
    uint64_t a = 0, b = 0;
    if (name[3] == '=') {
      char *ptr;
      a = strtoul(name.c_str() + 4, &ptr, 10);
      if (*ptr == ',') {
        ++ptr;
        b = strtoul(ptr, &ptr, 10);
      }
      if (*ptr != '\0') {
        cerr << "Error: unable to parse the GAB method parameters." << endl;
        exit(1);
      }
    }
    t = make_shared<const GaBTransformer>(length, prefix_length, a, b);
  } else if (name == "random_nucl") {
    t = make_shared<const PermutationTransformer>(length, prefix_length);
  } else if (name == "random_bits") {
    t = make_shared<const PermutationBitTransformer>(length, prefix_length);
  } else if (name == "inverse") {
    vector<size_t> p(length);
    for (size_t i = 0; i < length; ++i) {
      p[i] = length - i - 1;
    }
    t = make_shared<const PermutationTransformer>(length, prefix_length, p, "Inverse");
  } else if (name == "cyclic") {
    vector<size_t> p(length);
    for (size_t i = 0; i < length; ++i) {
      p[i] = (i + 1) % length;
    }
    t = make_shared<const PermutationTransformer>(length, prefix_length, p, "Cyclic");
  } else if (name == "lyndon") {
    t = make_shared<const LyndonTransformer>(length, prefix_length);
  } else if (name == "zigzag") {
    vector<size_t> p(length);
    for (size_t i = 0; i < length; ++i) {
      p[i] = ((i & 1) ? (length - i - (length & 1)) : i);
    }

    t = make_shared<const PermutationTransformer>(length, prefix_length, p, "ZigZag");
  } else if (name.substr(0,11) == "composition") {
    if (name[11] == '=') {
      size_t count = (name[12] == '(');
      size_t p = 13;
      size_t comma = 0;
      size_t n = name.length();
      string t1_name;
      string t2_name;
      while ((p < n) && count) {
        switch (name[p]) {
        case '(':
          ++count;
          break;
        case ')':
          --count;
          break;
        case '*':
          if (count == 1) {
            assert(t1_name.empty());
            t1_name = name.substr(13, p - 13);
            comma = p;
          }
          break;
        default:
        break;
        }
        ++p;
      }
      assert(count == 0);
      assert(p == n);
      assert(comma + 1 < n);
      t2_name = name.substr(comma + 1, n - comma - 2);
      shared_ptr<const Transformer> t1 = _string2transformer(t1_name);
      shared_ptr<const Transformer> t2 = _string2transformer(t2_name);
      t = make_shared<const CompositionTransformer>(length, prefix_length, t1, t2);
    } else {
      cerr << "Error: unable to parse the Composition method parameters." << endl;
      exit(1);
    }
  } else {
    cerr << "Error: Unsupported transformation method '" << name << "'." << endl;
    exit(1);
  }
  return t;
}

void Settings::setMethod(const string &method) {
  _transformer = _string2transformer(method);
  _method = method;
}

shared_ptr<const Transformer> Settings::transformer() const {
  DEBUG_MSG("ICI");
  if (!_transformer) {
    DEBUG_MSG("ICI");
    shared_ptr<const Transformer> _t = const_pointer_cast<const Transformer>(_transformer);
    _t = _string2transformer(_method);
   }
  return _transformer;
}

ostream &operator<<(ostream &os, const Settings &s) {
  os << "- length: " << s.length << " nucleotides\n"
     << "- prefix_length: " << s.prefix_length << " nucleotides\n"
     << "- method: " << s.getMethod() << " => " << s.transformer()->description << '\n'
     << "- nb_bins: " << s.nb_bins << " bins\n"
     << "- queue_size: " << s.queue_size << " k-mers\n"
     << "- tag: " << s.tag << '\n'
     << "- verbosity: " << (s.verbose ? "verbose" : "quiet") << endl;
  return os;
}

END_BIJECTHASH_NAMESPACE
