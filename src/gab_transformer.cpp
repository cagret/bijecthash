#include "gab_transformer.hpp"

using namespace std;

GaBTransformer::GaBTransformer(const Settings &s, uint64_t a, uint64_t b)
  : Transformer(s, "GaB"),
    _a(a), _b(b),
    _prefix_shift(settings.prefix_length * 2),
    _suffix_mask((1ULL << ((settings.length - settings.prefix_length) * 2)) - 1) {
}


uint64_t GaBTransformer::rot(uint64_t s, size_t width) {
  const size_t bitSize = 64;
  size_t halfWidth = width / 2;
  return (s << halfWidth) | (s >> (bitSize - halfWidth));
}

uint64_t GaBTransformer::Ga_b(uint64_t s, uint64_t a, uint64_t b, size_t sigma) {
  const uint64_t mask = (1ULL << sigma) - 1;
  return ((a * (rot(s, sigma) ^ b)) & mask);
}

Transformer::EncodedKmer GaBTransformer::operator()(const std::string &kmer) const {
  uint64_t v = _encode(kmer.c_str(), settings.length);
  v = Ga_b(v, _a, _b, 2 * settings.length);
  EncodedKmer e;
  e.prefix = v >> _prefix_shift;
  e.suffix = v & _suffix_mask;
  return e;
}

std::string GaBTransformer::operator()(const Transformer::EncodedKmer &e) const {
  uint64_t v = (e.prefix << _prefix_shift) | e.suffix;
  v = Ga_b(v, _a, _b, _sigma);
  return _decode(v, settings.length);
}
