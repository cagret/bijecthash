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

#include <kmer_collector.hpp>
#include <kmer_processor.hpp>
#include <locker.hpp>

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

template <typename T>
void queueWatcher(const CircularQueue<T> &queue) {

  double mean = 0;
  double var = 0;
  double nb = 0;
  auto delay = std::chrono::nanoseconds(100);
  int disp = 1024 - 1;
  size_t running_collectors;
  size_t s;
  do {
    running_collectors = KmerCollector::running();
  } while (!running_collectors);
  size_t running_processors = KmerProcessor::running();
  while (running_collectors > 0) {
    s = queue.size();
    mean += s;
    var += s * s;
    if (++disp & 1024) {
      const int v = (s * 100.0 / queue.capacity);
      assert(v >= 0);
      assert(v <= 100.);
      io_mutex.lock();
      std::cerr << "\033[squeue size: [" << std::string(v / 2, '=') << std::string(50-v/2, ' ') << "]"
                << " (" << v << "%)\033[K\033[u";
      io_mutex.unlock();
      disp = 0;
    }
    ++nb;
#ifdef DEBUG
    io_mutex.lock();
    std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
              << "[thread " << std::this_thread::get_id() << "]:"
              << "Watcher:" << running_collectors << " / " << KmerCollector::counter() << " running collectors"
              << " and " << running_processors << " / " << KmerProcessor::counter() << " running processors"
              << ", queue size: " << s << std::endl;
    io_mutex.unlock();
#endif
    std::this_thread::yield();
    std::this_thread::sleep_for(delay);
    running_collectors = KmerCollector::running();
    running_processors = KmerProcessor::running();
  }

  while (running_processors > 0) {
    s = queue.size();
    mean += s;
    var += s * s;
    ++nb;
#ifdef DEBUG
    io_mutex.lock();
    std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
              << "[thread " << std::this_thread::get_id() << "]:"
              << "Watcher:" << running_collectors << " / " << KmerCollector::counter() << " running collectors"
              << " and " << running_processors << " / " << KmerProcessor::counter() << " running processors"
              << ", queue size: " << s << std::endl;
    assert(KmerCollector::running() == 0);
    io_mutex.unlock();
#endif
    std::this_thread::yield();
    std::this_thread::sleep_for(delay);
    running_processors = KmerProcessor::running();
  }

  s = queue.size();
  if (nb != 0) {
    mean /= nb;
    var /= nb;
    var -= mean * mean;
  }
#ifdef DEBUG
  io_mutex.lock();
  std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
            << "[thread " << std::this_thread::get_id() << "]:"
            << "Watcher:" << KmerCollector::running() << " / " << KmerCollector::counter() << " running collectors"
            << " and " << KmerProcessor::running() << " / " << KmerProcessor::counter() << " running processors"
            << ", queue size: " << s << std::endl;
  io_mutex.unlock();
#endif
  assert(s == 0);
  assert(KmerCollector::running() == 0);
  assert(KmerCollector::running() == 0);
  io_mutex.lock();
  std::cerr << "[INFO] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
            << "[thread " << std::this_thread::get_id() << "]:"
            << "Watcher:"
            << "nb of samples: " << nb << std::endl;
  std::cerr << "[INFO] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
            << "[thread " << std::this_thread::get_id() << "]:"
            << "Watcher:"
            << "queue size average: " << mean << std::endl;
  std::cerr << "[INFO] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
            << "[thread " << std::this_thread::get_id() << "]:"
            << "Watcher:"
            << "queue size variance: " << var << std::endl;
  assert(nb != 0);
  io_mutex.unlock();
}

// Local Variables:
// mode:c++
// End:
