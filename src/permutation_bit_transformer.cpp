#include "permutation_bit_transformer.hpp"

#ifdef DEBUG
#  include <iostream>
#  include <iomanip>
#endif

using namespace std;

#ifdef DEBUG
#include "locker.hpp"
#endif
/*
uint64_t PermutationBitTransformer::_applyBitwisePermutation(uint64_t encoded_kmer, const vector<size_t> &permutation) const {
    assert(permutation.size() == this->_kmer_length);
    uint64_t permuted_kmer = 0;
    for (size_t i = 0; i < this->_kmer_length * 2; ++i) {
        size_t nucleotide_index = i / 2;
        size_t bit_index_in_nucleotide = i % 2;
        size_t permuted_nucleotide_index = permutation[nucleotide_index];
        size_t permuted_bit_index = permuted_nucleotide_index * 2 + bit_index_in_nucleotide;

        uint64_t bit = (encoded_kmer >> (this->_kmer_length * 2 - 1 - i)) & 1ULL;
        permuted_kmer |= (bit << (this->_kmer_length * 2 - 1 - permuted_bit_index));
    }
    return permuted_kmer;
}

*/
uint64_t PermutationBitTransformer::_applyBitwisePermutation(uint64_t encoded_kmer, const vector<size_t> &permutation) const {
    uint64_t permuted = 0;
    for (size_t i = 0; i < permutation.size(); ++i) {
        size_t bit_index = permutation[i];
        uint64_t bit = (encoded_kmer >> (2 * bit_index)) & 3;
        permuted |= bit << (2 * i);
    }
    return permuted;
}

vector<size_t> PermutationBitTransformer::_generateRandomPermutation(size_t k) {
    vector<size_t> p(k);
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
  uint64_t bin_kmer_encoded = _encode(kmer.c_str(), kmer.size());
  size_t suffix_length = kmer.size() - settings.prefix_length;
#ifdef DEBUG
  cerr << "[######### DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "Bin_kmer_encoded k-mer: '" << bin_kmer_encoded << "'" << endl;
#endif
  uint64_t permuted = _applyBitwisePermutation(bin_kmer_encoded, _permutation);
#if defined(DEBUG) || not(defined(NDEBUG))
  uint64_t unpermuted_kmer = _applyBitwisePermutation(permuted, _reverse_permutation);
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[________ DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "Permuted k-mer:   '" << permuted << "'" << endl;
  cerr << "[######### DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "Unpermuted k-mer: '" << unpermuted_kmer << "'" << endl;
  io_mutex.unlock();
#endif
  assert(bin_kmer_encoded == unpermuted_kmer);
#endif
  EncodedKmer e;
  e.prefix = permuted >> (2 * suffix_length);
  e.suffix = permuted & ((1ull << (2 * suffix_length)) - 1);
#ifdef DEBUG
  cerr << "[_________ DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "prefix'" << e.prefix << "'"
       << "prefix'" << e.suffix << "'" << endl;
#endif
  return e;
}


string PermutationBitTransformer::operator()(const EncodedKmer &e) const {
    size_t suffix_length = this->_kmer_length - this->settings.prefix_length;
#ifdef DEBUG
    cerr << "[######### DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "prefix_length: '" << settings.prefix_length << "'"
         << "kmer_length: '" << this->_kmer_length << "'"
         << "suffix_length: '" << suffix_length << "'"
         << "prefix_length: '" << settings.prefix_length << "'"
         << "prefix: '" << e.prefix << "'"
	 << "suffix: '" << e.suffix << "'" << endl;
#endif
    uint64_t combined = (e.prefix << (2 * suffix_length)) | e.suffix;
#ifdef DEBUG
    cerr << "[######### DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "COMBINED k-mer: '" << combined << "'" << endl;
#endif
    uint64_t original_permuted = _applyBitwisePermutation(combined, _reverse_permutation);
#ifdef DEBUG
    cerr << "[######### DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "Original_permuted k-mer: '" << original_permuted << "'" << endl;
#endif
    string decoded = _decode(original_permuted, this->_kmer_length);
#ifdef DEBUG
    cerr << "[######### DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "Decoded k-mer: '" << decoded << "'" << endl;
#endif
    return decoded;
}

PermutationBitTransformer::PermutationBitTransformer(const Settings &s, const vector<size_t> &permutation, const std::string &description)
  : Transformer(s, description), _kmer_length(s.length),
  _permutation(permutation.size() == settings.length ? permutation : _generateRandomPermutation(settings.length)),
  _reverse_permutation(_computeReversePermutation(_permutation))
{
  if (description.empty()) {
    string *desc_ptr = const_cast<string *>(&(this->description));
    desc_ptr->clear();
    *desc_ptr += "Permutation_bins[";
    for (size_t i = 0; i < settings.length; ++i) {
      if (i) *desc_ptr += ",";
      *desc_ptr += to_string(_permutation[i]);
    }
    *desc_ptr += "]";
  }
/*
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "description: '" << description << "'" << endl;
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
*/
}
