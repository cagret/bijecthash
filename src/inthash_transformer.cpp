#include "inthash_transformer.hpp"
#include "inthash.h"

using namespace std;


IntHashTransformer::IntHashTransformer(const Settings &s):
  Transformer(s, "IntHash"),
  _kmer_mask((1ull << (settings.length << 1)) - 1),
  _prefix_shift((settings.length - settings.prefix_length) << 1),
  _suffix_mask((1ull << ((settings.length - settings.prefix_length) << 1)) - 1),
  _prefix_mask((1ull << (settings.prefix_length << 1)) - 1)
{}


Transformer::EncodedKmer IntHashTransformer::operator()(const std::string &kmer) const {
  EncodedKmer e;
  if (settings.length <= 32) {
    uint64_t v = _encode(kmer.c_str(), settings.length);
    v = hash_64(v, _kmer_mask);
    e.prefix = v >> _prefix_shift;
    e.suffix = v & _suffix_mask;
    return e;
  } else {
  uint64_t prefix = _encode(kmer.c_str(), settings.prefix_length);
  uint64_t suffix = _encode(kmer.c_str() + settings.prefix_length, settings.length - settings.prefix_length);
  uint64_t hash_value = hash_64((prefix << _prefix_shift) | suffix, _prefix_mask | _suffix_mask);

  e.prefix = prefix;
  e.suffix = suffix;
  e.hash_value = hash_value;
  return e;
  }
}

string IntHashTransformer::operator()(const Transformer::EncodedKmer &e) const {
  uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
  if (settings.length <= 32) {
    v = hash_64i(v, _kmer_mask);
  } else {
    v = hash_64i(v, _prefix_mask | _suffix_mask);
  return _decode(v, settings.length);
  }
}
