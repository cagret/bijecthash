#include "gab_transformer.hpp"

#include <cassert>
#include <random>

#ifdef DEBUG
#  include <bitset>
#  include "locker.hpp"
#endif

using namespace std;

// The following functions (_f64() and _multiplicativeInverse()) are
// adapted from
// https://lemire.me/blog/2017/09/18/computing-the-inverse-of-odd-integers/
static uint64_t _f64(uint64_t a, uint64_t x) {
  return x * (2 - a * x);
}

uint64_t _multiplicativeInverse(uint64_t a, size_t sigma) {
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
  uint64_t x = (3 * a) ^ 2; // 5 bits
  x = _f64(a, x); // 10 bits
  x = _f64(a, x); // 20 bits
  x = _f64(a, x); // 40 bits
  x = _f64(a, x); // 80 bits
  uint64_t mask = ((sigma < 64) ? ((1ull << sigma) - 1) : uint64_t(-1));
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "a = " << a << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "a' = " << x << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "a * a' = " << (a * x) << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "(a * a') & " << bitset<64>(mask) << " = " << ((a * x) & mask) << endl;
  io_mutex.unlock();
#endif
  assert(((a * x) & mask) == 1);
  return x & mask;
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
  _rotation_offset(settings.length > 32 ? 32 : settings.length),
  _rotation_mask((1ull << settings.length) - 1ull),
  _kmer_mask((_rotation_mask << _rotation_offset) | _rotation_mask),
  _prefix_shift((((settings.length > 32) ? 32 : settings.length) - settings.prefix_length) << 1),
  _suffix_mask((settings.length > 32) ? ((1ull << ((settings.length - 32) << 1)) - 1) : ((1ull << ((settings.length - settings.prefix_length) << 1)) - 1)),
  _a(_setCorrectOddCoefficient(a)), _rev_a(_multiplicativeInverse(_a, settings.length > 32 ? 64 : 2 * settings.length)), _b(_setCorrectBitMask(b, _kmer_mask))
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
       << "s = " << s << ", "
       << "_a = " << _a << ", "
       << "_b = " << _b << endl;
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
       << "_rev_a * s = " << "(" << _rev_a << " * " << s << ") = " << (_rev_a * s) << endl;
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
  EncodedKmer e;
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
    e.prefix = v >> _prefix_shift;
    e.suffix = v & _suffix_mask;
  } else {
    uint64_t prefix = _encode(kmer.c_str(), 32);
    uint64_t suffix = _encode(kmer.c_str() + 32, settings.length - 32);
    uint64_t prefix_transformed = _G(prefix);
    e.prefix = prefix_transformed >> _prefix_shift;
    e.suffix = prefix_transformed << (settings.prefix_length << 1) | suffix;
  }
  return e;
}

std::string GaBTransformer::operator()(const Transformer::EncodedKmer &e) const {
  if (settings.length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    v = _G_rev(v);
    return _decode(v, settings.length);
  } else {
    uint64_t u = (e.prefix << _prefix_shift) | (e.suffix >> (settings.prefix_length << 1));
    u = _G_rev(u);
    uint64_t v = e.suffix & _suffix_mask;
    return _decode(u, 32) + _decode(v, settings.length - 32);
  }
}

std::string GaBTransformer::getTransformedKmer(const Transformer::EncodedKmer &e) const {
  if (settings.length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    return _decode(v, settings.length);
  } else {
    uint64_t u = (e.prefix << _prefix_shift) | (e.suffix >> (settings.prefix_length << 1));
    uint64_t v = e.suffix & _suffix_mask;
    return _decode(u, 32) + _decode(v, settings.length - 32);
  }
}
