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
    uint64_t u = _encode(kmer.c_str(), 32);
    uint64_t w = _encode(kmer.c_str() + 32, settings.length - 32);
    uint64_t u_hash = hash_64(u, _kmer_mask);
    e.prefix = u_hash >> _prefix_shift;
    e.suffix = (u_hash & _suffix_mask) | (w << ((32 - settings.prefix_length) << 1));
    return e;
  }
}

string IntHashTransformer::operator()(const Transformer::EncodedKmer &e) const {
  if (settings.length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    v = hash_64i(v, _kmer_mask);
    return _decode(v, settings.length);
  } else {
    uint64_t u_hash = (e.prefix << _prefix_shift) | (e.suffix & _suffix_mask);
    uint64_t u = hash_64i(u_hash, _kmer_mask);
    uint64_t w = e.suffix >> ((32 - settings.prefix_length) << 1);
    return _decode(u, 32) + _decode(w, settings.length - 32);
  }
}
