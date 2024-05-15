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

#include "lcp_stats.hpp"

#include "common.hpp"

#ifdef DEBUG
#  include <bitset>
#endif
#include <iostream>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

#ifdef __builtin_clzll_available
#  undef __builtin_clzll_available
#endif
#ifdef __has_builtin
#  if __has_builtin(__builtin_clzll)
#    define __builtin_clzll_available
#  endif
#endif

#define BIT_MASK_POS(v, mask, offset)           \
  DEBUG_MSG("v = " << bitset<64>(v));           \
  DEBUG_MSG("m = " << bitset<64>(mask));        \
  if (v & mask) {                               \
    v &= mask;                                  \
  } else {                                      \
    p += offset;                                \
  }                                             \
  DEBUG_MSG("v = " << bitset<64>(v));           \
  DEBUG_MSG("p = " << p)


static int clz(uint64_t v) {
#ifdef __builtin_clzll_available
  return __builtin_clzll(v);
#else
  int p = 0;
  BIT_MASK_POS(v, 0xFFFFFFFF00000000, 32);
  BIT_MASK_POS(v, 0xFFFF0000FFFF0000, 16);
  BIT_MASK_POS(v, 0xFF00FF00FF00FF00, 8);
  BIT_MASK_POS(v, 0xF0F0F0F0F0F0F0F0, 4);
  BIT_MASK_POS(v, 0xCCCCCCCCCCCCCCCC, 2);
  BIT_MASK_POS(v, 0xAAAAAAAAAAAAAAAA, 1);
  return p;
#endif
}

size_t LcpStats::LCP(const Transformer::EncodedKmer &e1, const Transformer::EncodedKmer &e2, size_t k, size_t k1) {
  const size_t nb_bits = sizeof(uint64_t) << 3;
  uint64_t v = (e1.prefix ^ e2.prefix);
  DEBUG_MSG("e1.prefix = " << bitset<64>(e1.prefix) << '\n'
            << MSG_DBG_HEADER << "e2.prefix = " << bitset<64>(e2.prefix) << '\n'
            << MSG_DBG_HEADER << "e1 ^ e2   = " << bitset<64>(e1.prefix ^ e2.prefix) << '\n'
            << MSG_DBG_HEADER << "v         = " << bitset<64>(v));
  size_t res = 0;
  if (v) {
    int p = nb_bits - clz(v);
    DEBUG_MSG("First leftmost bit set is at position " << p);
    res = ((k1 << 1) - p) >> 1;
  } else {
    size_t k2 = k - k1;
    uint64_t m = (((k2 << 1) < nb_bits) ? (1ull << (k2 << 1)) - 1 : uint64_t(-1));
    v = (e1.suffix ^ e2.suffix) & m;
    DEBUG_MSG("k = " << k << " = " << k1 << " + " << k2 << '\n'
              << MSG_DBG_HEADER << "e1.suffix = " << bitset<64>(e1.suffix) << '\n'
              << MSG_DBG_HEADER << "e2.suffix = " << bitset<64>(e2.suffix) << '\n'
              << MSG_DBG_HEADER << "e1 ^ e2   = " << bitset<64>(e1.suffix ^ e2.suffix) << '\n'
              << MSG_DBG_HEADER << "mask      = " << bitset<64>(m) << '\n'
              << MSG_DBG_HEADER << "v         = " << bitset<64>(v));
    if (v) {
      int p = nb_bits - clz(v);
      DEBUG_MSG("First leftmost bit set is at position " << p);
      res = (((k2 << 1) - p) >> 1) + k1;
    } else {
      res = k;
    }
  }
  DEBUG_MSG("res = " << res);

  average += res;
  variance += res * res;
  ++nb_kmers;

  return res;
}

END_BIJECTHASH_NAMESPACE
