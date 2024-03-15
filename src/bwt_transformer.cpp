#include "bwt_transformer.hpp"

//#define DEBUG

using namespace std;

BWTTransformer::BWTTransformer(const Settings &s)
  : Transformer(s, "BWT")
{}

Transformer::EncodedKmer BWTTransformer::operator()(const string &kmer) const {
  EncodedKmer e;
  string str = kmer;
  size_t length = str.size();

#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " Original string: '" << str << "'" << endl;
#endif

  // Create a matrix of rotations
  vector<string> rotations;
  rotations.reserve(length);
  for (size_t i = 0; i < length; ++i) {
    rotations.push_back(str.substr(i) + str.substr(0, i));
  }

  // Sort the rotations lexicographically
  sort(rotations.begin(), rotations.end());

  // Extract the last character of each rotation
  string bwt;
  bwt.reserve(length);
  for (const auto &rotation : rotations) {
    bwt.push_back(rotation.back());
  }

#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " BWT: '" << bwt << "'" << endl;
#endif

  e.prefix = _encode(bwt.c_str(), settings.prefix_length);
  e.suffix = _encode(bwt.c_str() + settings.prefix_length, settings.length - settings.prefix_length);
  return e;
}

string BWTTransformer::operator()(const Transformer::EncodedKmer &e) const {
  string bwt = _decode(e.prefix, settings.prefix_length) + _decode(e.suffix, settings.length - settings.prefix_length);
  size_t length = bwt.size();

  // Perform the inverse BWT transformation
  string original;
  original.reserve(length);
  size_t index = 0;
  for (size_t i = 0; i < length; ++i) {
    original.push_back(bwt[index]);
    index = find(bwt.begin(), bwt.end(), bwt[index]) - bwt.begin();
  }

  return original;
}
