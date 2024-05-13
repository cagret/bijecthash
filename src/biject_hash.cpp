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

#ifdef ENABLE_CACHE_STATISTICS
#  include "cache_statistics.hpp"
#endif
#include "circular_queue.hpp"
#include "common.hpp"
#include "kmer_collector.hpp"
#include "kmer_index.hpp"
#include "kmer_processor.hpp"
#include "program_options.hpp"
#ifdef WATCH_QUEUE
#  include "queue_watcher.hpp"
#endif
#include "settings.hpp"
#include "transformer.hpp"

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
using namespace bijecthash;

struct infos {
  //chrono::milliseconds time;
  long int time;
  long int memory;
#ifdef ENABLE_CACHE_STATISTICS
  CacheStatistics cache_stats;
#endif
  vector<KmerCollector::LcpStats> lcp_stats;
};

infos makeIndexMultiThread(KmerIndex &kmer_index, const vector<string> &filenames) {
  const Settings &s = kmer_index.settings;

  infos time_mem_stats;


  struct rusage rusage_start, rusage_end;
  getrusage(RUSAGE_SELF, &rusage_start);

#ifdef ENABLE_CACHE_STATISTICS
  time_mem_stats.cache_stats.start();
#endif

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

  DEBUG_MSG("CircularQueue:" << endl << kmer_queue);

#ifdef ENABLE_CACHE_STATISTICS
  time_mem_stats.cache_stats.stop();
#endif

  getrusage(RUSAGE_SELF, &rusage_end);
  time_mem_stats.time = rusage_end.ru_utime.tv_usec - rusage_start.ru_utime.tv_usec;
  time_mem_stats.memory = rusage_end.ru_maxrss - rusage_start.ru_maxrss;

#ifdef ENABLE_CACHE_STATISTICS
  time_mem_stats.cache_stats.update();
#endif

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
#ifdef ENABLE_CACHE_STATISTICS
  for (const auto &info: time_mem_stats.cache_stats) {
    cout << '\t' << info.first;
  }
#endif
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
#ifdef ENABLE_CACHE_STATISTICS
  for (auto &info: time_mem_stats.cache_stats) {
    cout << '\t' << info.second;
  }
#endif
  for (auto &info: stats) {
    cout << '\t' << info.second;
  }
  cout << endl;

  // cout << index << endl;

  cerr << "That's All, Folks!!!" << endl;

  return 0;
}
