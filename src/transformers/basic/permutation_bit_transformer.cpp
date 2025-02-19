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

#include "permutation_bit_transformer.hpp"

#include "common.hpp"

#include <algorithm>
#ifdef DEBUG
#  include <iomanip>
#endif
#include <random>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

uint64_t PermutationBitTransformer::_applyBitwisePermutation(uint64_t encoded_kmer, const vector<size_t> &permutation) const {
  uint64_t permuted = 0;
  for (size_t i = 0; i < permutation.size(); ++i) {
    size_t bit_index = permutation[i];
    uint64_t bit = (encoded_kmer >> bit_index) & 1;
    permuted |= (bit << i);
  }
  return permuted;
}

vector<size_t> PermutationBitTransformer::_generateRandomPermutation(size_t n) {
  vector<size_t> p(n);
  iota(p.begin(), p.end(), 0);
  random_device rd;
  mt19937 g(rd());
  shuffle(p.begin(), p.end(), g);
  return p;
}

vector<size_t> PermutationBitTransformer::_computeReversePermutation(const vector<size_t> &p) {
  size_t n = p.size();
  vector<size_t> r(n);
  for (size_t i = 0; i < n; ++i) {
    r[p[i]] = i;
  }
  return r;
}

Transformer::EncodedKmer PermutationBitTransformer::operator()(const string &kmer) const {
  EncodedKmer e;
  assert(kmer.size() == kmer_length);
  if (kmer_length <= 32) {
    uint64_t v = _encode(kmer.c_str(), kmer_length);
#ifdef DEBUG
    uint64_t orig = v;
#endif
    v = _applyBitwisePermutation(v, _permutation);
#ifdef DEBUG
    uint64_t rev_v = _applyBitwisePermutation(v, _reverse_permutation);
    DEBUG_MSG("orig = " << orig);
    DEBUG_MSG("v = " << v);
    DEBUG_MSG("rev_v = " << rev_v);
    assert(orig == rev_v);
#endif
    e.prefix = v >> _prefix_shift;
    e.suffix = v & _suffix_mask;
  } else {
    uint64_t prefix = _encode(kmer.c_str(), 32);
    uint64_t suffix = _encode(kmer.c_str() + 32, kmer_length - 32);
    uint64_t prefix_transformed = _applyBitwisePermutation(prefix, _permutation);
    e.prefix = prefix_transformed >> _prefix_shift;
    e.suffix = prefix_transformed << (prefix_length << 1) | suffix;
  }
  return e;
}

string PermutationBitTransformer::operator()(const EncodedKmer &e) const {
  if (kmer_length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    v = _applyBitwisePermutation(v, _reverse_permutation);
    return _decode(v, kmer_length);
  } else {
    uint64_t u = (e.prefix << _prefix_shift) | (e.suffix >> (prefix_length << 1));
    u = _applyBitwisePermutation(u, _reverse_permutation);
    uint64_t v = e.suffix & _suffix_mask;
    return _decode(u, 32) + _decode(v, kmer_length - 32);
  }
}

PermutationBitTransformer::PermutationBitTransformer(size_t kmer_length, size_t prefix_length,
                                                     const vector<size_t> &permutation, const string &description):
  Transformer(kmer_length, prefix_length, description),
  _permutation(permutation.size() == 2 * kmer_length ? permutation : _generateRandomPermutation(kmer_length <= 32 ? 2 * kmer_length : 64)),
  _reverse_permutation(_computeReversePermutation(_permutation)),
  _kmer_mask((1ull << kmer_length << kmer_length) - 1ull),
  _prefix_shift((((kmer_length > 32) ? 32 : kmer_length) - prefix_length) << 1),
  _suffix_mask((kmer_length > 32) ? ((1ull << ((kmer_length - 32) << 1)) - 1) : ((1ull << (suffix_length << 1)) - 1))
{
  if (description.empty()) {
    string *desc_ptr = const_cast<string *>(&(this->description));
    desc_ptr->clear();
    *desc_ptr += "Permutation_bins[";
    for (size_t i = 0; i < 2 * kmer_length; ++i) {
      if (i) *desc_ptr += ",";
      *desc_ptr += to_string(_permutation[i]);
    }
    *desc_ptr += "]";
  }

  DEBUG_MSG("description: '" << description << "'" << '\n';
            cerr << MSG_DBG_HEADER << "permutation:" << '\n';
            cerr << "  ";
            for (size_t i = 0; i < 2 * kmer_length; ++i) {
              cerr << setw(4) << i;
            }
            cerr << '\n' << "  ";
            for (size_t i = 0; i < 2 * kmer_length; ++i) {
              cerr << setw(4) << _permutation[i];
            }
            cerr << '\n';
            cerr << MSG_DBG_HEADER << "reverse permutation:" << '\n';
            cerr << "  ";
            for (size_t i = 0; i < 2 * kmer_length; ++i) {
              cerr << setw(4) << i;
            }
            cerr << '\n';
            cerr << "  ";
            for (size_t i = 0; i < 2 * kmer_length; ++i) {
              cerr << setw(4) << _reverse_permutation[i];
            }
            cerr);
}

END_BIJECTHASH_NAMESPACE
