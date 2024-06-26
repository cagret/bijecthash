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

#include "kmer_collector.hpp"

#include "common.hpp"
#include "file_reader.hpp"
#include "locker.hpp"
#include "transformer.hpp"

#ifdef DEBUG
#  include <bitset>
#endif
#include <iostream>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

atomic_size_t KmerCollector::_counter = 0;
atomic_size_t KmerCollector::_running = 0;

KmerCollector::KmerCollector(const Settings &settings,
                             const string &filename,
                             CircularQueue<string> &queue,
                             LcpStats &lcp_stats):
  _lcp_stats(lcp_stats), _queue(queue),
  id(++_counter), settings(settings), filename(filename)
{
  ++_running;
}

#ifdef __builtin_clzll_available
#  undef __builtin_clzll_available
#endif
#ifdef __has_builtin
#  if __has_builtin(__builtin_clzll)
#    define __builtin_clzll_available
#  endif
#endif

#ifdef DEBUG
#define BIT_MASK_POS(v, mask, offset)           \
  io_mutex.lock();                              \
  cerr << "v = " << bitset<64>(v) << endl;      \
  cerr << "m = " << bitset<64>(mask) << endl;   \
  io_mutex.unlock();                            \
  if (v & mask) {                               \
    v &= mask;                                  \
  } else {                                      \
    p += offset;                                \
  }                                             \
  io_mutex.lock();                              \
  cerr << "v = " << bitset<64>(v) << endl;      \
  cerr << "p = " << p << endl;                  \
  io_mutex.unlock();                            \
  (void)0
#else
#define BIT_MASK_POS(v, mask, offset)           \
  if (v & mask) {                               \
    v &= mask;                                  \
  } else {                                      \
    p += offset;                                \
  }                                             \
  (void)0
#endif


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

static size_t _LCP(const Transformer::EncodedKmer &e1, const Transformer::EncodedKmer &e2, size_t k, size_t k1) {
  const size_t nb_bits = sizeof(uint64_t) << 3;
  uint64_t v = (e1.prefix ^ e2.prefix);
  DEBUG_MSG("e1.prefix = " << bitset<64>(e1.prefix) << endl
            << MSG_DBG_HEADER << "e2.prefix = " << bitset<64>(e2.prefix) << endl
            << MSG_DBG_HEADER << "e1 ^ e2   = " << bitset<64>(e1.prefix ^ e2.prefix) << endl
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
    DEBUG_MSG("k = " << k << " = " << k1 << " + " << k2 << endl
              << MSG_DBG_HEADER << "e1.suffix = " << bitset<64>(e1.suffix) << endl
              << MSG_DBG_HEADER << "e2.suffix = " << bitset<64>(e2.suffix) << endl
              << MSG_DBG_HEADER << "e1 ^ e2   = " << bitset<64>(e1.suffix ^ e2.suffix) << endl
              << MSG_DBG_HEADER << "mask      = " << bitset<64>(m) << endl
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
  return res;
}

void KmerCollector::_run() {
  DEBUG_MSG("KmerCollector_" << id << ":"
            << "Starting file = '" << filename << "' processing.");
  FileReader reader(settings.length, filename, settings.verbose);

  if (!reader.isOpen()) {
    io_mutex.lock();
    cerr << "Error: Unable to open fasta/fastq file '" << filename << "'" << endl;
    io_mutex.unlock();
    exit(1);
  }

#ifdef DEBUG
  DEBUG_MSG("Test of the LCP");
  Transformer::EncodedKmer e1, e2;
  e1.prefix = e1.suffix = e2.prefix = e2.suffix = 0;
  size_t expected_lcp = (settings.length << 1) - 1;
  for (e2.suffix = 1; e2.suffix < (1ull << ((settings.length - settings.prefix_length) << 1)); e2.suffix <<= 1) {
    size_t lcp = _LCP(e1, e2, settings.length, settings.prefix_length);
    DEBUG_MSG("_LCP({" << e1.prefix << ", " << e1.suffix << "},"
              << " {" << e2.prefix << ", " << e2.suffix << "}) = " << lcp
              << " (expecting " << (expected_lcp >> 1) << ")");
    assert(lcp == (expected_lcp >> 1));
    --expected_lcp;
  }
  DEBUG_MSG("================================");
  for (e2.prefix = 1; e2.prefix < (1ull << (settings.prefix_length << 1)); e2.prefix <<= 1) {
    size_t lcp = _LCP(e1, e2, settings.length, settings.prefix_length);
    DEBUG_MSG("_LCP({" << e1.prefix << ", " << e1.suffix << "},"
              << " {" << e2.prefix << ", " << e2.suffix << "}) = " << lcp
              << " (expecting " << (expected_lcp >> 1) << ")");
    assert(lcp == (expected_lcp >> 1));
    --expected_lcp;
  }
  DEBUG_MSG("================================");
#endif
  
  Transformer::EncodedKmer prev_transformed_kmer, current_transformed_kmer;
  prev_transformed_kmer.prefix = prev_transformed_kmer.suffix = 0;
  shared_ptr<const Transformer> transformer = settings.transformer();
  size_t LCP_avg = 0;
  size_t LCP_var = 0;
  size_t nb_kmers = 0;
  while (reader.nextKmer()) {
    const string &kmer = reader.getCurrentKmer();
    current_transformed_kmer = (*transformer)(kmer);
    if (reader.getCurrentKmerID(false) == 1) {
      DEBUG_MSG("KmerCollector " << id << ":"
                << "New sequence: '" << reader.getCurrentSequenceDescription() << "'");
    }
    DEBUG_MSG("KmerCollector_" << id << ":"
              << "k-mer '" << reader.getCurrentKmer()
              << " (abs_ID: " << reader.getCurrentKmerID()
              << ",  rel_ID: " << reader.getCurrentKmerID(false)
              << ")");

    if (reader.getCurrentKmerID(false) != 1) {
      size_t lcp = _LCP(prev_transformed_kmer, current_transformed_kmer, settings.length, settings.prefix_length);
      LCP_avg += lcp;
      LCP_var += lcp * lcp;
      ++nb_kmers;
    }

    while (!_queue.push(kmer)) {
      DEBUG_MSG("KmerCollector_" << id << ":"
                << "Unable to push k-mer '" << kmer << "." << endl
                << MSG_DBG_HEADER
                << "KmerCollector_" << id << ":"
                << "queue size: " << _queue.size()
                << " (" << (_queue.empty() ? "empty" : "not empty")
                << ", " << (_queue.full() ? "full" : "not full") << ").");
      this_thread::yield();
      this_thread::sleep_for(10ns);
    }
    DEBUG_MSG("KmerCollector_" << id << ":"
              << "k-mer '" << kmer << " pushed successfully." << endl
              << MSG_DBG_HEADER
              << "KmerCollector_" << id << ":"
              << "queue size: " << _queue.size()
              << " (" << (_queue.empty() ? "empty" : "not empty")
              << ", " << (_queue.full() ? "full" : "not full") << ").");
    prev_transformed_kmer = current_transformed_kmer;
  }

  _lcp_stats.nb_kmers = nb_kmers;
  _lcp_stats.average = double(LCP_avg) / nb_kmers;
  _lcp_stats.variance = double(LCP_var) / nb_kmers;
  _lcp_stats.variance -= _lcp_stats.average * _lcp_stats.average;

  DEBUG_MSG("KmerCollector_" << id << ":"
            << "file '" << filename << "':"
            << "LCP ~ N(" << _lcp_stats.average << ", " << _lcp_stats.variance << ")"
            << " computed using " << _lcp_stats.nb_kmers << " k-mers.");

  --_running;
  DEBUG_MSG("KmerCollector_" << id << ":"
            << "_running = " << _running << ":"
            << "file '" << filename << "' processed.");
}

END_BIJECTHASH_NAMESPACE
