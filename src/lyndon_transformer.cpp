#include "lyndon_transformer.hpp"
//#define DEBUG

using namespace std;

LyndonTransformer::LyndonTransformer(const Settings &s):
  Transformer(s, "Lyndon")
{}


Transformer::EncodedKmer LyndonTransformer::operator()(const string &kmer) const {
  EncodedKmer e;
  string str = kmer;
  size_t length = str.size();

#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " Original string: '" << str << "'" << endl;
#endif

  size_t j = 0;
  for (size_t i = 1; i < length; ++i) {
    if (str.substr(i) + str.substr(0, i) < str.substr(j) + str.substr(0, j)) {
      j = i;
    }
  }

#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " Lyndon rotation index: " << j << endl;
#endif

  string lyndon_rotation;
  lyndon_rotation.reserve(length);
  lyndon_rotation = str.substr(j) + str.substr(0, j);

#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " Lyndon rotation: '" << lyndon_rotation << "'" << endl;
#endif

  e.prefix = _encode(lyndon_rotation.c_str(), settings.prefix_length);
  e.suffix = _encode(lyndon_rotation.c_str() + settings.prefix_length, settings.length - settings.prefix_length);
  return e;
}

string LyndonTransformer::operator()(const Transformer::EncodedKmer &e) const {
  return _decode(e.prefix, settings.prefix_length) + _decode(e.suffix, settings.length - settings.prefix_length);
}
