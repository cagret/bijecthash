#ifndef BWT_TRANSFORMER_HPP
#define BWT_TRANSFORMER_HPP

#include "transformer.hpp"
#include <vector>
#include <algorithm>

class BWTTransformer : public Transformer {
public:
  BWTTransformer(const Settings &s);

  EncodedKmer operator()(const std::string &kmer) const override;
  std::string operator()(const EncodedKmer &e) const override;
};

#endif // BWT_TRANSFORMER_HPP
