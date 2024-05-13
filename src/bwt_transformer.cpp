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

#include "bwt_transformer.hpp"

#include "common.hpp"

#include <algorithm>
#include <vector>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

BwtTransformer::BwtTransformer(size_t kmer_length, size_t prefix_length):
  Transformer(kmer_length, prefix_length, "Bwt") {
  assert(suffix_length <= 30);
}

Transformer::EncodedKmer BwtTransformer::operator()(const string& kmer) const {
  EncodedKmer encoded;
  int n = kmer.length();
  size_t bwt_pos =0;
  int pos = n - 1;
  vector<string> rotations(n);

  for (int i = 0; i < n; ++i) {
    int shift = (i + pos) % n;
    rotations[i] = kmer.substr(shift) + kmer.substr(0, shift);
  }

  sort(rotations.begin(), rotations.end());
  string result;
  for (int i = 0; i < n; ++i) {
    result += rotations[i][n - 1];
    if (rotations[i] == kmer) {
      bwt_pos = i;
    }
  }

  DEBUG_MSG("kmer: '" << kmer << "'" << " BWT rotation index: '" << bwt_pos << "'");

  assert(bwt_pos < 64); // BWT_pos can be encoded in only 6 bits


  encoded.prefix = _encode(result.c_str(), prefix_length);
  encoded.suffix = _encode(result.c_str() + prefix_length, suffix_length);
  encoded.suffix |= bwt_pos << 58;

  return encoded;
}


string BwtTransformer::operator()(const Transformer::EncodedKmer& e) const {
  size_t bwt_pos = (e.suffix >> 58);

  string prefix = _decode(e.prefix, prefix_length);
  string suffix = _decode(e.suffix, suffix_length);
  string bwt_rotation = prefix + suffix;

  string bwt = bwt_rotation.substr(bwt_rotation.size() - bwt_pos) + bwt_rotation.substr(0, bwt_rotation.size() - bwt_pos);

  return bwt;
}

END_BIJECTHASH_NAMESPACE
