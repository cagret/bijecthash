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

#include "settings.hpp"

#include "common.hpp"
#include "exception.hpp"
#include "transformer.hpp"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

Settings::Settings(size_t kmer_length, size_t prefix_length, const std::string &method,
                   const std::string &tag,
                   size_t nb_bins, size_t queue_size,
                   bool verbose):
  _transformer(), _method(method),
  kmer_length(kmer_length), prefix_length(prefix_length),
  tag(tag),
  nb_bins(nb_bins), queue_size(queue_size),
  verbose(verbose)
{
  assert(prefix_length > 0);
  assert(prefix_length < 14);
  assert(prefix_length < kmer_length);
  assert(kmer_length - prefix_length <= 64);
}

bool Settings::setMethod(const string &method) {
  try {
    _transformer = Transformer::string2transformer(kmer_length, prefix_length, method);
    _method = method;
  } catch (const Exception &e) {
    if (verbose) cerr << e.what();
    _transformer.reset();
  }
  return (bool) _transformer;
}

shared_ptr<const Transformer> Settings::transformer() const {
  if (!_transformer) {
    DEBUG_MSG("First time transformer is invoked");
    shared_ptr<const Transformer> _t = const_pointer_cast<const Transformer>(_transformer);
    _t = Transformer::string2transformer(kmer_length, prefix_length, _method);
   }
  return _transformer;
}

ostream &operator<<(ostream &os, const Settings &s) {
  os << "- kmer_length: " << s.kmer_length << " nucleotides\n"
     << "- prefix_length: " << s.prefix_length << " nucleotides\n"
     << "- method: " << s.getMethod() << " => " << s.transformer()->description << '\n'
     << "- nb_bins: " << s.nb_bins << " bins\n"
     << "- queue_size: " << s.queue_size << " k-mers\n"
     << "- tag: " << s.tag << '\n'
     << "- verbosity: " << (s.verbose ? "verbose" : "quiet") << endl;
  return os;
}

END_BIJECTHASH_NAMESPACE
