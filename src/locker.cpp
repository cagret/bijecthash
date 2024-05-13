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

#include "locker.hpp"

#include "common.hpp"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

SpinlockMutex io_mutex;

void SpinlockMutex::lock() {
  // From https://en.cppreference.com/w/cpp/atomic/atomic_flag
  while (_flag.test_and_set(memory_order_acquire)) {
    // Since C++20, it is possible to update atomic_flag's
    // value only when there is a chance to acquire the lock.
    // See also: https://stackoverflow.com/questions/62318642
#if defined(__cpp_lib_atomic_flag_test)
    while (lock.test(memory_order_relaxed)); // test lock
#endif
  }
}

void SpinlockMutex::unlock() {
  _flag.clear(memory_order_release);
}

void ReadWriteLock::requestReadAccess() {
  DEBUG_MSG("Request a read access for " << this);
  ++_nb_pending_readers;
  DEBUG_MSG("There is " << (_writer.load()  ? "one" : "no") << " writer process, "
            << _nb_pending_writers.load() << " pending writers"
            << ", " << _nb_pending_readers.load() << " pending readers"
            << " including " << this
            << " and " << _nb_readers.load() << " readers");
  // Give priority to pending writers (while they are the most abundant)
  while (_nb_pending_writers.load() > _nb_pending_readers.load());
  DEBUG_MSG("Request a locking access for " << this);
  _mutex.lock(); // Read & write requests are locked
  DEBUG_MSG("Got the locking access for " << this);
  while (_writer.load()); // wait until no more writer is working
  ++_nb_readers;
  assert(_nb_pending_readers.load() > 0); // this thread is a pending reader...
  --_nb_pending_readers;
  DEBUG_MSG("There is " << (_writer.load()  ? "one" : "no") << " writer process, "
            << _nb_pending_writers.load() << " pending writers"
            << ", " << _nb_pending_readers.load() << " pending readers"
            << " and " << _nb_readers.load() << " readers"
            << " including " << this);
  DEBUG_MSG("Release the locking access from " << this);
  _mutex.unlock(); // Read & write requests are unlocked
  DEBUG_MSG("Read access granted for " << this);
}

void ReadWriteLock::releaseReadAccess() {
  DEBUG_MSG("Releasing the read access for this (" << this << ") reader");
  assert(_nb_readers > 0); // This thread must have read grants.
  --_nb_readers;
  DEBUG_MSG("Read access released for this (" << this << ") reader.");
  DEBUG_MSG("There is " << (_writer.load()  ? "one" : "no") << " writer process, "
            << _nb_pending_writers.load() << " pending writers"
            << ", " << _nb_pending_readers.load() << " pending readers"
            << " and " << _nb_readers.load() << " readers"
            << " not including " << this << " anymore.");
}

void ReadWriteLock::requestWriteAccess() {
  DEBUG_MSG("Request a write access for this (" << this << ") writer");
  ++_nb_pending_writers;
  DEBUG_MSG("There is " << (_writer.load()  ? "one" : "no") << " writer process, "
            << _nb_pending_writers.load() << " pending writers"
            << " including " << this
            << ", " << _nb_pending_readers.load() << " pending readers"
            << " and " << _nb_readers.load() << " readers"
            );
  // Give priority to pending readers (while they are the most abundant)
  while (_nb_pending_readers.load() > _nb_pending_writers.load());
  DEBUG_MSG("Request a locking access for " << this);
  _mutex.lock();
  DEBUG_MSG("Got the locking access for " << this);
  // wait for readers to end their tasks
  while (_nb_readers.load() > 0);
  DEBUG_MSG("There is " << (_writer.load()  ? "one" : "no") << " writer process, "
            << _nb_pending_writers.load() << " pending writers"
            << " including " << this
            << ", " << _nb_pending_readers.load() << " pending readers"
            << " and " << _nb_readers.load() << " readers");
  // wait for writers to end their tasks
  while (_writer.exchange(true, memory_order_acquire));
  assert(_nb_readers.load() == 0); // there must not have any new reader...
  assert(_nb_pending_writers.load() > 0); // this thread is a pending writer...
  --_nb_pending_writers;
  DEBUG_MSG("There is " << (_writer.load()  ? "one" : "no") << " writer process, "
            << " (" << this << "), "
            << _nb_pending_writers.load() << " pending writers"
            << ", " << _nb_pending_readers.load() << " pending readers"
            << " and " << _nb_readers.load() << " readers");
  DEBUG_MSG("Release the locking access from " << this);
  _mutex.unlock(); // Read & write requests are unlocked
  DEBUG_MSG("Writer access granted for " << this);
}

void ReadWriteLock::releaseWriteAccess() {
  DEBUG_MSG("Releasing the write access for this (" << this << ") writer");
  assert(_writer.load() > 0); // This thread must have write grants.
  _writer.store(false, memory_order_release);
  DEBUG_MSG("Write access released for this (" << this << ") writer.");
  DEBUG_MSG("There is " << (_writer.load()  ? "one" : "no") << " writer process, "
            << " not including " << this << " anymore."
            << _nb_pending_writers.load() << " pending writers"
            << ", " << _nb_pending_readers.load() << " pending readers"
            << " and " << _nb_readers.load() << " readers");
  io_mutex.unlock();
}

END_BIJECTHASH_NAMESPACE
