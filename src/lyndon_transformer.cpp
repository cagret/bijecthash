#include "lyndon_transformer.hpp"
#include <cassert>
//#define DEBUG

using namespace std;

LyndonTransformer::LyndonTransformer(const Settings &s):
	Transformer(s, "Lyndon"){
    assert(settings.length-settings.prefix_length<=30);
  }

Transformer::EncodedKmer LyndonTransformer::operator()(const string &kmer) const {
  EncodedKmer e;
  size_t lyndon_pos = 0;
  for (size_t i = 1; i < kmer.size(); ++i) {
    if (kmer.substr(i) + kmer.substr(0, i) < kmer.substr(lyndon_pos) + kmer.substr(0, lyndon_pos)) {
      lyndon_pos = i;
    }
  }

#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " Lyndon rotation index: " << lyndon_pos << endl;
#endif

  assert(lyndon_pos < 64); //lyndon_pos can only be encoded in 6 bits
  string lyndon_rotation;
  lyndon_rotation.reserve(kmer.size());
  lyndon_rotation = kmer.substr(lyndon_pos) + kmer.substr(0, lyndon_pos);

#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " Lyndon rotation: '" << lyndon_rotation << "'" << endl;
#endif

  e.prefix = _encode(lyndon_rotation.c_str(), settings.prefix_length);
  e.suffix = _encode(lyndon_rotation.c_str() + settings.prefix_length, settings.length - settings.prefix_length);
  e.suffix |= lyndon_pos<<58;
  return e;
}

string LyndonTransformer::operator()(const Transformer::EncodedKmer &e) const {
  size_t lyndon_pos = e.suffix >> 58;
  string lyndon_rotation = _decode(e.prefix, settings.prefix_length) + _decode(e.suffix, settings.length - settings.prefix_length);
  string kmer = lyndon_rotation.substr(lyndon_rotation.size() - lyndon_pos) + lyndon_rotation.substr(0, lyndon_rotation.size() - lyndon_pos);
  return kmer;
}
