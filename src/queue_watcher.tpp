#include <kmer_collector.hpp>
#include <kmer_processor.hpp>
#include <locker.hpp>

#include <cassert>
#include <iostream>
#include <chrono>
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
