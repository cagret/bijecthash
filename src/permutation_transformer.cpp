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

#include "permutation_transformer.hpp"

#include "common.hpp"

#include <algorithm>
#ifdef DEBUG
#  include <iomanip>
#endif
#include <random>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

PermutationTransformer::PermutationTransformer(size_t kmer_length, size_t prefix_length, const vector<size_t> &permutation, const string &description):
  Transformer(kmer_length, prefix_length, description),
  _permutation(permutation.size() == kmer_length ? permutation : _generateRandomPermutation(kmer_length)),
  _reverse_permutation(_computeReversePermutation(_permutation))
{
  if (description.empty()) {
    string *desc_ptr = const_cast<string *>(&(this->description));
    desc_ptr->clear();
    *desc_ptr += "Permutation_nucl[";
    for (size_t i = 0; i < kmer_length; ++i) {
      if (i) *desc_ptr += ",";
      *desc_ptr += to_string(_permutation[i]);
    }
    *desc_ptr += "]";
  }
  DEBUG_MSG("description: '" << description << "'" << '\n';
            cerr << MSG_DBG_HEADER << "permutation:" << '\n';
            cerr << "  ";
            for (size_t i = 0; i < kmer_length; ++i) {
              cerr << setw(4) << i;
            }
            cerr << '\n';
            cerr << "  ";
            for (size_t i = 0; i < kmer_length; ++i) {
              cerr << setw(4) << _permutation[i];
            }
            cerr << '\n';
            cerr << MSG_DBG_HEADER << "reverse permutation:" << '\n';
            cerr << "  ";
            for (size_t i = 0; i < kmer_length; ++i) {
              cerr << setw(4) << i;
            }
            cerr << '\n';
            cerr << "  ";
            for (size_t i = 0; i < kmer_length; ++i) {
              cerr << setw(4) << _reverse_permutation[i];
            }
            cerr);
}


vector<size_t> PermutationTransformer::_generateRandomPermutation(size_t k) {
  vector<size_t> p(k);
  iota(p.begin(), p.end(), 0);
  random_device rd;
  mt19937 g(rd());
  shuffle(p.begin(), p.end(), g);
  return p;
}

vector<size_t> PermutationTransformer::_computeReversePermutation(const vector<size_t> &p) {
  size_t n = p.size();
  vector<size_t> r(n);
  for (size_t i = 0; i < n; i++) {
    r[p[i]] = i;
  }
  return r;
}

string PermutationTransformer::_applyPermutation(const string &s, const vector<size_t> &p) {
  string permuted(s.size(), 0);
  for (size_t i = 0; i < s.size(); ++i) {
    permuted[i] = s[p[i]];
  }
  return permuted;
}

Transformer::EncodedKmer PermutationTransformer::operator()(const string &kmer) const {
  EncodedKmer e;
  string permuted_kmer = _applyPermutation(kmer, _permutation);
  DEBUG_MSG("Permuted k-mer:   '" << permuted_kmer << "'" << '\n';
            string unpermuted_kmer = _applyPermutation(permuted_kmer, _reverse_permutation);
            cerr << MSG_DBG_HEADER << "Unpermuted k-mer: '" << unpermuted_kmer << "'" << '\n';
            if (unpermuted_kmer != kmer) {
              cerr << "Error: the unpermuted k-mer"
                   << " differs from the original k-mer"
                   << endl;
              exit(1);
            }
            cerr);
  e.prefix = _encode(permuted_kmer.c_str(), prefix_length);
  e.suffix = _encode(permuted_kmer.c_str() + prefix_length, suffix_length);
  return e;
}

string PermutationTransformer::operator()(const Transformer::EncodedKmer &e) const {
  string permuted_kmer = _decode(e.prefix, prefix_length);
  permuted_kmer += _decode(e.suffix, suffix_length);
  string kmer = _applyPermutation(permuted_kmer, _reverse_permutation);
  return kmer;
}

END_BIJECTHASH_NAMESPACE
