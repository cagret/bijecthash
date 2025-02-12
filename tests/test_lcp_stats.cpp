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

#ifdef NDEBUG
#  undef NDEBUG
#endif
#include <cassert>
#include <iostream>

#include "lcp_stats.hpp"
#include "transformer.hpp"

using namespace std;
using namespace bijecthash;

const size_t max_nb_bits = sizeof(uint64_t) << 3;
const size_t max_nb_symbols = max_nb_bits >> 1;

void test_lcp_stats(size_t length, size_t prefix_length) {

  cout << "Test of the LCP for " << length << "-mers using prefix lenght " << prefix_length << endl;

  assert(prefix_length > 0);
  assert(prefix_length <= max_nb_symbols);
  assert(length > prefix_length);
  size_t suffix_length = length - prefix_length;
  assert(suffix_length <= max_nb_symbols);

  Transformer::EncodedKmer e1, e2;
  LcpStats lcp_stats;
  lcp_stats.start();
  assert(lcp_stats.nb_kmers == 0);

  e1.prefix = e1.suffix = e2.prefix = e2.suffix = 0;

  size_t expected_lcp = (length << 1) - 1;

  uint64_t max_v = (1ull << suffix_length << suffix_length);
  if (max_v) {
    for (e2.suffix = 1; e2.suffix < max_v; e2.suffix <<= 1) {
      size_t lcp = lcp_stats.LCP(e1, e2, length, prefix_length);
      cout << "LCP({" << e1.prefix << ", " << e1.suffix << "},"
           << " {" << e2.prefix << ", " << e2.suffix << "}) = " << lcp
           << " (expecting " << (expected_lcp >> 1) << ")" << endl;
      assert(lcp == (expected_lcp >> 1));
      --expected_lcp;
    }
  } else {
    expected_lcp -= max_nb_symbols * 2;
  }
  cout << "expected_lcp = " << expected_lcp << endl;
  assert((expected_lcp >> 1) + 1 == prefix_length);

  cout << "================================" << endl;

  max_v = (1ull << prefix_length << prefix_length);
  if (max_v) {
    for (e2.prefix = 1; e2.prefix < max_v; e2.prefix <<= 1) {
      size_t lcp = lcp_stats.LCP(e1, e2, length, prefix_length);
      cout << "LCP({" << e1.prefix << ", " << e1.suffix << "},"
           << " {" << e2.prefix << ", " << e2.suffix << "}) = " << lcp
           << " (expecting " << (expected_lcp >> 1) << ")" << endl;
      assert(lcp == (expected_lcp >> 1));
      --expected_lcp;
    }
  } else {
    expected_lcp -= max_nb_symbols * 2;
  }
  cout << "expected_lcp = " << expected_lcp << endl;
  assert(expected_lcp == (size_t) -1);

  cout << "================================" << endl;

  cout << endl;
}

int main() {

  for (size_t l = 2; l <= max_nb_symbols * 2; l += (l == 2 ? 6 : 8)) {
    const size_t min_p = ((l <= max_nb_symbols) ? 1 : (l - max_nb_symbols));
    const size_t max_p = ((l <= max_nb_symbols) ? (l - 1) : max_nb_symbols);
    for (size_t p = min_p; p <= max_p; ++p) {
      test_lcp_stats(l, p);
    }
  }
  return 0;

}
