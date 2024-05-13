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

#include "lyndon_transformer.hpp"

#include "common.hpp"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

LyndonTransformer::LyndonTransformer(size_t kmer_length, size_t prefix_length):
  Transformer(kmer_length, prefix_length, "Lyndon"){
  assert(suffix_length <= 30);
}

Transformer::EncodedKmer LyndonTransformer::operator()(const string &kmer) const {
  EncodedKmer e;
  size_t lyndon_pos = 0;
  for (size_t i = 1; i < kmer.size(); ++i) {
    if (kmer.substr(i) + kmer.substr(0, i) < kmer.substr(lyndon_pos) + kmer.substr(0, lyndon_pos)) {
      lyndon_pos = i;
    }
  }

  DEBUG_MSG("Lyndon rotation index: " << lyndon_pos);

  assert(lyndon_pos < 64); // lyndon_pos can be encoded in only 6 bits
  string lyndon_rotation;
  lyndon_rotation.reserve(kmer.size());
  lyndon_rotation = kmer.substr(lyndon_pos) + kmer.substr(0, lyndon_pos);

  DEBUG_MSG("Lyndon rotation: '" << lyndon_rotation << "'");

  e.prefix = _encode(lyndon_rotation.c_str(), prefix_length);
  e.suffix = _encode(lyndon_rotation.c_str() + prefix_length, suffix_length);
  e.suffix |= lyndon_pos<<58;
  return e;
}

string LyndonTransformer::operator()(const Transformer::EncodedKmer &e) const {
  size_t lyndon_pos = e.suffix >> 58;
  string lyndon_rotation = _decode(e.prefix, prefix_length) + _decode(e.suffix, suffix_length);
  string kmer = lyndon_rotation.substr(lyndon_rotation.size() - lyndon_pos) + lyndon_rotation.substr(0, lyndon_rotation.size() - lyndon_pos);
  return kmer;
}

END_BIJECTHASH_NAMESPACE
