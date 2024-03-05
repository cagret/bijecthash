#include "inthash_transformer.hpp"
#include "inthash.h"

using namespace std;

IntHashTransformer::IntHashTransformer(const Settings &s):
  Transformer(s, "IntHash"),
  _kmer_mask((1ul << (settings.length << 1)) - 1),
  _prefix_shift((settings.length - settings.prefix_length) << 1),
  _suffix_mask((1ul << ((settings.length - settings.prefix_length) << 1)) - 1)
{}

Transformer::EncodedKmer IntHashTransformer::operator()(const std::string &kmer) const {
  uint64_t v = _encode(kmer.c_str(), settings.length);
  v = hash_64(v, _kmer_mask);
  EncodedKmer e;
  e.prefix = v >> _prefix_shift;
  e.suffix = v & _suffix_mask;
  return e;
}

string IntHashTransformer::operator()(const Transformer::EncodedKmer &e) const {
  uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
  v = hash_64i(v, _kmer_mask);
  return _decode(v, settings.length);
}
