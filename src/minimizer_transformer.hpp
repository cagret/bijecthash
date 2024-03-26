#ifndef __MINIMIZER_TRANSFORMER_HPP__
#define __MINIMIZER_TRANSFORMER_HPP__

#include <limits>
#include <string>
#include <algorithm>

#include "transformer.hpp"

/**
 * Transformer that calculates a minimizer from a k-mer and uses it for encoding.
 */
class MinimizerTransformer : public Transformer {

public:

  /**
   * Constructs a MinimizerTransformer with a specified minimizer length.
   *
   * \param k The length of the minimizer.
   */
  MinimizerTransformer(const Settings &settings);
  /**
   * Encodes a given k-mer into a prefix/suffix code using a minimizer.
   *
   * Each derived class must overload this operator.
   *
   * \param kmer The k-mer to encode.
   *
   * \return Returns the EncodedKmer corresponding to the given k-mer.
   */
  virtual EncodedKmer operator()(const std::string &kmer) const override;

  /**
   * Decodes a given encoded k-mer back to its original string representation.
   *
   * Each derived class must overload this operator.
   *
   * \param e The encoded k-mer to decode.
   *
   * \return Returns the k-mer corresponding to the given encoding.
   */
  virtual std::string operator()(const EncodedKmer &e) const override;

private:

  /**
   * Internal method to calculate the xorshift hash of a substring.
   *
   * \param s The string to hash.
   * \param start The starting index of the substring.
   * \param end The ending index of the substring.
   *
   * \return The hash of the substring.
   */
  uint64_t xorshift(uint64_t x) const;

};

#endif // __MINIMIZER_TRANSFORMER_HPP__
