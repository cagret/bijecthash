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

#include "transformer.hpp"

#include "common.hpp"
#include "locker.hpp"

#include <iostream>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

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
      io_mutex.lock();
      cerr << "Error: unable to encode the given k-mer (" << string(dna_str, n) << ")"
           << " since it contains the symbol '" << dna_str[i] << "'"
           << " which is neither A nor C nor G nor T." << endl;
      io_mutex.unlock();
      exit(1);
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
