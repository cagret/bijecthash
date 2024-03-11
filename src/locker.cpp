#include "locker.hpp"

#include <thread>
#include <iostream>
#include <cassert>

using namespace std;

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
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Request a read access for " << this
       << endl;
  io_mutex.unlock();
#endif
  ++_nb_pending_readers;
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "There is " << (_writer.load()  ? "one" : "no") << " writer process, "
       << _nb_pending_writers.load() << " pending writers"
       << ", " << _nb_pending_readers.load() << " pending readers"
       << " including " << this
       << " and " << _nb_readers.load() << " readers"
       << endl;
  io_mutex.unlock();
#endif
  // Give priority to pending writers (while they are the most abundant)
  while (_nb_pending_writers.load() > _nb_pending_readers.load());
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Request a locking access for " << this
       << endl;
  io_mutex.unlock();
#endif
  _mutex.lock(); // Read & write requests are locked
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Got the locking access for " << this
       << endl;
  io_mutex.unlock();
#endif
  while (_writer.load()); // wait until no more writer is working
  ++_nb_readers;
  assert(_nb_pending_readers.load() > 0); // this thread is a pending reader...
  --_nb_pending_readers;
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "There is " << (_writer.load()  ? "one" : "no") << " writer process, "
       << _nb_pending_writers.load() << " pending writers"
       << ", " << _nb_pending_readers.load() << " pending readers"
       << " and " << _nb_readers.load() << " readers"
       << " including " << this
       << endl;
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Release the locking access from " << this
       << endl;
  io_mutex.unlock();
#endif
  _mutex.unlock(); // Read & write requests are unlocked
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Read access granted for " << this
       << endl;
  io_mutex.unlock();
#endif
}

void ReadWriteLock::releaseReadAccess() {
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Releasing the read access for this (" << this << ") reader"
       << endl;
  io_mutex.unlock();
#endif
  assert(_nb_readers > 0); // This thread must have read grants.
  --_nb_readers;
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Read access released for this (" << this << ") reader."
       << endl;
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "There is " << (_writer.load()  ? "one" : "no") << " writer process, "
       << _nb_pending_writers.load() << " pending writers"
       << ", " << _nb_pending_readers.load() << " pending readers"
       << " and " << _nb_readers.load() << " readers"
       << " not including " << this << " anymore."
       << endl;
  io_mutex.unlock();
#endif
}

void ReadWriteLock::requestWriteAccess() {
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Request a write access for this (" << this << ")"
       << " writer" << endl;
  io_mutex.unlock();
#endif
  ++_nb_pending_writers;
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "There is " << (_writer.load()  ? "one" : "no") << " writer process, "
       << _nb_pending_writers.load() << " pending writers"
       << " including " << this
       << ", " << _nb_pending_readers.load() << " pending readers"
       << " and " << _nb_readers.load() << " readers"
       << endl;
  io_mutex.unlock();
#endif
  // Give priority to pending readers (while they are the most abundant)
  while (_nb_pending_readers.load() > _nb_pending_writers.load());
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Request a locking access for " << this
       << endl;
  io_mutex.unlock();
#endif
  _mutex.lock();
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Got the locking access for " << this
       << endl;
  io_mutex.unlock();
#endif
  // wait for readers to end their tasks
  while (_nb_readers.load() > 0);
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "There is " << (_writer.load()  ? "one" : "no") << " writer process, "
       << _nb_pending_writers.load() << " pending writers"
       << " including " << this
       << ", " << _nb_pending_readers.load() << " pending readers"
       << " and " << _nb_readers.load() << " readers"
       << endl;
  io_mutex.unlock();
#endif
  // wait for writers to end their tasks
  while (_writer.exchange(true, memory_order_acquire));
  assert(_nb_readers.load() == 0); // there must not have any new reader...
  assert(_nb_pending_writers.load() > 0); // this thread is a pending writer...
  --_nb_pending_writers;
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "There is " << (_writer.load()  ? "one" : "no") << " writer process, "
       << " (" << this << "), "
       << _nb_pending_writers.load() << " pending writers"
       << ", " << _nb_pending_readers.load() << " pending readers"
       << " and " << _nb_readers.load() << " readers"
       << endl;
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Release the locking access from " << this
       << endl;
  io_mutex.unlock();
#endif
  _mutex.unlock(); // Read & write requests are unlocked
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Writer access granted for " << this
       << endl;
  io_mutex.unlock();
#endif
}

void ReadWriteLock::releaseWriteAccess() {
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Releasing the write access for this (" << this << ") writer"
       << endl;
  io_mutex.unlock();
#endif
  assert(_writer.load() > 0); // This thread must have write grants.
  _writer.store(false, memory_order_release);
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "Write access released for this (" << this << ") writer."
       << endl;
  cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":"
       << "[Thread " << this_thread::get_id()  << "]:"
       << "There is " << (_writer.load()  ? "one" : "no") << " writer process, "
       << " not including " << this << " anymore."
       << _nb_pending_writers.load() << " pending writers"
       << ", " << _nb_pending_readers.load() << " pending readers"
       << " and " << _nb_readers.load() << " readers"
       << endl;
  io_mutex.unlock();
#endif
}
