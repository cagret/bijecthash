#ifndef __KMER_COLLECTOR_HPP__
#define __KMER_COLLECTOR_HPP__

#include <atomic>
#include <thread>
#include <string>
#include <settings.hpp>
#include <circular_queue.hpp>

/**
 * A k-mer collector helper that stores k-mers in a circular queue.
 *
 * This helper class allows to run the k-mer collector in a dedicated
 * thread.
 */
class KmerCollector {

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
   * Average of the longuest common prefix between two consecutive transformed k-mers.
   */
  double _average_lcp;

  /**
   * Variance of the longuest common prefix between two consecutive
   * transformed k-mers.
   */
  double _variance_lcp;

  /**
   * The circular queue feed by this k-mer collector.
   */
  CircularQueue<std::string> &_queue;

  /**
   * The thread running this collector.
   */
  std::thread _thread;

  /**
   * Read the k-mers from the associated file and store them into the
   * queue.
   *
   * This method will exit only when the file will be entirely
   * parsed. When the queue is full, it waits until some other thread
   * consumes some k-mers to release some space.
   */
  void _run();

public:

  /**
   * This k-mer collector id
   */
  const size_t id;

  /**
   * The settings used by this k-mer collector.
   */
  const Settings &settings;

  /**
   * The filename associated to this k-mer collector.
   */
  const std::string &filename;

  /**
   * Builds a k-mer collector.
   *
   * \param settings The settings to use for the file collector.
   *
   * \param filename The name of the file to read.
   *
   * \param queue The queue to feed with k-mers.
   */
  KmerCollector(const Settings &settings,
                const std::string &filename,
                CircularQueue<std::string> &queue);

  /**
   * Start the dedicated thread.
   *
   * \see The k-mer queue feeding is performed by the private _run()
   * method.
   */
  inline void run() {
    _thread = std::thread(&KmerCollector::_run, this);
  }

  /**
   * This join the thread when it ends.
   */
  inline void join() {
    _thread.join();
  }

  /**
   * Get the number of create instances.
   *
   * \return Return the number of created instances.
   */
  static inline size_t counter() {
    return _counter;
  }

  /**
   * Get the total number of running instances.
   *
   * \return Return the total number of running instances. An instance
   * is considered as running from its creation and until it reaches
   * the end of the _run() method.
   */
  static inline size_t running() {
    return _running;
  }

};

#endif
