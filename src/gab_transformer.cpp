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

#include "gab_transformer.hpp"

#include "common.hpp"

#ifdef DEBUG
#  include <bitset>
#endif
#include <random>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

// The following functions (_f64() and _multiplicativeInverse()) are
// adapted from
// https://lemire.me/blog/2017/09/18/computing-the-inverse-of-odd-integers/
static uint64_t _f64(uint64_t a, uint64_t x) {
  return x * (2 - a * x);
}

uint64_t _multiplicativeInverse(uint64_t a, size_t sigma) {
  // We want to find, given and odd value a, the rev_a such that:
  //
  // a * rev_a = 1 mod 2^sigma
  //
  // The extended euclidian algorithm solves the following equation
  // given a and b (Bézout's Lemma ensure the existance of some
  // integer x and y):
  //
  // a * x + b * y = gcd(a, b).
  //
  // Since for any odd value of a, gcd(a, 2^sigma) = 1, solving
  //
  // a * rev_a = 1 mod 2^sigma
  //
  // is equivalent to solving the extended euclidian algorithm for:
  // prefix_length * 2
  // a * rev_a + 2^sigma * y = gcd(a, 2^sigma)
  assert(a & 1);
  uint64_t x = (3 * a) ^ 2; // 5 bits
  x = _f64(a, x); // 10 bits
  x = _f64(a, x); // 20 bits
  x = _f64(a, x); // 40 bits
  x = _f64(a, x); // 80 bits
  uint64_t mask = ((sigma < 64) ? ((1ull << sigma) - 1) : uint64_t(-1));
  DEBUG_MSG("a = " << a << '\n'
            << MSG_DBG_HEADER << "a' = " << x << '\n'
            << MSG_DBG_HEADER << "a * a' = " << (a * x) << '\n'
            << MSG_DBG_HEADER << "(a * a') & " << bitset<64>(mask) << " = " << ((a * x) & mask));
  assert(((a * x) & mask) == 1);
  return x & mask;
}

static uint64_t _rand() {
  random_device rd;
  mt19937 g(rd());
  return g();
}

static uint64_t _setCorrectOddCoefficient(uint64_t v) {
  if (!v) {
    v = _rand();
  }
  if ((v & 1) == 0) {
    ++v;
  }
  return v;
}

static uint64_t _setCorrectBitMask(uint64_t v, uint64_t mask) {
  if (!v) {
    v = _rand();
  }
  return v & mask;
}

GaBTransformer::GaBTransformer(size_t kmer_length, size_t prefix_length, uint64_t a, uint64_t b):
  Transformer(kmer_length, prefix_length, "GaB"),
  _rotation_offset(kmer_length > 32 ? 32 : kmer_length),
  _rotation_mask((1ull << kmer_length) - 1ull),
  _kmer_mask((_rotation_mask << _rotation_offset) | _rotation_mask),
  _prefix_shift((((kmer_length > 32) ? 32 : kmer_length) - prefix_length) << 1),
  _suffix_mask((kmer_length > 32) ? ((1ull << ((kmer_length - 32) << 1)) - 1) : ((1ull << (suffix_length << 1)) - 1)),
  _a(_setCorrectOddCoefficient(a)), _rev_a(_multiplicativeInverse(_a, kmer_length > 32 ? 64 : 2 * kmer_length)), _b(_setCorrectBitMask(b, _kmer_mask))
{
  string *desc_ptr = const_cast<string *>(&(this->description));
  *desc_ptr += "(" + to_string(_a) + "," + to_string(_b) + ")";
}

uint64_t GaBTransformer::_rotate(uint64_t s) const {
  s = (((s << _rotation_offset) | (s >> _rotation_offset)) & _kmer_mask);
  return s;
}

uint64_t GaBTransformer::_G(uint64_t s) const {
  DEBUG_MSG("s = " << s << ", " << "_a = " << _a << ", " << "_b = " << _b;
            uint64_t rotation = _rotate(s);
            cerr << MSG_DBG_HEADER << "_rotate(s) = " << rotation << '\n'
            << MSG_DBG_HEADER << "_rotate(s) ^ _b = " << (rotation ^ _b) << '\n'
            << MSG_DBG_HEADER << "_a * (_rotate(s) ^ _b) = " << (_a * (rotation ^ _b)) << '\n'
            << MSG_DBG_HEADER << "(_a * (_rotate(s) ^ _b)) & _kmer_mask = " << ((_a * (rotation ^ _b)) & _kmer_mask));
  return ((_a * (_rotate(s) ^ _b)) & _kmer_mask);
}

uint64_t GaBTransformer::_G_rev(uint64_t s) const {
  DEBUG_MSG("s = " << s << '\n'
            << MSG_DBG_HEADER << "_rev_a * s = " << "(" << _rev_a << " * " << s << ") = " << (_rev_a * s) << '\n'
            << MSG_DBG_HEADER << "(_rev_a * s) & _kmer_mask = " << ((_rev_a * s) & _kmer_mask) << '\n'
            << MSG_DBG_HEADER << "((_rev_a * s) & _kmer_mask) ^ _b = " << (((_rev_a * s) & _kmer_mask) ^ _b) << '\n'
            << MSG_DBG_HEADER << "_rotate(((_rev_a * s) & _kmer_mask) ^ _b) = " << _rotate(((_rev_a * s) & _kmer_mask) ^ _b));
  return _rotate(((_rev_a * s) & _kmer_mask) ^ _b);
}

Transformer::EncodedKmer GaBTransformer::operator()(const std::string &kmer) const {
  EncodedKmer e;
  if (kmer_length <= 32) {
    uint64_t v = _encode(kmer.c_str(), kmer_length);
#ifdef DEBUG
    uint64_t orig = v;
#endif
    v = _G(v);
#ifdef DEBUG
    uint64_t rev_v = _G_rev(v);
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
    uint64_t prefix_transformed = _G(prefix);
    e.prefix = prefix_transformed >> _prefix_shift;
    e.suffix = prefix_transformed << (prefix_length << 1) | suffix;
  }
  return e;
}

std::string GaBTransformer::operator()(const Transformer::EncodedKmer &e) const {
  if (kmer_length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    v = _G_rev(v);
    return _decode(v, kmer_length);
  } else {
    uint64_t u = (e.prefix << _prefix_shift) | (e.suffix >> (prefix_length << 1));
    u = _G_rev(u);
    uint64_t v = e.suffix & _suffix_mask;
    return _decode(u, 32) + _decode(v, kmer_length - 32);
  }
}

std::string GaBTransformer::getTransformedKmer(const Transformer::EncodedKmer &e) const {
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
