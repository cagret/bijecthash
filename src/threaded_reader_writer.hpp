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

#include <cstddef>
#include <vector>

#include <circular_queue.hpp>

namespace bijecthash {


  /**
   * This templated class is wrapper around the multiple
   * readers/multiple writers design pattern.
   *
   * The class uses a circular queue to store the shared data.
   *
   * \tparam Reader The reader class. This class must have a `run()`
   * method that launches a new thread. This thread must read the data
   * of type T from the shared circular queue. Thus a reference to
   * this circular queue must be given as constructor parameter. This
   * class must also provide a `join()` method that blocks the current
   * thread until the threaded reader ends its execution.
   *
   * \tparam Writer The writer class. This class must have a `run()`
   * method that launches a new thread. This thread must write the
   * data of type T into the shared circular queue. Thus a reference
   * to this circular queue must be given as constructor
   * parameter. This class must also provide a `join()` method that
   * blocks the current thread until the threaded writer ends its
   * execution.
   *
   * \tparam T The type of data handled by both the readers and the
   * writers.
   */
  template <typename Reader, typename Writer, typename T>
  class ThreadedReaderWriter {

  protected:

    /**
     * The shared data.
     */
    CircularQueue<T> _queue;

    /**
     * The readers.
     */
    std::vector<Reader> _readers;

    /**
     * The writers.
     */
    std::vector<Writer> _writers;

    /**
     * This method is executed (once) before starting readers and
     * writers threads.
     *
     * Any class that inherits from this one can override this method.
     */
    virtual void _pre() {}

    /**
     * This method is executed (once) after all reader and writer
     * threads have completed.
     *
     * Any class that inherits from this one can override this method.
     */
    virtual void _post() {}


  public:

    /**
     * Builds a Muliple readers/multiple writers handler.
     *
     * \param queue_size The size of the circular queue.
     *
     *  \param nb_readers The estimated number of readers (to
     *  preallocate memory). This is not mandatory but might improve
     *  running time and memory consumtion if there are lots of
     *  readers to handle.
     *
     *  \param nb_writers The estimated number of writers (to
     *  preallocate memory). This is not mandatory but might improve
     *  running time and memory consumtion if there are lots of
     *  writers to handle.
     */
    ThreadedReaderWriter(size_t queue_size, size_t nb_readers = 1, size_t nb_writers = 1):
      _queue(queue_size), _readers(), _writers() {
      _readers.reserve(nb_readers);
      _writers.reserve(nb_writers);
    }

    /**
     * Starts the thread of each reader and writer and waits for each
     * to complete.
     */
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
