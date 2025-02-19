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

#include "minimizer_transformer.hpp"

#include "common.hpp"

#include <limits>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

MinimizerTransformer::MinimizerTransformer(size_t kmer_length, size_t prefix_length) :
  Transformer(kmer_length, prefix_length, "Minimizer") {
  assert(suffix_length <= 30);
}

uint64_t MinimizerTransformer::xorshift(uint64_t x) const {
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  return x * 0x2545F4914F6CDD1D;
}

Transformer::EncodedKmer MinimizerTransformer::operator()(const string& kmer) const {
  Transformer::EncodedKmer encoded;
  uint64_t min_hash = numeric_limits<uint64_t>::max();
  size_t minimizer_pos = 0;
  assert(kmer.size() == kmer_length);

  for (size_t i = 0; i + prefix_length <= kmer_length; ++i) {
    uint64_t hash = xorshift(_encode(kmer.c_str() + i, prefix_length));
    if (hash < min_hash) {
      min_hash = hash;
      minimizer_pos = i;
    }
  }
  assert(minimizer_pos + prefix_length <= kmer_length);

  string before = kmer.substr(0, minimizer_pos);
  string minimizer = kmer.substr(minimizer_pos, prefix_length);
  string after = kmer.substr(minimizer_pos + prefix_length);

  DEBUG_MSG("Minimizer: '" << minimizer << "', Before: '" << before << "', After: '" << after << "'");

  string transformed = minimizer + before + after;

  DEBUG_MSG("Transformed: '" << transformed << "', "
            << "Prefix:  '" << transformed.c_str() << "', Prefix length: '" << prefix_length << "', "
            << "Suffix:'" << transformed.c_str() + prefix_length << "', Suffix Length: '" << suffix_length << "', "
            << "Minimiser pos: '" << minimizer_pos << (64 - 6) << "'");

  encoded.prefix = _encode(transformed.c_str(), prefix_length);
  encoded.suffix = _encode(transformed.c_str() + prefix_length, suffix_length);
  encoded.suffix |= minimizer_pos << (64 - 6);

  return encoded;
}


string MinimizerTransformer::operator()(const Transformer::EncodedKmer& encoded) const {
  size_t minimizer_pos = encoded.suffix >> (64 - 6);

  string decoded;
  decoded.reserve(kmer_length);

  string prefix = _decode(encoded.prefix, prefix_length);
  decoded += prefix;

  string suffix = _decode(encoded.suffix, suffix_length);
  decoded += suffix;

  string minimizer = decoded.substr(0, prefix_length);
  string before = decoded.substr(prefix_length, minimizer_pos);
  string after = decoded.substr(minimizer_pos + prefix_length);

  DEBUG_MSG(" Minimizer: '" << minimizer << "', Before: '" << before << "', After: '" << after << "'");

  return before + minimizer + after;
}

END_BIJECTHASH_NAMESPACE
