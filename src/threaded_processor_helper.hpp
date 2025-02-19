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

#ifndef __THREADED_PROCESSOR_HELPER_HPP__
#define __THREADED_PROCESSOR_HELPER_HPP__

#include <atomic>
#include <cstddef>
#include <thread>

#include <circular_queue.hpp>

namespace bijecthash {

  /**
   * A thread helper that defines a processor which uses a circular
   * queue to share data.
   *
   * This helper class allows to run the process in a dedicated
   * thread.
   */
  template <typename C, typename T>
  class ThreadedProcessorHelper {

  private:

    /**
     * The total number of created instances.
     */
    static std::atomic_size_t _counter;

    /**
     * The total number of running instances (even if thread is not
     * started).
     */
    static std::atomic_size_t _running;

    /**
     * The thread running this data processor.
     */
    std::thread _thread;

    /**
     * This thread running state.
     */
    bool _is_running;

    /**
     * Load or Store some data from/into the queue.
     *
     * This method must be overriden by any derived class.
     */
    virtual void _run() = 0;

  protected:

    /**
     * The circular queue used by this data processor.
     */
    CircularQueue<T> &_queue;

  public:

    /**
     * This processor id (in the set of the
     * ThreadedProcessorHelper<C,T>)
     */
    const size_t id;

    /**
     * Builds a processor helper using a given queue.
     *
     * The counter of handled processor is updated.
     *
     * \param queue The queue storing the data to exchange.
     */
    ThreadedProcessorHelper(CircularQueue<T> &queue): _is_running(false), _queue(queue), id(++_counter) {
    }

    /**
     * Builds a processor helper by copy of an existing one.
     *
     * The counter of handled processor is updated.
     *
     * The new processor is not started, whatever the state of the one
     * handled by the given helper.
     *
     * \param t The processor helper to copy (the processors will
     * share the same circular queue).
     */
    ThreadedProcessorHelper(const ThreadedProcessorHelper<C,T> &t): _is_running(false), _queue(t._queue), id(++_counter) {
    }

    /**
     * Destroy the current helper and update the counter of running
     * processors if so.
     */
    ~ThreadedProcessorHelper() {
      if (_is_running) {
        --_running;
      }
    }

    ThreadedProcessorHelper<C, T> &operator=(const ThreadedProcessorHelper<C,T> &t) = delete;

    /**
     * Start the dedicated thread if not already running.
     *
     * \see The k-mer dequeuing and processing is performed by the
     * private _run() method.
     */
    void run() {
      if (!_is_running) {
        _thread = std::thread(&ThreadedProcessorHelper<C,T>::_run, this);
        ++_running;
        _is_running = true;
      }
    }

    /**
     * This join the thread when it ends.
     */
    void join() {
      _thread.join();
      --_running;
      _is_running = false;
    }

    /**
     * Gets the running state
     *
     * \return Returns true if this is currently running in a dedicated thread.
     */
    bool isRunning() {
      return _is_running;
    }

    /**
     * Get the number of create instances.
     *
     * \return Return the number of created instances.
     */
    static size_t counter() {
      return _counter;
    }

    /**
     * Get the total number of running instances.
     *
     * \return Return the total number of running instances. An instance
     * is considered as running from its creation and until it reaches
     * the end of the _run() method.
     */
    static size_t running() {
      return _running;
    }

  };

  template <typename C, typename T>
  std::atomic_size_t ThreadedProcessorHelper<C, T>::_counter(0);

  template <typename C, typename T>
  std::atomic_size_t ThreadedProcessorHelper<C, T>::_running(0);

}

#endif
