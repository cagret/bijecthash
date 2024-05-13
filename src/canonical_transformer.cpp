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

#include "canonical_transformer.hpp"

#include "common.hpp"

#include <iostream>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

static char complement(char nucl) {
  if (nucl > 'Z') {
    nucl += 'A' - 'a';
  }
  switch (nucl) {
  case 'A': return 'T';
  case 'C': return 'G';
  case 'G': return 'C';
  case 'T':
  case 'U': return 'A';
  default:
    cerr << "Unable to compute the reverse complement of nucleotide '" << nucl << "'" << endl;
    exit(1);
  }
  return 0;
}

CanonicalTransformer::CanonicalTransformer(size_t kmer_length, size_t prefix_length):
  Transformer(kmer_length, prefix_length, "Canonical") {
  // We need one bit (to store information about the conserved k-mer
  // (between the k-mer and is reverse).
  assert(suffix_length < ((sizeof(uint64_t) << 3) / 2));
}

Transformer::EncodedKmer CanonicalTransformer::operator()(const string &kmer) const {
  assert(kmer.length() == kmer_length);
  string lowest_kmer(kmer_length, '\0');
  size_t i = 0;
  int best = 0;
  while ((best >= 0) && (i < kmer_length)) {
    lowest_kmer[i] = complement(kmer[kmer_length - i - 1]);
    if (best == 0) {
      if (kmer[i] < lowest_kmer[i]) {
        best = -1;
      } else {
        if (kmer[i] > lowest_kmer[i]) {
          best = 1;
        }
      }
    }
    ++i;
  }
  uint64_t m = 0;
  if (best < 0) {
    lowest_kmer = kmer;
  } else {
    m = 1ull << 62;
  }
  EncodedKmer e;
  e.prefix = _encode(lowest_kmer.c_str(), prefix_length);
  e.suffix = _encode(lowest_kmer.c_str() + prefix_length, suffix_length);
  e.suffix |= m;
  return e;
}

string CanonicalTransformer::operator()(const Transformer::EncodedKmer &e) const {
  string kmer = getTransformedKmer(e);
  if (e.suffix >> 62) {
    size_t p = (kmer_length >> 1) + (kmer_length & 1);
    for (size_t i = 0; i < p; ++i) {
      char c = complement(kmer[i]);
      kmer[i] = complement(kmer[kmer_length - i - 1]);
      kmer[kmer_length - i - 1] = c;
    }
  }
  return kmer;
}

END_BIJECTHASH_NAMESPACE
