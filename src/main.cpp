#include "transformer.hpp"
#include "program_options.hpp"
#include "settings.hpp"
#include "circular_queue.hpp"
#include "kmer_index.hpp"
#include "kmer_collector.hpp"
#include "kmer_processor.hpp"

#ifdef WATCH_QUEUE
#  include "queue_watcher.hpp"
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

struct infos {
  //chrono::milliseconds time;
  long int time;
  long int memory;
  vector<KmerCollector::LcpStats> lcp_stats;
};

infos makeIndexMultiThread(KmerIndex &kmer_index, const vector<string> &filenames) {
  const Settings &s = kmer_index.settings;

  infos time_mem_stats;
  struct rusage rusage_start, rusage_end;
  getrusage(RUSAGE_SELF, &rusage_start);

  CircularQueue<string> kmer_queue(s.queue_size);
  vector<KmerCollector> collectors;
  collectors.reserve(filenames.size());
  time_mem_stats.lcp_stats.reserve(filenames.size());
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
  thread watcher(queueWatcher<string>, cref(kmer_queue));
#endif

  for (auto &filename: filenames) {
    time_mem_stats.lcp_stats.emplace_back();
    collectors.emplace_back(s, filename, kmer_queue, time_mem_stats.lcp_stats.back());
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

  getrusage(RUSAGE_SELF, &rusage_end);
  time_mem_stats.time = rusage_end.ru_utime.tv_usec - rusage_start.ru_utime.tv_usec;
  time_mem_stats.memory = rusage_end.ru_maxrss - rusage_start.ru_maxrss;

  return time_mem_stats;

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
  infos time_mem_stats = makeIndexMultiThread(index, filenames);
  map<string, double> stats = index.statistics();

  cout << "#XP\tLength\tPrefixLength\tMethod\tTime(ms)\tMemory(KB)\tNbFiles";
  for (size_t i = 0; i < time_mem_stats.lcp_stats.size(); ++i) {
    cout << '\t' << "File_" << (i + 1) << "_LCP_nb_values"
         << '\t' << "File_" << (i + 1) << "_LCP_avg"
         << '\t' << "File_" << (i + 1) << "_LCP_var";
  }
  for (auto &info: stats) {
    const string &kw = info.first;
    cout << '\t' << kw.substr(kw.find(' ') + 1);
  }
  cout << endl;

  cout << settings.tag
       << '\t' << settings.length
       << '\t' << settings.prefix_length
       << '\t' << index.transformer().description
       //<< '\t' << time_mem_stats.time.count()
       << '\t' << time_mem_stats.time
       << '\t' << time_mem_stats.memory
       << '\t' << filenames.size();
  for (auto &lcp_stat: time_mem_stats.lcp_stats) {
    cout << '\t' << lcp_stat.nb_kmers
         << '\t' << lcp_stat.average
         << '\t' << lcp_stat.variance;
  }
  for (auto &info: stats) {
    cout << '\t' << info.second;
  }
  cout << endl;

  // cout << index << endl;

  cerr << "That's All, Folks!!!" << endl;

  return 0;
}
