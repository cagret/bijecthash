#include "fileReader.hpp"
#include "transformer.hpp"
#include "identity_transformer.hpp"
#include "inthash_transformer.hpp"
#include "gab_transformer.hpp"
#include "permutation_transformer.hpp"
#include "program_options.hpp"
#include "settings.hpp"

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

//#define DEBUG

using namespace std;

string fmt(string w, size_t i, size_t max) {
  string m = to_string(max);
  string s = to_string(i);
  if (s.length() < m.length()) {
    string p(m.length() - s.length(), '0');
    s = p + s;
  }
  return s + " " + w;
}

map<string, double> computeStatistics(const vector<set<uint64_t>> &index, size_t nb_bins) {

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

vector<set<uint64_t>> makeIndex(const Transformer &transformer, const vector<string> &filenames, const string &tag) {

  const Settings &s = transformer.settings;

  size_t k1 = s.prefix_length;
  size_t nb_subtrees = (1ul << (2 * k1));
  vector<set<uint64_t>> kmerIndex(nb_subtrees);
  FileReader reader(s);

  struct rusage rusage_start;
  getrusage(RUSAGE_SELF, &rusage_start);
  auto start = std::chrono::high_resolution_clock::now();

  for (auto filename: filenames) {
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

      kmerIndex[encoded.prefix].insert(encoded.suffix);

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
  return kmerIndex;

}

int main(int argc, char* argv[]) {

  const ProgramOptions opts(argc, argv);
  const Settings settings = opts.settings();
  const vector<string> filenames = opts.filenames();

  cerr << "Settings:" << endl
       << settings << endl;
  cerr << "Files to process:" << endl;
  for (auto f: filenames) {
    cerr << "- '" << f << "'" << endl;
  }

  vector<set<uint64_t>> index;

  if (settings.method == "identity") {
    index = makeIndex(IdentityTransformer(settings), filenames, settings.tag);
  } else if (settings.method == "inthash") {
    index = makeIndex(IntHashTransformer(settings), filenames, settings.tag);
  } else if (settings.method == "GAB") {
    index = makeIndex(GaBTransformer(settings, 17, 42), filenames, settings.tag);
  } else if (settings.method == "random") {
    index = makeIndex(PermutationTransformer(settings), filenames, settings.tag);
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
  for (auto info: stats) {
    const string &kw = info.first;
    cout << c << kw.substr(kw.find(' ') + 1);
    c = '\t';
  }
  c = '\n';
  for (auto info: stats) {
    cout << c << info.second;
    c = '\t';
  }
  cout << endl;

  return 0;
}
