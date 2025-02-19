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

#ifndef __LOCKER_HPP__
#define __LOCKER_HPP__

#include <atomic>
#include <cstddef>

namespace bijecthash {

  /**
   * Spin lock mutual exclusion using a single atomic flag (should be
   * more efficient for very small but happening often operations
   * requirign mutual exclusions).
   */
  class SpinlockMutex {

  private:

    /**
     * The mutex flag (true when a thread is running).
     */
    std::atomic_flag _flag;

  public:

    /**
     * Creates a locker with flag initialized to false.
     */
    inline SpinlockMutex(): _flag(ATOMIC_FLAG_INIT) {}

    /**
     * Deleted copy constructor.
     */
    SpinlockMutex(const SpinlockMutex &m) = delete;


    /**
     * Deleted assignment operator.
     */
    SpinlockMutex &operator=(const SpinlockMutex &l) = delete;

    /**
     * Acquire mutex.
     */
    void lock();

    /**
     * Release mutex.
     */
    void unlock();

  };

  /**
   * I/O operations may require mutuam exlusion. The io_mutex instance
   * is provided for that reason.
   */
  extern SpinlockMutex io_mutex;


  /**
   * A simple wrapper class to handle mutexes.
   *
   * Such a guardian wait until acquiring the mutex, then automatically
   * release it at the end of life of the guardian (the current scope).
   *
   * This is a poor implementation of the std::lock_guard defined in the
   * \c <mutex&gt; header.
   */
  template<typename T = SpinlockMutex>
  class LockerGuardian {

  private:

    /**
     * The mutex to handle.
     */
    T& _locker;

  public:

    /**
     * This builds a handler for the given mutex.
     *
     * The guardian construction is completed when the mutex is
     * acquired.
     *
     * \param locker The mutex to handle.
     */
    inline explicit LockerGuardian(T &locker): _locker(locker) {
      _locker.lock();
    }

    /**
     * Deleted copy constructor.
     */
    LockerGuardian(const LockerGuardian &) = delete;

    /**
     * Deleted assignment operator.
     */
    LockerGuardian &operator=(const LockerGuardian &) = delete;

    /**
     * Guardian destructor.
     *
     * This automatically release the acquired mutex.
     */
    inline ~LockerGuardian() {
      _locker.unlock();
    }

  };


  /**
   * A Mutliple Reader - Single Writer (MRSW) mutex.
   *
   * This mutex is not lock-free since while readers must wait some
   * writer to end its writing andvice-versa.
   *
   * This MRSW mutex tries to equilibrate the pending readers and
   * pending writers load.
   */
  class ReadWriteLock {

  private:

    /**
     * The mutex on the ReadWriteLock instance to add a new reader or writer.
     */
    SpinlockMutex _mutex;

    /**
     * The writer flag (true means a writer is writing).
     */
    std::atomic_bool _writer;

    /**
     * The number of current readers
     */
    std::atomic_size_t _nb_readers;

    /**
     * The number of pending readers.
     */
    std::atomic_size_t _nb_pending_readers;

    /**
     * The number of pending writers.
     */
    std::atomic_size_t _nb_pending_writers;

  public:

    /**
     * Deleted copy constructor.
     */
    ReadWriteLock(const ReadWriteLock &rwl) = delete;

    /**
     * Deleted assignment operator.
     */
    ReadWriteLock &operator=(const ReadWriteLock &) = delete;

    /**
     * Create a MRSW lock.
     */
    inline ReadWriteLock():
      _mutex(),
      _writer(false), _nb_readers(0),
      _nb_pending_readers(0), _nb_pending_writers(0)
    {}

    /**
     * Request a reader access.
     *
     * The current thread is added to the pending readers. If there is
     * more pending readers than pending writers, then it requests an
     * exlusive access to the _mutex attribute. The current thread waits
     * for some eventual writer to end its task then it moves from the
     * pending readers to the current (multiple) readers, release its
     * exclusive access and starts to read.
     *
     * Once reading is achieved, the current thread must call the
     * releaseReadAccess() method.
     */
    void requestReadAccess();

    /**
     * Simply remove this thread from the current (multiple) readers.
     */
    void releaseReadAccess();

    /**
     * Request a writer access.
     *
     * The current thread is added to the pending writers. If there is
     * more pending writers than pending readers, then it requests an
     * exlusive access to the _mutex attribute. The current thread waits
     * for any eventual readers to end their tasks then it moves from the
     * pending readers to the current (single) writer, release its exclusive
     * access and starts to write.
     *
     * Once writing is achieved, the current thread must call the
     * releaseWriteAccess() method.
     */
    void requestWriteAccess();

    /**
     * Simply removes this thread from the current (single) writer.
     */
    void releaseWriteAccess();

  };

}

#endif
