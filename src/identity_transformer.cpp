#include "identity_transformer.hpp"

using namespace std;

Transformer::EncodedKmer IdentityTransformer::operator()(const std::string &kmer) const {
  EncodedKmer e;
  e.prefix = _encode(kmer.c_str(), _prefix_length);
  e.suffix = _encode(kmer.c_str() + _prefix_length, _suffix_length);
  return e;
}

string IdentityTransformer::operator()(const Transformer::EncodedKmer &e) const {
  string kmer = _decode(e.prefix, _prefix_length);
  kmer += _decode(e.suffix, _suffix_length);
  return kmer;
}