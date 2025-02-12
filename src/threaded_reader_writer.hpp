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

#ifndef __THREADED_READER_WRITER_HPP__
#define __THREADED_READER_WRITER_HPP__

#include <atomic>
#include <thread>

#include <circular_queue.hpp>

namespace bijecthash {

  template <typename Reader, typename Writer, typename T>
  class ThreadedReaderWriter {

  protected:

    CircularQueue<T> _queue;
    std::vector<Reader> _readers;
    std::vector<Writer> _writers;

    virtual void _pre() {}

    virtual void _post() {}


  public:

    ThreadedReaderWriter(size_t queue_size, size_t nb_readers = 1, size_t nb_writers = 1):
      _queue(queue_size), _readers(), _writers() {
      _readers.reserve(nb_readers);
      _writers.reserve(nb_writers);
    }

    void run() {
      _pre();
      for (auto &w: _writers) {
        w.run();
      }
      for (auto &r: _readers) {
        r.run();
      }
      for (auto &w: _writers) {
        w.join();
      }
      for (auto &r: _readers) {
        r.join();
      }
      _post();
    }

  };

}

#endif
