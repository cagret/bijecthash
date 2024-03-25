#include "canonical_transformer.hpp"

#include <cassert>

using namespace std;

static char complement(char nucl) {
  if (nucl > 'Z') {
    nucl += 'A' - 'a';
  }
  switch (nucl) {
  case 'A': return 'T';
  case 'C': return 'G';
  case 'G': return 'C';
  case 'T':
  case 'U': return 'A';
  default:
    cerr << "Unable to compute the reverse complement of nucleotide '" << nucl << "'" << endl;
    exit(1);
  }
  return 0;
}

CanonicalTransformer::CanonicalTransformer(const Settings &s):
  Transformer(s, "Canonical") {
  // We need one bit (to store information about the conserved k-mer
  // (between the k-mer and is reverse).
  assert((settings.length - settings.prefix_length) < ((sizeof(uint64_t) << 3) / 2));
}

Transformer::EncodedKmer CanonicalTransformer::operator()(const std::string &kmer) const {
  assert(kmer.length() == settings.length);
  string lowest_kmer(settings.length, '\0');
  size_t i = 0;
  int best = 0;
  while ((best >= 0) && (i < settings.length)) {
    lowest_kmer[i] = complement(kmer[settings.length - i - 1]);
    if (best == 0) {
      if (kmer[i] < lowest_kmer[i]) {
        best = -1;
      } else {
        if (kmer[i] > lowest_kmer[i]) {
          best = 1;
        }
      }
    }
    ++i;
  }
  uint64_t m = 0;
  if (best < 0) {
    lowest_kmer = kmer;
  } else {
    m = 1ull << 62;
  }
  EncodedKmer e;
  e.prefix = _encode(lowest_kmer.c_str(), settings.prefix_length);
  e.suffix = _encode(lowest_kmer.c_str() + settings.prefix_length, settings.length - settings.prefix_length);
  e.suffix |= m;
  return e;
}

string CanonicalTransformer::operator()(const Transformer::EncodedKmer &e) const {
  string kmer = _decode(e.prefix, settings.prefix_length);
  kmer += _decode(e.suffix, settings.length - settings.prefix_length);
  if (e.suffix >> 62) {
    size_t p = (settings.length >> 1) + (settings.length & 1);
    for (size_t i = 0; i < p; ++i) {
      char c = complement(kmer[i]);
      kmer[i] = complement(kmer[settings.length - i - 1]);
      kmer[settings.length - i - 1] = c;
    }
  }
  return kmer;
}
