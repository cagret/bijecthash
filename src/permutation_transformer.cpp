#include "permutation_transformer.hpp"

#include <random>
#include <algorithm>

#ifdef DEBUG
#  include <iostream>
#  include <iomanip>
#endif

using namespace std;

PermutationTransformer::PermutationTransformer(const Settings &s, const vector<size_t> &permutation, const std::string &description):
  Transformer(s, description),
  _permutation(permutation.size() == settings.length ? permutation : _generateRandomPermutation(settings.length)),
  _reverse_permutation(_computeReversePermutation(_permutation))
{
  if (description.empty()) {
    string *desc_ptr = const_cast<string *>(&description);
    desc_ptr->clear();
    *desc_ptr += "Permutation[";
    for (size_t i = 0; i < settings.length; ++i) {
      if (i) *desc_ptr += ",";
      *desc_ptr += to_string(_permutation[i]);
    }
    *desc_ptr += "]";
  }
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "permutation:" << endl;
  cerr << "  ";
  for (size_t i = 0; i < settings.length; ++i) {
    cerr << setw(4) << i;
  }
  cerr << endl;
  cerr << "  ";
  for (size_t i = 0; i < settings.length; ++i) {
      cerr << setw(4) << _permutation[i];
  }
  cerr << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "reverse permutation:" << endl;
  cerr << "  ";
  for (size_t i = 0; i < settings.length; ++i) {
    cerr << setw(4) << i;
  }
  cerr << endl;
  cerr << "  ";
  for (size_t i = 0; i < settings.length; ++i) {
    cerr << setw(4) << _reverse_permutation[i];
  }
  cerr << endl;
  io_mutex.unlock();
#endif
}


vector<size_t> PermutationTransformer::_generateRandomPermutation(size_t k) {
  vector<size_t> p(k);
  iota(p.begin(), p.end(), 0);
  random_device rd;
  mt19937 g(rd());
  shuffle(p.begin(), p.end(), g);
  return p;
}

vector<size_t> PermutationTransformer::_computeReversePermutation(const vector<size_t> &p) {
  size_t n = p.size();
  vector<size_t> r(n);
  for (size_t i = 0; i < n; i++) {
    r[p[i]] = i;
  }
  return r;
}

string PermutationTransformer::_applyPermutation(const string &s, const vector<size_t> &p) {
  string permuted(s.size(), 0);
  for (size_t i = 0; i < s.size(); ++i) {
    permuted[i] = s[p[i]];
  }
  return permuted;
}

Transformer::EncodedKmer PermutationTransformer::operator()(const std::string &kmer) const {
  EncodedKmer e;
  string permuted_kmer = _applyPermutation(kmer, _permutation);
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "Permuted k-mer:   '" << permuted_kmer << "'" << endl;
  string unpermuted_kmer = _applyPermutation(permuted_kmer, _reverse_permutation);
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "Unpermuted k-mer: '" << unpermuted_kmer << "'" << endl;
  if (unpermuted_kmer != kmer) {
    cerr << "Error: the unpermuted k-mer"
         << " differs from the original k-mer"
         << endl;
    exit(1);
  }
  io_mutex.unlock();
#endif
  e.prefix = _encode(permuted_kmer.c_str(), settings.prefix_length);
  e.suffix = _encode(permuted_kmer.c_str() + settings.prefix_length, settings.length - settings.prefix_length);
  return e;
}

string PermutationTransformer::operator()(const Transformer::EncodedKmer &e) const {
  string permuted_kmer = _decode(e.prefix, settings.prefix_length);
  permuted_kmer += _decode(e.suffix, settings.length - settings.prefix_length);
  string kmer = _applyPermutation(permuted_kmer, _reverse_permutation);
  return kmer;
}
