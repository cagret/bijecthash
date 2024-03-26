#include "transformer.hpp"

#include "locker.hpp"

#include <cassert>
#include <iostream>
#include <mutex>

using namespace std;

uint64_t Transformer::_encode(const char *dna_str, size_t n) {
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "n = " << n << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "dna_str = '" << string(dna_str, n) << "'" << endl;
  io_mutex.unlock();
#endif
  assert(n <= (4 * sizeof(uint64_t)));
  uint64_t encoded = 0;
  for (size_t i = 0; i < n; ++i) {
    int val = 0;
    switch (dna_str[i]) {
    case 'A': val = 0; break;
    case 'C': val = 1; break;
    case 'G': val = 2; break;
    case 'T': val = 3; break;
    default:
      io_mutex.lock();
      cerr << "Error: unable to encode the given k-mer (" << string(dna_str, n) << ")"
           << " since it contains the symbol '" << dna_str[i] << "'"
           << " which is neither A nor C nor G nor T." << endl;
      io_mutex.unlock();
      exit(1);
    }
    encoded = (encoded << 2) | val;
  }
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "encoded = " << encoded << endl;
  io_mutex.unlock();
#endif
  return encoded;
}

string Transformer::_decode(uint64_t v, size_t n) {
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "n = " << n << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "v = " << v << endl;
  io_mutex.unlock();
#endif
  string decoded(n, '?');
  while (n--) {
    decoded[n] = "ACGT"[v & 3];
    v >>= 2;
  }
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "decoded = '" << decoded << "'" << endl;
  io_mutex.unlock();
#endif
  return decoded;
}

Transformer::Transformer(const Settings &s, const string &description):
  settings(s),
  description(description)
{
  assert(settings.length > 0);
  assert(settings.prefix_length > 0);
  assert(settings.length > settings.prefix_length);
  assert(s.prefix_length <= (4 * sizeof(uint64_t)));
  assert((s.length - s.prefix_length) <= (4 * sizeof(uint64_t)));
}

string Transformer::getTransformedKmer(const Transformer::EncodedKmer &e) const {
  string kmer = _decode(e.prefix, settings.prefix_length);
  kmer += _decode(e.suffix, settings.length - settings.prefix_length);
  return kmer;
}
