#include "fileReader.hpp"
#include "transformer.hpp"
#include "identity_transformer.hpp"
#include "inthash_transformer.hpp"
#include "gab_transformer.hpp"
#include "permutation_transformer.hpp"

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
#if (NB_BINS+0) < 1
#  undef NB_BINS
#endif
#ifndef NB_BINS
#  define NB_BINS 10
#endif

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

vector<set<uint64_t>> makeIndex(const string &filename, const Transformer &transformer) {

  size_t k = transformer.getKmerLength();
  size_t k1 = transformer.getKmerPrefixLength();

  FileReader reader(k, filename);

  if (!reader.isOpen()) {
    cerr << "Error: Unable to open fasta/fastq file '" << filename << "'" << endl;
    terminate();
  }

  size_t nb_subtrees = (1ul << (2 * k1));
  vector<set<uint64_t>> kmerIndex(nb_subtrees);

  struct rusage rusage_start;
  getrusage(RUSAGE_SELF, &rusage_start);
  auto start = std::chrono::high_resolution_clock::now();

  // Process each kmer
  while (reader.nextKmer()) {

    const string &kmer = reader.getCurrentKmer();

#ifdef DEBUG
    if (reader.getCurrentKmerID(false) == 1) {
      cout << "New sequence: '" << reader.getCurrentSequenceDescription() << "'" << endl;
    }
    cout << "k-mer '" << reader.getCurrentKmer()
         << " (abs_ID: " << reader.getCurrentKmerID()
         << ",  rel_ID: " << reader.getCurrentKmerID(false)
         << ")" << endl;
#endif

    Transformer::EncodedKmer encoded = transformer(kmer);
    string decoded = transformer(encoded);

#ifdef DEBUG
    cout << "original kmer: '" << kmer << "'" << endl;
    cout << "decoded kmer:  '" << decoded << "'" << endl;
#endif
    assert(decoded == kmer);

    kmerIndex[encoded.prefix].insert(encoded.suffix);

  }

  auto end = std::chrono::high_resolution_clock::now();
  struct rusage rusage_end;
  getrusage(RUSAGE_SELF, &rusage_end);
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  auto memory = rusage_end.ru_maxrss - rusage_start.ru_maxrss;

  cout << "# Method\tFile\ttime(ms)\tmemory(KB)\n"
       << transformer.description()
       << "\t" << filename
       << "\t" << elapsed.count()
       << "\t"<< memory
       << endl;

  return kmerIndex;

}


int main(int argc, char* argv[]) {
  // Check if the correct number of arguments are provided
  if (argc != 4) {
    cerr << "Error: Invalid arguments. Usage: " << argv[0] << " <filename> <k> <method>" << endl
         << "where method is one of:" << endl
         << "  - identity" << endl
         << "  - random" << endl
         << "  - cyclic" << endl
         << "  - inverse" << endl
         << "  - zigzag" << endl
         << "  - inthash" << endl
         << "  - GAB" << endl
         << endl;
    return 1;
  }

  // Parse command-line arguments
  string filename = argv[1];
  size_t k = strtoul(argv[2], NULL, 10); // Size of k-mers
  string method = argv[3];

  // Validate k
  if (k <= 0) {
    cerr << "Error: Invalid value for k. Please provide a positive integer." << endl;
    return 1;
  }

  size_t k1 = k / 3 + 2;
  // Don't allow a prefix length greater than 13 (since 4^13 > 67M subtrees, which is already enormeous)
  if (k1 > 13) k1 = 13;

  vector<set<uint64_t>> index;

  if (method == "identity") {
    index = makeIndex(filename, IdentityTransformer(k, k1));
  } else if (method == "inthash") {
    index = makeIndex(filename, IntHashTransformer(k, k1));
  } else if (method == "GAB") {
    index = makeIndex(filename, GaBTransformer(k, k1, 17, 42, k*2));
  } else if (method == "random") {
    index = makeIndex(filename, PermutationTransformer(k, k1));
  } else if (method == "inverse") {
    vector<size_t> p(k);
    for (size_t i = 0; i < k; ++i) {
      p[i] = k - i - 1;
    }
    index = makeIndex(filename, PermutationTransformer(k, k1, p, "Inverse"));
  } else if (method == "cyclic") {
    vector<size_t> p(k);
    for (size_t i = 0; i < k; ++i) {
      p[i] = (i + 1) % k;
    }
    index = makeIndex(filename, PermutationTransformer(k, k1, p, "Cyclic"));
  } else if (method == "zigzag") {
    vector<size_t> p(k);
    for (size_t i = 0; i < k; ++i) {
      p[i] = ((i & 1) ? (k - i - (k & 1)) : i);
    }
    index = makeIndex(filename, PermutationTransformer(k, k1, p, "ZigZag"));
  } else {
    cerr << "Error: Unknown method '" << method << "'" << endl;
    return 1;
  }

  map<string, double> stats = computeStatistics(index, NB_BINS);
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
