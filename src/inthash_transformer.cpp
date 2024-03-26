#include "inthash_transformer.hpp"
#include "inthash.h"

#include <cassert>

#ifdef DEBUG
#include <locker.hpp>
#endif

using namespace std;

IntHashTransformer::IntHashTransformer(const Settings &s):
  Transformer(s, "IntHash"),
  _kmer_mask((1ull << settings.length << settings.length) - 1ull),
  _prefix_shift((((settings.length > 32) ? 32 : settings.length) - settings.prefix_length) << 1),
  _suffix_mask((settings.length > 32) ? ((1ull << ((settings.length - 32) << 1)) - 1) : ((1ull << ((settings.length - settings.prefix_length) << 1)) - 1))
{}


Transformer::EncodedKmer IntHashTransformer::operator()(const std::string &kmer) const {
  EncodedKmer e;
  if (settings.length <= 32) {
    uint64_t v = _encode(kmer.c_str(), settings.length);
#ifdef DEBUG
    uint64_t orig = v;
#endif
    v = hash_64(v, _kmer_mask);
#ifdef DEBUG
    uint64_t rev_v = hash_64i(v, _kmer_mask);
    io_mutex.lock();
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "orig = " << orig << endl;
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "v = " << v << endl;
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "rev_v = " << rev_v << endl;
    assert(orig == rev_v);
    io_mutex.unlock();
#endif
    e.prefix = v >> _prefix_shift;
    e.suffix = v & _suffix_mask;
  } else {
    uint64_t prefix = _encode(kmer.c_str(), 32);
    uint64_t suffix = _encode(kmer.c_str() + 32, settings.length - 32);
    uint64_t prefix_transformed = hash_64(prefix, _kmer_mask);
    e.prefix = prefix_transformed >> _prefix_shift;
    e.suffix = prefix_transformed << (settings.prefix_length << 1) | suffix;
  }
  return e;
}

string IntHashTransformer::operator()(const Transformer::EncodedKmer &e) const {
  if (settings.length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    v = hash_64i(v, _kmer_mask);
    return _decode(v, settings.length);
  } else {
    uint64_t u = (e.prefix << _prefix_shift) | (e.suffix >> (settings.prefix_length << 1));
    u = hash_64i(u, _kmer_mask);
    uint64_t v = e.suffix & _suffix_mask;
    return _decode(u, 32) + _decode(v, settings.length - 32);
  }
}

std::string IntHashTransformer::getTransformedKmer(const Transformer::EncodedKmer &e) const {
  if (settings.length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    return _decode(v, settings.length);
  } else {
    uint64_t u = (e.prefix << _prefix_shift) | (e.suffix >> (settings.prefix_length << 1));
    uint64_t v = e.suffix & _suffix_mask;
    return _decode(u, 32) + _decode(v, settings.length - 32);
  }
}
