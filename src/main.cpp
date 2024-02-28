#include "fileReader.hpp"
#include "transformer.hpp"
#include "identity_transformer.hpp"
#include "inthash_transformer.hpp"
#include "permutation_transformer.hpp"

#include <cassert>
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <chrono>
#include <sys/resource.h>
//#define DEBUG 

using namespace std;

double calculateVariance(const vector<set<uint64_t>> &index) {
  double mean = 0;
  double variance = 0;
  for (const auto &s : index) {
    mean += s.size();
    variance += s.size() * s.size();
  }
  mean /= index.size();
  variance /= index.size();
  variance -= mean * mean;
  return variance;
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
  int k = stoi(argv[2]); // Size of k-mers
  string method = argv[3];

  // Validate k
  if (k <= 0) {
    cerr << "Error: Invalid value for k. Please provide a positive integer." << endl;
    return 1;
  }

  size_t k1 = k / 3 + 2;

  vector<set<uint64_t>> index;

  if (method == "identity") {
    index = makeIndex(filename, IdentityTransformer(k, k1));
  } else if (method == "inthash") {
    index = makeIndex(filename, IntHashTransformer(k, k1));
  } else if (method == "GAB") {
    // TODO
  } else if (method == "random") {
    index = makeIndex(filename, PermutationTransformer(k, k1));
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
  
  double variance = calculateVariance(index);
  cout << "Variance des tailles des listes de suffixes: " << variance << endl;

  return 0;
}
