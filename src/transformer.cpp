#include "transformer.hpp"
#include <cassert>
#include <iostream>

using namespace std;

uint64_t Transformer::_encode(const char *dna_str, size_t n) {
  assert(n <= (4 * sizeof(uint64_t)));
  uint64_t encoded = 0;
  for (size_t i = 0; i < n; ++i) {
    int val = 0;
    switch (dna_str[i]) {
    case 'A': val = 0; break;
    case 'C': val = 1; break;
    case 'G': val = 2; break;
    case 'T': val = 3; break;
    default:
      cerr << "Error: unable to encode the given k-mer (" << string(dna_str, n) << ")"
           << " since it contains the symbol '" << dna_str[i] << "'"
           << " which is neither A nor C nor G nor T." << endl;
      terminate();
    }
    encoded = (encoded << 2) | val;
  }
  return encoded;
}

string Transformer::_decode(uint64_t v, size_t n) {
  string decoded(n, '?');
  while (n--) {
    decoded[n] = "ACGT"[v & 3];
    v >>= 2;
  }
  return decoded;
}

Transformer::Transformer(size_t k, size_t prefix_length, const string &description):
  _prefix_length(prefix_length),
  _suffix_length(k - prefix_length),
  _description(description)
{
  assert(_prefix_length > 0);
  assert(_suffix_length > 0);
  assert(_prefix_length <= (4 * sizeof(uint64_t)));
  assert(_suffix_length <= (4 * sizeof(uint64_t)));
}
