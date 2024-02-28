#include "inthash_transformer.hpp"
#include "inthash.h"

using namespace std;

IntHashTransformer::IntHashTransformer(size_t k, size_t prefix_length):
  Transformer(k, prefix_length, "IntHash"),
  _kmer_mask((1ul << (k << 1)) - 1),
  _prefix_shift(_suffix_length << 1),
  _suffix_mask((1ul << (_suffix_length << 1)) - 1)
{}

Transformer::EncodedKmer IntHashTransformer::operator()(const std::string &kmer) const {
  uint64_t v = _encode(kmer.c_str(), getKmerLength());
  v = hash_64(v, _kmer_mask);
  EncodedKmer e;
  e.prefix = v >> _prefix_shift;
  e.suffix = v & _suffix_mask;
  return e;
}

string IntHashTransformer::operator()(const Transformer::EncodedKmer &e) const {
  uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
  v = hash_64i(v, _kmer_mask);
  return _decode(v, getKmerLength());
}
