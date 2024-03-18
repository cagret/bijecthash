#include "lyndon_transformer.hpp"
//#define DEBUG

using namespace std;


Transformer::EncodedKmer LyndonTransformer::operator()(const string &kmer) const {
  EncodedKmer e;
  size_t j = 0;
  for (size_t i = 1; i < kmer.size(); ++i) {
    if (kmer.substr(i) + kmer.substr(0, i) < kmer.substr(j) + kmer.substr(0, j)) {
      j = i;
    }
  }

#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " Lyndon rotation index: " << j << endl;
#endif

  string lyndon_rotation;
  lyndon_rotation.reserve(kmer.size());
  lyndon_rotation = kmer.substr(j) + kmer.substr(0, j);

#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " Lyndon rotation: '" << lyndon_rotation << "'" << endl;
#endif

  e.prefix = _encode(lyndon_rotation.c_str(), settings.prefix_length);
  e.suffix = _encode(lyndon_rotation.c_str() + settings.prefix_length, settings.length - settings.prefix_length);
  e.lyndonIndex = j;
  return e;
}

string LyndonTransformer::operator()(const Transformer::EncodedKmer &e) const {
  string lyndon_rotation = _decode(e.prefix, settings.prefix_length) + _decode(e.suffix, settings.length - settings.prefix_length);
  string kmer = lyndon_rotation.substr(lyndon_rotation.size() - e.lyndonIndex) + lyndon_rotation.substr(0, lyndon_rotation.size() - e.lyndonIndex);
  return kmer;
}
