#include "gab_transformer.hpp"

#include <cassert>
#include <random>

#ifdef DEBUG
#include <locker.hpp>
#endif

using namespace std;

// trick to assign new values for a and b using computation of both old values
static void _assign(uint64_t v1, uint64_t v2, uint64_t &new_a, uint64_t &new_b) {
  new_a = v1;
  new_b = v2;
}

static uint64_t _multiplicativeInverse(uint64_t a, size_t sigma) {
  // We want to find, given and odd value a, the rev_a such that:
  //
  // a * rev_a = 1 mod 2^sigma
  //
  // The extended euclidian algorithm solves the following equation
  // given a and b (Bézout's Lemma ensure the existance of some
  // integer x and y):
  //
  // a * x + b * y = gcd(a, b).
  //
  // Since for any odd value of a, gcd(a, 2^sigma) = 1, solving
  //
  // a * rev_a = 1 mod 2^sigma
  //
  // is equivalent to solving the extended euclidian algorithm for:
  //settings.prefix_length * 2
  // a * rev_a + 2^sigma * y = gcd(a, 2^sigma)
  assert(a & 1);
  uint64_t x = 1, y = 0;
  uint64_t x1 = 0, y1 = 1, a1 = a, b1 = 1ul << sigma;
  while (b1) {
    uint64_t q = a1 / b1;
    _assign(x1, x - q * x1, x, x1);
    _assign(y1, y - q * y1, y, y1);
    _assign(b1, a1 - q * b1, a1, b1);
  }
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "a = " << a << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "a' = " << x << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "a * a' = " << (a * x) << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "(a * a') & " << ((1ul << sigma) - 1) << " = " << ((a * x) & ((1ul << sigma) - 1)) << endl;
  io_mutex.unlock();
#endif
  assert(((a * x) & ((1ul << sigma) - 1)) == 1);
  return x;
}

static uint64_t _rand() {
  random_device rd;
  mt19937 g(rd());
  return g();
}

static uint64_t _setCorrectOddCoefficient(uint64_t v) {
  if (!v) {
    v = _rand();
  }
  if ((v & 1) == 0) {
    ++v;
  }
  return v;
}

static uint64_t _setCorrectBitMask(uint64_t v, uint64_t mask) {
  if (!v) {
    v = _rand();
  }
  return v & mask;
}

GaBTransformer::GaBTransformer(const Settings &s, uint64_t a, uint64_t b):
  Transformer(s, "GaB"),
  _rotation_offset(settings.length),
  _rotation_mask((1ul << settings.length) - 1),
  _kmer_mask((_rotation_mask << _rotation_offset) | _rotation_mask),
  _prefix_shift((settings.length - settings.prefix_length) << 1),
  _suffix_mask((1ul << ((settings.length - settings.prefix_length) << 1)) - 1),
  _a(_setCorrectOddCoefficient(a)), _rev_a(_multiplicativeInverse(_a, 2 * settings.length)), _b(_setCorrectBitMask(b, _kmer_mask))
{
  string *desc_ptr = const_cast<string *>(&(this->description));
  *desc_ptr += "(" + to_string(_a) + "," + to_string(_b) + ")";
}

uint64_t GaBTransformer::_rotate(uint64_t s) const {
  s = (((s << _rotation_offset) | (s >> _rotation_offset)) & _kmer_mask);
  return s;
}

uint64_t GaBTransformer::_G(uint64_t s) const {
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "s = " << s << endl;
  uint64_t rotation = _rotate(s);
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "_rotate(s) = " << rotation << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "_rotate(s) ^ _b = " << (rotation ^ _b) << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "_a * (_rotate(s) ^ _b) = " << (_a * (rotation ^ _b)) << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "(_a * (_rotate(s) ^ _b)) & _kmer_mask = " << ((_a * (rotation ^ _b)) & _kmer_mask) << endl;
  io_mutex.unlock();
#endif
  return ((_a * (_rotate(s) ^ _b)) & _kmer_mask);
}

uint64_t GaBTransformer::_G_rev(uint64_t s) const {
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "s = " << s << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "_rev_a * s = " << (_rev_a * s) << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "(_rev_a * s) & _kmer_mask = " << ((_rev_a * s) & _kmer_mask) << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "((_rev_a * s) & _kmer_mask) ^ _b = " << (((_rev_a * s) & _kmer_mask) ^ _b) << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "_rotate(((_rev_a * s) & _kmer_mask) ^ _b) = " << _rotate(((_rev_a * s) & _kmer_mask) ^ _b) << endl;
  io_mutex.unlock();
#endif
  return _rotate(((_rev_a * s) & _kmer_mask) ^ _b);
}

Transformer::EncodedKmer GaBTransformer::operator()(const std::string &kmer) const {
    if (settings.length <= 32) {
      uint64_t v = _encode(kmer.c_str(), settings.length);
#ifdef DEBUG
  uint64_t orig = v;
#endif
  v = _G(v);
#ifdef DEBUG
  uint64_t rev_v = _G_rev(v);
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
  EncodedKmer e;
  e.prefix = v >> _prefix_shift;
  e.suffix = v & _suffix_mask;
  return e;
} else {
	uint64_t prefix = _encode(kmer.c_str(), 32);
        uint64_t suffix = _encode(kmer.c_str() + 32, settings.length - 32);
        uint64_t prefix_transformed = _G(prefix); 
        EncodedKmer e;
        e.prefix = prefix_transformed >> _prefix_shift;
        e.suffix = (prefix_transformed & _suffix_mask) | (suffix << ((settings.length - 32) << 1));;
        
        return e;  }

}

std::string GaBTransformer::operator()(const Transformer::EncodedKmer &e) const {
  if (settings.length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    v = _G_rev(v);
    return _decode(v, settings.length);
  } else {
    uint64_t u_transformed = (e.prefix << _prefix_shift) | (e.suffix & _suffix_mask);
    uint64_t u = _G_rev(u_transformed);
    uint64_t w = e.suffix >> ((32 - settings.prefix_length) << 1);
    return _decode(u, 32) + _decode(w, settings.length - 32);
  }
}
