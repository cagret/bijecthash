#include "transformer.hpp"
#include "program_options.hpp"
#include "settings.hpp"
#include "circular_queue.hpp"
#include "kmer_index.hpp"
#include "kmer_collector.hpp"
#include "kmer_processor.hpp"

#ifdef WATCH_QUEUE
#  include "locker.hpp"
#endif

#include <libgen.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <chrono>
#include <sys/resource.h>
#include <thread>

using namespace std;

#ifdef WATCH_QUEUE
void threadsWatcher(const CircularQueue<string> &queue) {

  double mean = 0;
  double var = 0;
  double nb = 0;
  auto delay = 100ns;
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
      cerr << "\033[squeue size: [" << string(v / 2, '=') << string(50-v/2, ' ') << "]"
           << " (" << v << "%)\033[K\033[u";
      io_mutex.unlock();
      disp = 0;
    }
    ++nb;
#ifdef DEBUG
    io_mutex.lock();
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "Watcher:" << running_collectors << " / " << KmerCollector::counter() << " running collectors"
         << " and " << running_processors << " / " << KmerProcessor::counter() << " running processors"
         << ", queue size: " << s << endl;
    io_mutex.unlock();
#endif
    this_thread::yield();
    this_thread::sleep_for(delay);
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
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "Watcher:" << running_collectors << " / " << KmerCollector::counter() << " running collectors"
         << " and " << running_processors << " / " << KmerProcessor::counter() << " running processors"
         << ", queue size: " << s << endl;
    assert(KmerCollector::running() == 0);
    io_mutex.unlock();
#endif
    this_thread::yield();
    this_thread::sleep_for(delay);
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
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:"
       << "Watcher:" << KmerCollector::running() << " / " << KmerCollector::counter() << " running collectors"
       << " and " << KmerProcessor::running() << " / " << KmerProcessor::counter() << " running processors"
       << ", queue size: " << s << endl;
  io_mutex.unlock();
#endif
  assert(s == 0);
  assert(KmerCollector::running() == 0);
  assert(KmerCollector::running() == 0);
  io_mutex.lock();
  cerr << "[INFO] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:"
       << "Watcher:"
       << "nb of samples: " << nb << endl;
  cerr << "[INFO] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:"
       << "Watcher:"
       << "queue size average: " << mean << endl;
  cerr << "[INFO] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:"
       << "Watcher:"
       << "queue size variance: " << var << endl;
  assert(nb != 0);
  io_mutex.unlock();
}
#endif

struct infos {
  chrono::milliseconds time;
  long int memory;
};

infos makeIndexMultiThread(KmerIndex &kmer_index, const vector<string> &filenames) {
  const Settings &s = kmer_index.settings;

  struct rusage rusage_start;
  getrusage(RUSAGE_SELF, &rusage_start);
  auto start = chrono::high_resolution_clock::now();

  CircularQueue<string> kmer_queue(s.queue_size);
  vector<KmerCollector> collectors;
  collectors.reserve(filenames.size());
  vector<KmerProcessor> processors;
  size_t nb_threads = thread::hardware_concurrency();
  if (nb_threads > 3 * filenames.size()) {
    // Don't use more than 2 processor for 1 collector
    nb_threads = 2 * filenames.size();
  } else {
    // If there is less than 2 processor for 1 collector...
    if (nb_threads > 2 * filenames.size()) {
      // If there is more than 1 processor for 1 collector, use all of available threads.
      nb_threads -= filenames.size();
    } else {
      // Use at least 1 collector for 1 processor.
      nb_threads = filenames.size();
    }
  }
  // And always use one more thread.
  ++nb_threads;
  cerr << "Using " << nb_threads << " k-mer processor(s) [heuristic]"
       << " for " << filenames.size() << " k-mer collector(s) [one per file]." << endl
       << endl;
  processors.reserve(nb_threads);

#ifdef WATCH_QUEUE
  thread watcher(threadsWatcher, cref(kmer_queue));
#endif

  for (auto &filename: filenames) {
    collectors.emplace_back(s, filename, kmer_queue);
    collectors.back().run();
  }

  while (nb_threads--) {
    processors.emplace_back(s, kmer_index, kmer_queue);
    processors.back().run();
  }

  for (auto &worker: collectors) {
    worker.join();
  }

  for (auto &worker: processors) {
    worker.join();
  }
#ifdef WATCH_QUEUE
  watcher.join();
#endif

#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "CircularQueue:" << endl
       << kmer_queue << endl;
#endif

  auto end = chrono::high_resolution_clock::now();
  struct rusage rusage_end;
  getrusage(RUSAGE_SELF, &rusage_end);
  infos time_mem;
  time_mem.time = chrono::duration_cast<chrono::milliseconds>(end - start);
  time_mem.memory = rusage_end.ru_maxrss - rusage_start.ru_maxrss;

  return time_mem;

}

int main(int argc, char* argv[]) {

  const ProgramOptions opts(argc, argv);
  const Settings settings = opts.settings();
  const vector<string> filenames = opts.filenames();

  cerr << "Settings:" << endl
       << settings << endl;
  cerr << "Files to process:" << endl;
  for (auto &filename: filenames) {
    cerr << "- '" << filename << "'" << endl;
  }
  cerr << endl;

  KmerIndex index(settings);

  infos time_mem = makeIndexMultiThread(index, filenames);
  map<string, double> stats = index.statistics();

  cout << "# XP\tLength\tPrefix-length\tMethod\tTime(ms)\tMemory(KB)";
  for (auto &info: stats) {
    const string &kw = info.first;
    cout << '\t' << kw.substr(kw.find(' ') + 1);
  }
  cout << endl;

  cout << settings.tag
       << '\t' << settings.length
       << '\t' << settings.prefix_length
       << '\t' << settings.method
       << '\t' << time_mem.time.count()
       << '\t' << time_mem.memory;
  for (auto &info: stats) {
    cout << '\t' << info.second;
  }
  cout << endl;

  // cout << index << endl;

  return 0;
}
