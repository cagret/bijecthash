#include "permutation_transformer.hpp"

#include <random>
#include <algorithm>

//#define DEBUG
#ifdef DEBUG
#  include <iostream>
#  include <iomanip>
#endif

using namespace std;

PermutationTransformer::PermutationTransformer(size_t k, size_t prefix_length, const vector<size_t> &permutation, const std::string &description):
  Transformer(k, prefix_length, description),
  _permutation(permutation.size() == k ? permutation : _generateRandomPermutation(k)),
  _reverse_permutation(_computeReversePermutation(_permutation))
{
  if (description.empty()) {
    string *desc_ptr = const_cast<string *>(&_description);
    desc_ptr->clear();
    *desc_ptr += "Permutation[";
    for (size_t i = 0; i < k; ++i) {
      if (i) *desc_ptr += ",";
      *desc_ptr += to_string(_permutation[i]);
    }
    *desc_ptr += "]";
  }
#ifdef DEBUG
  cout << "permutation:" << endl;
  for (size_t i = 0; i < k; ++i) {
    cout << setw(4) << i;
  }
  cout << endl;
  for (size_t i = 0; i < k; ++i) {
    cout << setw(4) << _permutation[i];
  }
  cout << endl;
  cout << "reverse permutation:" << endl;
  for (size_t i = 0; i < k; ++i) {
    cout << setw(4) << i;
  }
  cout << endl;
  for (size_t i = 0; i < k; ++i) {
    cout << setw(4) << _reverse_permutation[i];
  }
  cout << endl;
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
  cout << "Permuted k-mer:   '" << permuted_kmer << "'" << endl;
  string unpermuted_kmer = _applyPermutation(permuted_kmer, _reverse_permutation);
  cout << "Unpermuted k-mer: '" << unpermuted_kmer << "'" << endl;
  if (unpermuted_kmer != kmer) {
    cerr << "Error: the unpermuted k-mer"
         << " differs from the original k-mer"
         << endl;
    terminate();
  }
#endif
  e.prefix = _encode(permuted_kmer.c_str(), _prefix_length);
  e.suffix = _encode(permuted_kmer.c_str() + _prefix_length, _suffix_length);
  return e;
}

string PermutationTransformer::operator()(const Transformer::EncodedKmer &e) const {
  string permuted_kmer = _decode(e.prefix, _prefix_length);
  permuted_kmer += _decode(e.suffix, _suffix_length);
  string kmer = _applyPermutation(permuted_kmer, _reverse_permutation);
  return kmer;
}
