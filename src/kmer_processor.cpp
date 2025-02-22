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

#include "kmer_processor.hpp"

#include "common.hpp"
#include "kmer_collector.hpp"
#include "locker.hpp"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

KmerProcessor::KmerProcessor(CircularQueue<string> &queue):
  ThreadedProcessorHelper<KmerProcessor, string>(queue) {}

void KmerProcessor::_run() {
  while ((KmerCollector::running() > 0) || !_queue.empty()) {
    string kmer;
    DEBUG_MSG("KmerProcessor_" << id << ":"
              << "Running KmerProcessor: " << running() << "/" << counter() << '\n'
              << MSG_DBG_HEADER << "KmerProcessor_" << id << ":"
              << "queue size: " << _queue.size());
    bool ok = _queue.pop(kmer);
    while (!_queue.empty() && !ok) {
      ok = _queue.pop(kmer);
      DEBUG_MSG("KmerProcessor_" << id << ":"
                << "Unable to pop any kmer (queue size: " << _queue.size() << ").");
      this_thread::yield();
      this_thread::sleep_for(1ns);
    }
    if (ok) {
      DEBUG_MSG("KmerProcessor_" << id << ":"
                << "k-mer '" << kmer << "' successfully popped.");
      _process(kmer);
    } else {
      this_thread::yield();
      this_thread::sleep_for(1ns);
    }
  }
  DEBUG_MSG("KmerProcessor_" << id << ":"
            << "running: " << running() << ":"
            << "KmerProcessor_" << id << " has finished.");
}

void KmerProcessor::_process(string &__UNUSED__(kmer)) {}

END_BIJECTHASH_NAMESPACE
