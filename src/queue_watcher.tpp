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

#include <cassert>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>

#include <threaded_processor_helper.hpp>
#include <locker.hpp>

namespace bijecthash {

  template <typename Reader, typename Writer, typename T>
  void queueWatcher(const CircularQueue<T> &queue) {

    double mean = 0;
    double var = 0;
    double nb = 0;
    auto delay = std::chrono::nanoseconds(100);
    int disp = 1024 - 1;
    size_t running_writers;
    size_t s;

    do {
      running_writers = ThreadedProcessorHelper<Writer, T>::running();
    } while (!running_writers);

    size_t running_readers = ThreadedProcessorHelper<Reader, T>::running();
    while (running_writers > 0) {
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
                << "Watcher: " << running_writers << " / " << ThreadedProcessorHelper<Writer, T>::counter() << " running writers"
                << " and " << running_readers << " / " << ThreadedProcessorHelper<Reader, T>::counter() << " running readers"
                << ", queue size: " << s << std::endl;
      io_mutex.unlock();
#endif
      std::this_thread::yield();
      std::this_thread::sleep_for(delay);
      running_writers = ThreadedProcessorHelper<Writer, T>::running();
      running_readers = ThreadedProcessorHelper<Reader, T>::running();
    }

    while (running_readers > 0) {
      s = queue.size();
      mean += s;
      var += s * s;
      ++nb;
#ifdef DEBUG
      io_mutex.lock();
      std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                << "[thread " << std::this_thread::get_id() << "]:"
                << "Watcher: " << running_writers << " / " << ThreadedProcessorHelper<Writer, T>::counter() << " running writers"
                << " and " << running_readers << " / " << ThreadedProcessorHelper<Reader, T>::counter() << " running readers"
                << ", queue size: " << s << std::endl;
      assert((ThreadedProcessorHelper<Writer, T>::running()) == 0);
      io_mutex.unlock();
#endif
      std::this_thread::yield();
      std::this_thread::sleep_for(delay);
      running_readers = ThreadedProcessorHelper<Reader, T>::running();
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
              << "Watcher: " << ThreadedProcessorHelper<Writer, T>::running() << " / " << ThreadedProcessorHelper<Writer, T>::counter() << " running writers"
              << " and " << ThreadedProcessorHelper<Reader, T>::running() << " / " << ThreadedProcessorHelper<Reader, T>::counter() << " running readers"
              << ", queue size: " << s << std::endl;
    io_mutex.unlock();
#endif
    assert(s == 0);
    assert((ThreadedProcessorHelper<Writer, T>::running()) == 0);
    assert((ThreadedProcessorHelper<Reader, T>::running()) == 0);
    io_mutex.lock();
    std::cerr << "[INFO] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
              << "[thread " << std::this_thread::get_id() << "]:"
              << "Watcher:"
              << "nb of samples: " << nb << '\n'
              << "[INFO] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
              << "[thread " << std::this_thread::get_id() << "]:"
              << "Watcher:"
              << "queue size average: " << mean << '\n'
              << "[INFO] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
              << "[thread " << std::this_thread::get_id() << "]:"
              << "Watcher:"
              << "queue size variance: " << var << std::endl;
    assert(nb != 0);
    io_mutex.unlock();
  }

}
// Local Variables:
// mode:c++
// End:
