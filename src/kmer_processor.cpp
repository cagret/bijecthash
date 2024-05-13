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

#include "kmer_processor.hpp"

#include "common.hpp"
#include "kmer_collector.hpp"
#include "locker.hpp"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

atomic_size_t KmerProcessor::_counter = 0;
atomic_size_t KmerProcessor::_running = 0;

KmerProcessor::KmerProcessor(const Settings &settings,
                             KmerIndex &index,
                             CircularQueue<string> &queue):
  _queue(queue), _index(index),
  id(++_counter), settings(settings)
{
  ++_running;
}

void KmerProcessor::_run() {
  while ((KmerCollector::running() > 0) || !_queue.empty()) {
    string kmer;
    DEBUG_MSG("KmerProcessor_" << id << ":"
              << "Running KmerProcessor: " << _running << "/" << _counter << endl
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
      _index.insert(kmer);
    } else {
      this_thread::yield();
      this_thread::sleep_for(1ns);
    }
  }
  --_running;
  DEBUG_MSG("KmerProcessor_" << id << ":"
            << "_running = " << _running << ":"
            << "KmerProcessor_" << id << " has finished.");
}
    
END_BIJECTHASH_NAMESPACE
