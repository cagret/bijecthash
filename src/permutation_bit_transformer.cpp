#include "permutation_bit_transformer.hpp"

#ifdef DEBUG
#  include <iostream>
#  include <iomanip>
#endif

using namespace std;

#ifdef DEBUG
#include "locker.hpp"
#endif

uint64_t PermutationBitTransformer::_applyBitwisePermutation(uint64_t encoded_kmer, const vector<size_t> &permutation) const {
  uint64_t permuted = 0;
  for (size_t i = 0; i < permutation.size(); ++i) {
    size_t bit_index = permutation[i];
    uint64_t bit = (encoded_kmer >> bit_index) & 1;
    permuted |= (bit << i);
  }
  return permuted;
}

vector<size_t> PermutationBitTransformer::_generateRandomPermutation(size_t n) {
  vector<size_t> p(n);
  iota(p.begin(), p.end(), 0);
  random_device rd;
  mt19937 g(rd());
  shuffle(p.begin(), p.end(), g);
  return p;
}

vector<size_t> PermutationBitTransformer::_computeReversePermutation(const vector<size_t> &p) {
  size_t n = p.size();
  vector<size_t> r(n);
  for (size_t i = 0; i < n; ++i) {
    r[p[i]] = i;
  }
  return r;
}

Transformer::EncodedKmer PermutationBitTransformer::operator()(const std::string &kmer) const {
  EncodedKmer e;
  assert(kmer.size() == settings.length);
  if (settings.length <= 32) {
    uint64_t v = _encode(kmer.c_str(), settings.length);
#ifdef DEBUG
    uint64_t orig = v;
#endif
    v = _applyBitwisePermutation(v, _permutation);
#ifdef DEBUG
    uint64_t rev_v = _applyBitwisePermutation(v, _reverse_permutation);
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
    uint64_t prefix_transformed = _applyBitwisePermutation(prefix, _permutation);
    e.prefix = prefix_transformed >> _prefix_shift;
    e.suffix = prefix_transformed << (settings.prefix_length << 1) | suffix;
  }
  return e;
}

string PermutationBitTransformer::operator()(const EncodedKmer &e) const {
  if (settings.length <= 32) {
    uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
    v = _applyBitwisePermutation(v, _reverse_permutation);
    return _decode(v, settings.length);
  } else {
    uint64_t u = (e.prefix << _prefix_shift) | (e.suffix >> (settings.prefix_length << 1));
    u = _applyBitwisePermutation(u, _reverse_permutation);
    uint64_t v = e.suffix & _suffix_mask;
    return _decode(u, 32) + _decode(v, settings.length - 32);
  }
}

PermutationBitTransformer::PermutationBitTransformer(const Settings &s, const vector<size_t> &permutation, const std::string &description)
  : Transformer(s, description),
    _permutation(permutation.size() == 2 * settings.length ? permutation : _generateRandomPermutation(settings.length <= 32 ? 2 * settings.length : 64)),
    _reverse_permutation(_computeReversePermutation(_permutation)),
    _kmer_mask((1ull << settings.length << settings.length) - 1ull),
    _prefix_shift((((settings.length > 32) ? 32 : settings.length) - settings.prefix_length) << 1),
    _suffix_mask((settings.length > 32) ? ((1ull << ((settings.length - 32) << 1)) - 1) : ((1ull << ((settings.length - settings.prefix_length) << 1)) - 1))
{
  if (description.empty()) {
    string *desc_ptr = const_cast<string *>(&(this->description));
    desc_ptr->clear();
    *desc_ptr += "Permutation_bins[";
    for (size_t i = 0; i < 2 * settings.length; ++i) {
      if (i) *desc_ptr += ",";
      *desc_ptr += to_string(_permutation[i]);
    }
    *desc_ptr += "]";
  }

#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "description: '" << description << "'" << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "permutation:" << endl;
  cerr << "  ";
  for (size_t i = 0; i < 2 * settings.length; ++i) {
    cerr << setw(4) << i;
  }
  cerr << endl;
  cerr << "  ";
  for (size_t i = 0; i < 2 * settings.length; ++i) {
    cerr << setw(4) << _permutation[i];
  }
  cerr << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "reverse permutation:" << endl;
  cerr << "  ";
  for (size_t i = 0; i < 2 * settings.length; ++i) {
    cerr << setw(4) << i;
  }
  cerr << endl;
  cerr << "  ";
  for (size_t i = 0; i < 2 * settings.length; ++i) {
    cerr << setw(4) << _reverse_permutation[i];
  }
  cerr << endl;
  io_mutex.unlock();
#endif
}
