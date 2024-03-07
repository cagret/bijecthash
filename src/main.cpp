#include "file_reader.hpp"
#include "transformer.hpp"
#include "identity_transformer.hpp"
#include "inthash_transformer.hpp"
#include "gab_transformer.hpp"
#include "permutation_transformer.hpp"
#include "program_options.hpp"
#include "settings.hpp"
#include "circular_queue.hpp"

#include <libgen.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string>
#include <chrono>
#include <sys/resource.h>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

mutex print_mutex;

// A shorthand for the index type
typedef vector<set<uint64_t>> index_t;

string fmt(string w, size_t i, size_t max) {
  string m = to_string(max);
  string s = to_string(i);
  if (s.length() < m.length()) {
    string p(m.length() - s.length(), '0');
    s = p + s;
  }
  return s + " " + w;
}

map<string, double> computeStatistics(const index_t &index, size_t nb_bins) {

  map<string, double> stats;

  vector<size_t> sizes;
  size_t n = index.size();
  sizes.reserve(n);
  if (nb_bins > index.size()) {
    nb_bins = index.size();
  }
  size_t m = nb_bins + 6;

  double mean = 0;
  double variance = 0;

  for (const auto &s : index) {
    sizes.push_back(s.size());
    mean += s.size();
    variance += s.size() * s.size();
  }

  sort(sizes.begin(), sizes.end());
  stats[fmt("min", 1, m)] = sizes.front();
  stats[fmt("med", 2, m)] = sizes[n / 2];
  stats[fmt("max", 3, m)] = sizes.back();

  mean /= n;

  stats[fmt("mean", 4, m)] = mean;

  variance /= n;
  variance -= mean * mean;
  stats[fmt("var", 5, m)] = variance;

  vector<size_t> bins(nb_bins, 0);
  size_t bin_size = n / bins.size() + (n % bins.size() > 0);
  stats[fmt("bin_size", 6, m)] = bin_size;

  for (size_t i = 0; i < n; ++i) {
    bins[i / bin_size] += sizes[i];
  }
  sizes.clear();

  n = bins.size();
  for (size_t i = 0; i < n; ++i) {
    stats[fmt("bin_" + to_string(i+1), i + 7, m)] = bins[i];
  }
  return stats;

}

struct KmerProducerConsumer {
  static size_t counter;
  static atomic_size_t still_alive;
  const size_t id;
  const Settings &settings;
  const string &filename;
  CircularQueue<string> &queue;
  const Transformer &transformer;
  index_t &index;
  mutex &index_mutex;
  thread prod;
  thread cons;

  KmerProducerConsumer(const Settings &settings,
                       const string &filename,
                       CircularQueue<string> &queue,
                       const Transformer &transformer,
                       index_t &index,
                       mutex &m):
    id(++counter),
    settings(settings), filename(filename), queue(queue),
    transformer(transformer), index(index), index_mutex(m)
  {
    ++still_alive;
  }

  void producer() {
#ifdef DEBUG
    print_mutex.lock();
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "Producer " << id << ":"
         << "filename = '" << filename << "'" << endl;
    print_mutex.unlock();
#endif
    FileReader reader(settings);
    reader.open(filename);

    if (!reader.isOpen()) {
      cerr << "Error: Unable to open fasta/fastq file '" << filename << "'" << endl;
      terminate();
    }

    while (reader.nextKmer()) {
      const string &kmer = reader.getCurrentKmer();
#ifdef DEBUG
      if (reader.getCurrentKmerID(false) == 1) {
        print_mutex.lock();
        cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
             << "[thread " << this_thread::get_id() << "]:"
             << "Producer " << id << ":"
             << "New sequence: '" << reader.getCurrentSequenceDescription() << "'" << endl;
        print_mutex.unlock();
      }
      print_mutex.lock();
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "[thread " << this_thread::get_id() << "]:"
           << "Producer " << id << ":"
           << "k-mer '" << reader.getCurrentKmer()
           << " (abs_ID: " << reader.getCurrentKmerID()
           << ",  rel_ID: " << reader.getCurrentKmerID(false)
           << ")" << endl;
      print_mutex.unlock();
#endif
      while (!queue.push(kmer)) {
#ifdef DEBUG
        print_mutex.lock();
        cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
             << "[thread " << this_thread::get_id() << "]:"
             << "Producer " << id << ":"
             << "Unable to push k-mer '" << kmer << "." << endl;
        cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
             << "[thread " << this_thread::get_id() << "]:"
             << "Producer " << id << ":"
             << "queue size: " << queue.size()
             << " (" << (queue.empty() ? "empty" : "not empty")
             << ", " << (queue.full() ? "full" : "not full") << ")." << endl;
        print_mutex.unlock();
#endif
        this_thread::yield();
        this_thread::sleep_for(1ns);
      }
#ifdef DEBUG
      print_mutex.lock();
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "[thread " << this_thread::get_id() << "]:"
           << "Producer " << id << ":"
           << "k-mer '" << kmer << " pushed successfully." << endl;
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "[thread " << this_thread::get_id() << "]:"
           << "Producer " << id << ":"
           << "queue size: " << queue.size()
           << " (" << (queue.empty() ? "empty" : "not empty")
           << ", " << (queue.full() ? "full" : "not full") << ")." << endl;
      print_mutex.unlock();
      cerr << queue;
#endif
    }
    --still_alive;
#ifdef DEBUG
    print_mutex.lock();
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "Producer " << id << ":"
         << "still_alive = " << still_alive << ":"
         << "file '" << filename << "' processed." << endl;
    print_mutex.unlock();
#endif
  }

  void consumer() {
    while ((still_alive.load() > 0) || !queue.empty()) {
      string kmer;
      bool ok = queue.pop(kmer);
      while (!queue.empty() && !ok) {
        ok = queue.pop(kmer);
#ifdef DEBUG
        print_mutex.lock();
        cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
             << "[thread " << this_thread::get_id() << "]:"
             << "Consumer " << id << ":"
             << "still_alive = " << still_alive.load() << ":"
             << "queue.size() = " << queue.size() << ":"
             << "Unable to pop any kmer." << endl;
        print_mutex.unlock();
#endif
        this_thread::yield();
        this_thread::sleep_for(1ns);
      }
      if (ok) {
#ifdef DEBUG
        print_mutex.lock();
        cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
             << "[thread " << this_thread::get_id() << "]:"
             << "Consumer " << id << ":"
             << "k-mer '" << kmer << "' successfully popped." << endl;
        print_mutex.unlock();
#endif
        Transformer::EncodedKmer encoded = transformer(kmer);
        string decoded = transformer(encoded);

#ifdef DEBUG
        print_mutex.lock();
        cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
             << "[thread " << this_thread::get_id() << "]:"
             << "Consumer " << id << ":"
             << "original kmer: '" << kmer << "'" << endl;
        cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
             << "[thread " << this_thread::get_id() << "]:"
             << "Consumer " << id << ":"
             << "decoded kmer:  '" << decoded << "'" << endl;
        print_mutex.unlock();
#endif
        assert(decoded == kmer);
        lock_guard<mutex> g(index_mutex);
        index[encoded.prefix].insert(encoded.suffix);
      }
    }
#ifdef DEBUG
    print_mutex.lock();
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "Consumer " << id << ":"
         << "still_alive = " << still_alive << ":"
         << "Producer " << id << " has finished." << endl;
    print_mutex.unlock();
    cerr << queue;
#endif
  }

  void run() {
    prod = thread(&KmerProducerConsumer::producer, this);
    cons = thread(&KmerProducerConsumer::consumer, this);
  }

  void join() {
    prod.join();
    cons.join();
  }

};

size_t KmerProducerConsumer::counter = 0;
atomic_size_t KmerProducerConsumer::still_alive = 0;


index_t makeIndexMultiThread(const Transformer &transformer, const vector<string> &filenames, const string &tag) {

  const Settings &s = transformer.settings;

  size_t k1 = s.prefix_length;
  size_t nb_subtrees = (1ul << (2 * k1));
  index_t kmer_index(nb_subtrees);
  mutex index_guardian;

  struct rusage rusage_start;
  getrusage(RUSAGE_SELF, &rusage_start);
  auto start = std::chrono::high_resolution_clock::now();

  CircularQueue<string> kmer_queue(10);
  vector<KmerProducerConsumer> workers;
  workers.reserve(filenames.size());

  for (auto &filename: filenames) {
    workers.emplace_back(s, filename, kmer_queue, transformer, kmer_index, index_guardian);
    workers.back().run();
  }

  for (auto &worker: workers) {
    worker.join();
  }
#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "CircularQueue:" << endl
       << kmer_queue << endl;
#endif

  auto end = std::chrono::high_resolution_clock::now();
  struct rusage rusage_end;
  getrusage(RUSAGE_SELF, &rusage_end);
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  auto memory = rusage_end.ru_maxrss - rusage_start.ru_maxrss;

  cout << "# XP\tLength\tPrefixLength\tMethod\tTime(ms)\tMemory(KB)\n"
       << tag
       << '\t' << s.length
       << '\t' << s.prefix_length
       << '\t' << transformer.description
       << '\t' << elapsed.count()
       << '\t' << memory
       << endl;
  return kmer_index;

}

index_t makeIndex(const Transformer &transformer, const vector<string> &filenames, const string &tag) {

  const Settings &s = transformer.settings;

  size_t k1 = s.prefix_length;
  size_t nb_subtrees = (1ul << (2 * k1));
  index_t kmer_index(nb_subtrees);
  FileReader reader(s);

  struct rusage rusage_start;
  getrusage(RUSAGE_SELF, &rusage_start);
  auto start = std::chrono::high_resolution_clock::now();

  for (auto &filename: filenames) {
    reader.open(filename);

    if (!reader.isOpen()) {
      cerr << "Error: Unable to open fasta/fastq file '" << filename << "'" << endl;
      terminate();
    }

    // Process each kmer
    while (reader.nextKmer()) {

      const string &kmer = reader.getCurrentKmer();

#ifdef DEBUG
      if (reader.getCurrentKmerID(false) == 1) {
        cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
             << "New sequence: '" << reader.getCurrentSequenceDescription() << "'" << endl;
      }
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "k-mer '" << reader.getCurrentKmer()
           << " (abs_ID: " << reader.getCurrentKmerID()
           << ",  rel_ID: " << reader.getCurrentKmerID(false)
           << ")" << endl;
#endif

      Transformer::EncodedKmer encoded = transformer(kmer);
      string decoded = transformer(encoded);

#ifdef DEBUG
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "original kmer: '" << kmer << "'" << endl;
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "decoded kmer:  '" << decoded << "'" << endl;
#endif
      assert(decoded == kmer);

      kmer_index[encoded.prefix].insert(encoded.suffix);

    }

  }

  auto end = std::chrono::high_resolution_clock::now();
  struct rusage rusage_end;
  getrusage(RUSAGE_SELF, &rusage_end);
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  auto memory = rusage_end.ru_maxrss - rusage_start.ru_maxrss;

  cout << "# XP\tLength\tPrefixLength\tMethod\tTime(ms)\tMemory(KB)\n"
       << tag
       << '\t' << s.length
       << '\t' << s.prefix_length
       << '\t' << transformer.description
       << '\t' << elapsed.count()
       << '\t' << memory
       << endl;
  return kmer_index;

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

  index_t index;

  if (settings.method == "identity") {
    index = makeIndex(IdentityTransformer(settings), filenames, settings.tag);
  } else if (settings.method == "inthash") {
    index = makeIndex(IntHashTransformer(settings), filenames, settings.tag);
  } else if (settings.method == "GAB") {
    index = makeIndex(GaBTransformer(settings, 17, 42), filenames, settings.tag);
  } else if (settings.method == "random") {
    index = makeIndexMultiThread(PermutationTransformer(settings), filenames, settings.tag);
  } else if (settings.method == "inverse") {
    vector<size_t> p(settings.length);
    for (size_t i = 0; i < settings.length; ++i) {
      p[i] = settings.length - i - 1;
    }
    index = makeIndex(PermutationTransformer(settings, p, "Inverse"), filenames, settings.tag);
  } else if (settings.method == "cyclic") {
    vector<size_t> p(settings.length);
    for (size_t i = 0; i < settings.length; ++i) {
      p[i] = (i + 1) % settings.length;
    }
    index = makeIndex(PermutationTransformer(settings, p, "Cyclic"), filenames, settings.tag);
  } else if (settings.method == "zigzag") {
    vector<size_t> p(settings.length);
    for (size_t i = 0; i < settings.length; ++i) {
      p[i] = ((i & 1) ? (settings.length - i - (settings.length & 1)) : i);
    }
    index = makeIndex(PermutationTransformer(settings, p, "ZigZag"), filenames, settings.tag);
  } else {
    cerr << "Error: Unknown method '" << settings.method << "'" << endl;
    return 1;
  }

  map<string, double> stats = computeStatistics(index, settings.nb_bins);
  char c = '#';
  for (auto &info: stats) {
    const string &kw = info.first;
    cout << c << kw.substr(kw.find(' ') + 1);
    c = '\t';
  }
  c = '\n';
  for (auto &info: stats) {
    cout << c << info.second;
    c = '\t';
  }
  cout << endl;

  return 0;
}
