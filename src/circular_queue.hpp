#ifndef __CIRCULAR_QUEUE_HPP__
#define __CIRCULAR_QUEUE_HPP__

//#include <atomic>
#include <vector>
#include <mutex>

extern std::mutex print_mutex;

template <typename T>
class CircularQueue {

private:
  T *_data;
  std::vector<bool> _available;
  const size_t _capacity;
  // std::atomic_size_t _size;
  // std::atomic_size_t _first;
  // std::atomic_size_t _last;
  size_t _size;
  size_t _first;
  size_t _last;
  mutable std::mutex _mutex;

  CircularQueue &operator=(const CircularQueue &&) = delete;
  CircularQueue &operator=(const CircularQueue &) = delete;
  CircularQueue(const CircularQueue &) = delete;

  bool _threadUnsafePush(const T &t) {
    if (_threadUnsafeFull()) return false;
    _data[_last++] = t;
    _last %= _capacity;
    ++_size;
    return true;
  }

  bool _threadUnsafeEmplace(const T &&t) {
    if (_threadUnsafeFull()) return false;
    _data[_last++] = std::move(t);
    _last %= _capacity;
    ++_size;
    return true;
  }

  bool _threadUnsafePop(T &t) {
    if (_threadUnsafeEmpty()) return false;
    t = _data[_first++];
    _first %= _capacity;
    --_size;
    return true;
  }

  size_t _threadUnsafeSize() const {
    return _size;
  }

  bool _threadUnsafeEmpty() const {
    return (_threadUnsafeSize() == 0);
  }

  bool _threadUnsafeFull() const {
    return (_threadUnsafeSize() == _capacity);
  }

public:

  CircularQueue(size_t capacity):
    _data(new T[capacity]),
    _capacity(capacity),
    _size(0),
    _first(0),
    _last(0)
  {}

  ~CircularQueue() {
    if (_capacity) {
      delete [] _data;
    }
  }

  bool push(const T &t) {
    // if (full()) return false;
    // size_t p = _last.load(std::memory_order_acquire);
    // _data[p++] = t;
    // p %= _capacity;
    // ++_size;
    // _last.store(p, std::memory_order_release);
    std::lock_guard<std::mutex> g(_mutex);
    return _threadUnsafePush(t);
  }

  bool emplace(const T &&t) {
    // if (full()) return false;
    // size_t p = _last.load(std::memory_order_acquire);
    // _data[p++] = std::move(t);
    // p %= _capacity;
    // ++_size;
    // _last.store(p, std::memory_order_release);
    std::lock_guard<std::mutex> g(_mutex);
    return _threadUnsafeEmplace(t);
  }

  bool pop(T &t) {
    // if (empty()) return false;
    // size_t p = _first.load(std::memory_order_acquire);
    // t = std::move(_data[p++]);
    // p %= _capacity;
    // --_size;
    // _first.store(p, std::memory_order_release);
    std::lock_guard<std::mutex> g(_mutex);
    return _threadUnsafePop(t);
  }

  size_t size() const {
    // return _size.load();
    std::lock_guard<std::mutex> g(_mutex);
    return _threadUnsafeSize();
  }

  bool empty() const {
    // return (size() == 0);
    std::lock_guard<std::mutex> g(_mutex);
    return _threadUnsafeEmpty();
  }

  bool full() const {
    // return (size() == _capacity);
    std::lock_guard<std::mutex> g(_mutex);
    return _threadUnsafeFull();
  }

  void toStream(std::ostream &os) const {
    std::lock_guard<std::mutex> g(print_mutex);
    os << "CircularQueue:\n";
    for (size_t i = 0; i < _size; ++i) {
      os << "- '" << _data[(_first + i) % _capacity] << "'\n";
    }
    os << std::endl;
  }

};

template <typename T>
std::ostream &operator<<(std::ostream &os, const CircularQueue<T> &q) {
  q.toStream(os);
  return os;
}

#endif
