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

#ifndef __CIRCULAR_QUEUE_HPP__
#define __CIRCULAR_QUEUE_HPP__

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iostream>

#include <locker.hpp>

namespace bijecthash {

  /**
   * A template basic implementation of a thread safe circular queue (of
   * some given fixed capacity).
   *
   * Notice that even if it uses atomic variables, the current
   * implementation is not lock free at all.
   */
  template <typename T>
  class CircularQueue {

  public:

    /**
     * The capacity of the queue.
     */
    const size_t capacity;

  private:

    /**
     * Multithread mutex locker
     */
    mutable SpinlockMutex _mutex;

    /**
     * Mask to compute modulo more efficiently
     */
    const size_t _mask;

    /**
     * The data to store in this queue.
     */
    T *_data;

    /**
     * The current number of elements in this queue.
     */
    std::atomic_size_t _size;

    /**
     * The index of the first queued element.
     */
    std::atomic_size_t _first;

    /**
     * The index of the after-the-last queued element.
     */
    std::atomic_size_t _last;

    /**
     * For thread safety, move assignment operator is removed.
     */
    CircularQueue &operator=(const CircularQueue &&) = delete;

    /**
     * For thread safety, copy assignment operator is removed.
     */
    CircularQueue &operator=(const CircularQueue &) = delete;

    /**
     * For thread safety, copy constructor is removed.
     */
    CircularQueue(const CircularQueue &) = delete;

    /**
     * Get the ceiling power of two of the given number
     *
     * \param n The value to ceil.
     *
     * \return Returns the ceiling power of two of the given value.
     */
    inline static size_t _nextPowerOfTwo(size_t n) {
      n--;
      n |= n >> 1;
      n |= n >> 2;
      n |= n >> 4;
      n |= n >> 8;
      n |= n >> 16;
      n |= n >> 32;
      n++;
      return n;
    }

  public:

    /**
     * Create a thread safe circular queue of the given capacity.
     *
     * \param capacity The amount of data that can be stored in the
     * queue. Notice that for performance consideration, the capacity is
     * rounded to the closest to capacity power of two value.
     */
    CircularQueue(size_t capacity):
      capacity(_nextPowerOfTwo(capacity)),
      _mask(this->capacity - 1),
      _data(new T[this->capacity]),
      _size(0),
      _first(0),
      _last(0)
    {
#ifdef DEBUG
      io_mutex.lock();
      std::cerr << "[DEBUG] "
                << "[Thread " << this_thread::get_id()  << "] "
                << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "
                << "Creating a circular queue having capacity " << capacity
                << std::endl;
      io_mutex.unlock();
#endif
    }

    /**
     * Destructor
     */
    ~CircularQueue() {
      if (capacity) {
        delete [] _data;
      }
    }

    /**
     * Try to enqueue a copy of the given element in this queue.
     *
     * \param t The element to enqueue.
     *
     * \return This return true on success and false otherwise (when the
     * queue is full).
     */
    bool push(const T &t) {
      LockerGuardian<> g(_mutex);
#ifdef DEBUG
      io_mutex.lock();
      std::cerr << "[DEBUG] "
                << "[Thread " << this_thread::get_id()  << "] "
                << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "
                << "Try to enqueue some element on queue of size " << _size.load()
                << " which is [" << _first.load() << ", " << _last.load() << "["
                << std::endl;
      io_mutex.unlock();
#endif
      if (full()) return false;
#ifdef DEBUG
      io_mutex.lock();
      std::cerr << "[DEBUG] "
                << "[Thread " << this_thread::get_id()  << "] "
                << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "
                << "Ok, let's go..."
                << std::endl;
      io_mutex.unlock();
#endif
      size_t p = _last.load();
#ifdef DEBUG
      size_t p_copy = p;
      io_mutex.lock();
      std::cerr << "[DEBUG] "
                << "[Thread " << this_thread::get_id()  << "] "
                << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "
                << "Adding element at position " << p_copy << '\n';
                << std::endl;
      io_mutex.unlock();
#endif
      _data[p++] = t;
      p = p & _mask;
      ++_size;
      _last.store(p);
#ifdef DEBUG
      io_mutex.lock();
      std::cerr << "[DEBUG] "
                << "[Thread " << this_thread::get_id()  << "] "
                << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "
                << "Element added at position " << p_copy
                << ", now size is " << _size.load()
                << " which is [" << _first.load() << ", " << _last.load() << "["
                << std::endl;
      io_mutex.unlock();
#endif
      return true;
    }

    /**
     * Try to enqueue the given element in this queue.
     *
     * \param t The element to enqueue.
     *
     * \return This return true on success and false otherwise (when the
     * queue is full).
     */
    bool emplace(const T &&t) {
      LockerGuardian<> g(_mutex);
      if (full()) return false;
      size_t p = _last.load();
      _data[p++] = std::move(t);
      p = p & _mask;
      ++_size;
      _last.store(p);
      return true;
    }

    /**
     * Try to dequeue the oldest element in this queue.
     *
     * \param t A variable where to store the dequeued element.
     *
     * \return This return true on success and false otherwise (when the
     * queue is empty).
     */
    bool pop(T &t) {
      LockerGuardian<> g(_mutex);
#ifdef DEBUG
      io_mutex.lock();
      std::cerr << "[DEBUG] "
                << "[Thread " << this_thread::get_id()  << "] "
                << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "
                << "Try to dequeue some element on queue of size " << _size.load()
                << " which is [" << _first.load() << ", " << _last.load() << "["
                << std::endl;
      io_mutex.unlock();
#endif
      if (empty()) return false;
#ifdef DEBUG
      io_mutex.lock();
      std::cerr << "[DEBUG] "
                << "[Thread " << this_thread::get_id()  << "] "
                << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "
                << "Ok, let's go..."
                << std::endl;
      io_mutex.unlock();
#endif
      size_t p = _first.load();
#ifdef DEBUG
      size_t p_copy = p;
      io_mutex.lock();
      std::cerr << "[DEBUG] "
                << "[Thread " << this_thread::get_id()  << "] "
                << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "
                << "Removing element at position " << p_copy
                << std::endl;
      io_mutex.unlock();
#endif
      t = std::move(_data[p++]);
      p = p & _mask;
      --_size;
      _first.store(p);
#ifdef DEBUG
      io_mutex.lock();
      std::cerr << "[DEBUG] "
                << "[Thread " << this_thread::get_id()  << "] "
                << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "
                << "Element removed from position " << p_copy
                << ", now size is " << _size.load()
                << " which is [" << _first.load() << ", " << _last.load() << "["
                << std::endl;
      io_mutex.unlock();
#endif
      return true;
    }

    /**
     * Get the size of this queue.
     *
     * \return Returns the number of enqueued elements.
     */
    size_t size() const {
      return _size.load();
    }

    /**
     * Check if there is some elements in this queue.
     *
     * \return Returns true if there is no element in this queue and
     * false otherwise.
     */
    bool empty() const {
      return (size() == 0);
    }

    /**
     * Check if there is some available room in this queue.
     *
     * \return Returns true if there is no room to add some element in
     * this queue and false otherwise.
     */
    bool full() const {
      return (size() == capacity);
    }

    /**
     * Print this queue to the given stream.
     *
     * \param os The output stream on which to print this queue.
     */
    void toStream(std::ostream &os) const {
      LockerGuardian<> g1(_mutex);
      LockerGuardian<> g2(io_mutex);
      os << "CircularQueue:\n";
      size_t n = _size.load();
      for (size_t i = 0; i < n; ++i) {
        os << "- '" << _data[(_first + i) & _mask] << "'\n";
      }
    }

  };

  /**
   * Overloads the stream insertion operator for circular queues.
   *
   * \param os The output stream on which to insert the queue.
   *
   * \param q The que to insert into the stream.
   *
   * \return Returns the modified output stream.
   */
  template <typename T>
  std::ostream &operator<<(std::ostream &os, const CircularQueue<T> &q) {
    q.toStream(os);
    return os;
  }

}

#endif
