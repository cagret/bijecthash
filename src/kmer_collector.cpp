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

#include "kmer_collector.hpp"

#include "common.hpp"
#include "exception.hpp"
#include "locker.hpp"

#include <iostream>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

KmerCollector::KmerCollector(size_t k, const string &filename, CircularQueue<string> &queue, bool verbose):
  ThreadedProcessorHelper<KmerCollector, string>(queue),
  _reader(k, filename, verbose)
{
  if (!_reader.isOpen()) {
    Exception e;
    e << "Error: Unable to open fasta/fastq file '" << filename << "'\n";
    throw e;
  }
}

void KmerCollector::_run() {
  DEBUG_MSG("KmerCollector_" << id << ":"
            << "Starting file = '" << _reader.getFilename() << "' processing.");

  while (_reader.nextKmer()) {
    string kmer = _reader.getCurrentKmer();
    if (_reader.getCurrentKmerID(false) == 1) {
      DEBUG_MSG("KmerCollector " << id << ":"
                << "New sequence: '" << _reader.getCurrentSequenceDescription() << "'");
    }
    DEBUG_MSG("KmerCollector_" << id << ":"
              << "k-mer '" << _reader.getCurrentKmer()
              << " (abs_ID: " << _reader.getCurrentKmerID()
              << ",  rel_ID: " << _reader.getCurrentKmerID(false)
              << ")");

    _process(kmer);
    while (!_queue.push(kmer)) {
      DEBUG_MSG("KmerCollector_" << id << ":"
                << "Unable to push k-mer '" << kmer << "." << '\n'
                << MSG_DBG_HEADER
                << "KmerCollector_" << id << ":"
                << "queue size: " << _queue.size()
                << " (" << (_queue.empty() ? "empty" : "not empty")
                << ", " << (_queue.full() ? "full" : "not full") << ").");
      this_thread::yield();
      this_thread::sleep_for(10ns);
    }
    DEBUG_MSG("KmerCollector_" << id << ":"
              << "k-mer '" << kmer << " pushed successfully." << '\n'
              << MSG_DBG_HEADER
              << "KmerCollector_" << id << ":"
              << "queue size: " << _queue.size()
              << " (" << (_queue.empty() ? "empty" : "not empty")
              << ", " << (_queue.full() ? "full" : "not full") << ").");
  }
  DEBUG_MSG("KmerCollector_" << id << ":"
            << "running: " << running() << ":"
            << "file '" << _reader.getFilename() << "' processed.");
}

void KmerCollector::_process(string &__UNUSED__(kmer)) {}

END_BIJECTHASH_NAMESPACE
