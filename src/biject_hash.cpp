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

#include "bh_kmer_collector.hpp"
#include "bh_kmer_index.hpp"
#include "bh_kmer_processor.hpp"
#ifdef ENABLE_CACHE_STATISTICS
#  include "cache_statistics.hpp"
#endif
#include "common.hpp"
#include "lcp_stats.hpp"
#include "program_options.hpp"
#include "queue_watcher.hpp"
#include "settings.hpp"
#include "threaded_reader_writer.hpp"
#include "transformer.hpp"

#include <libgen.h>
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
  LcpStats lcp_stats;
};

typedef ThreadedReaderWriter<BhKmerProcessor, BhKmerCollector, string> BijectHashBaseClass;
class BijectHash: public BijectHashBaseClass {

private:

  BhKmerIndex &_index;
  infos _time_mem_stats;
#ifdef WATCH_QUEUE
  thread _watcher;
#endif

  virtual void _pre() override {

#ifdef WATCH_QUEUE
    _watcher = std::thread(queueWatcher<KmerProcessor, KmerCollector, string>, std::cref(_queue));
#endif
    struct rusage rusage_start;
    getrusage(RUSAGE_SELF, &rusage_start);
    _time_mem_stats.time = rusage_start.ru_utime.tv_usec;
    _time_mem_stats.memory = rusage_start.ru_maxrss;
#ifdef ENABLE_CACHE_STATISTICS
    _time_mem_stats.cache_stats.start();
#endif

    DEBUG_MSG("Ready to run");

  }

  virtual void _post() override {

#ifdef WATCH_QUEUE
      _watcher.join();
#endif

      assert(_queue.empty());

#ifdef ENABLE_CACHE_STATISTICS
    _time_mem_stats.cache_stats.stop();
#endif

    struct rusage rusage_end;
    getrusage(RUSAGE_SELF, &rusage_end);

    _time_mem_stats.time = rusage_end.ru_utime.tv_usec - _time_mem_stats.time;
    _time_mem_stats.memory = rusage_end.ru_maxrss - _time_mem_stats.memory;

#ifdef ENABLE_CACHE_STATISTICS
    _time_mem_stats.cache_stats.update();
#endif

    DEBUG_MSG("Statistics computation done");

  }

public:

  BijectHash(BhKmerIndex &index, const vector<string> &filenames):
    BijectHashBaseClass(index.settings.queue_size, 0, 0),
    _index(index)
  {

    const Settings &s = index.settings;
    _writers.reserve(filenames.size());
    for (auto &filename: filenames) {
      _writers.emplace_back(s, filename, _queue);
    }

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
         << " for " << filenames.size() << " k-mer collector(s) [one per file]." << '\n'
         << endl;
    while (nb_threads--) {
      _readers.emplace_back(_index, _queue);
    }

  }

  const infos &getTimeMemStats() {
    return _time_mem_stats;
  }

};


int main(int argc, char* argv[]) {

  DEBUG_MSG("BEFORE");
  const ProgramOptions opts(argc, argv);
  DEBUG_MSG("MIDDLE");
  const Settings settings = opts.settings();
  DEBUG_MSG("AFTER");
  const vector<string> filenames = opts.filenames();

  cerr << "Settings:" << '\n'
       << settings << endl;
  cerr << "Files to process:" << '\n';
  for (auto &filename: filenames) {
    cerr << "- '" << filename << "'" << '\n';
  }
  cerr << endl;

  BhKmerIndex index(settings);
  BijectHash bh(index, filenames);
  bh.run();
  const infos &time_mem_stats = bh.getTimeMemStats();
  map<string, double> stats = index.statistics();

  cout << "#XP\tLength\tPrefixLength\tMethod\tTime(ms)\tMemory(KB)\tNbFiles\tLCP_nb_values\tLCP_avg\tLCP_var";
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
       << '\t' << settings.kmer_length
       << '\t' << settings.prefix_length
       << '\t' << index.transformer().description
       //<< '\t' << time_mem_stats.time.count()
       << '\t' << time_mem_stats.time
       << '\t' << time_mem_stats.memory
       << '\t' << filenames.size()
       << '\t' << time_mem_stats.lcp_stats.nb_kmers
       << '\t' << time_mem_stats.lcp_stats.average
       << '\t' << time_mem_stats.lcp_stats.variance;
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
