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

#include "inthash_transformer.hpp"

#include "common.hpp"
#include "inthash.h"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

IntHashTransformer::IntHashTransformer(size_t kmer_length, size_t prefix_length):
  Transformer(kmer_length, prefix_length, "IntHash"),
  _kmer_mask((1ull << kmer_length << kmer_length) - 1ull),
  _prefix_shift((((kmer_length > 32) ? 32 : kmer_length) - prefix_length) << 1),
  _suffix_mask((kmer_length > 32) ? ((1ull << ((kmer_length - 32) << 1)) - 1) : ((1ull << (suffix_length << 1)) - 1))
{}


Transformer::EncodedKmer IntHashTransformer::operator()(const string &kmer) const {
  EncodedKmer e;
  assert(kmer.size() == kmer_length);
  if (kmer_length <= 32) {
    uint64_t v = _encode(kmer.c_str(), kmer_length);
#ifdef DEBUG
    uint64_t orig = v;
#endif
    v = hash_64(v, _kmer_mask);
#ifdef DEBUG
    uint64_t rev_v = hash_64i(v, _kmer_mask);
    DEBUG_MSG("orig = " << orig << '\n'
              << MSG_DBG_HEADER << "v = " << v << '\n'
              << MSG_DBG_HEADER << "rev_v = " << rev_v);
    assert(orig == rev_v);
#endif
    e.prefix = v >> _prefix_shift;
    e.suffix = v & _suffix_mask;
  } else {
    uint64_t prefix = _encode(kmer.c_str(), 32);
    uint64_t suffix = _encode(kmer.c_str() + 32, kmer_length - 32);
    uint64_t prefix_transformed = hash_64(prefix, _kmer_mask);
    e.prefix = prefix_transformed >> _prefix_shift;
    e.suffix = prefix_transformed << (prefix_length << 1) | suffix;
  }
  return e;
}

string IntHashTransformer::operator()(const Transformer::EncodedKmer &e) const {
  if (kmer_length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    v = hash_64i(v, _kmer_mask);
    return _decode(v, kmer_length);
  } else {
    uint64_t u = (e.prefix << _prefix_shift) | (e.suffix >> (prefix_length << 1));
    u = hash_64i(u, _kmer_mask);
    uint64_t v = e.suffix & _suffix_mask;
    return _decode(u, 32) + _decode(v, kmer_length - 32);
  }
}

string IntHashTransformer::getTransformedKmer(const Transformer::EncodedKmer &e) const {
  if (kmer_length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    return _decode(v, kmer_length);
  } else {
    uint64_t u = (e.prefix << _prefix_shift) | (e.suffix >> (prefix_length << 1));
    uint64_t v = e.suffix & _suffix_mask;
    return _decode(u, 32) + _decode(v, kmer_length - 32);
  }
}

END_BIJECTHASH_NAMESPACE
