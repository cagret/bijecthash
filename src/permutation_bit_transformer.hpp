#ifndef __PERMUTATION_BIT_TRANSFORMER_HPP__
#define __PERMUTATION_BIT_TRANSFORMER_HPP__

#include <transformer.hpp>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <bitset>
#include <cassert>
#include <numeric>

/**
 * The permutation transformer encodes k-mer after applying some given
 * permutation on the bits.
 */
class PermutationBitTransformer: public Transformer {

private:

  /**
   * This applies the given permutation to the bits of the given string.
   *
   * \param encoded_kmer The value to permute (interpreted as a bit sequence).
   *
   * \param permutation The permutation of the bits to apply.
   *
   * \return Returns the permuted value (interpreted as a bit sequence).
   */
  uint64_t _applyBitwisePermutation(uint64_t encoded_kmer, const std::vector<size_t> &permutation) const;

protected:

  /**
   * The permutation to apply to the bits of the k-mer.
   */
  const std::vector<size_t> _permutation;

  /**
   * The reverse permutation which allows to retrieve the original k-mer
   * from the permuted one.
   */
  const std::vector<size_t> _reverse_permutation;


  /**
   * Precomputed binary mask for retrieving the whole k-mer.
   */
  const uint64_t _kmer_mask;

  /**
   * Precomputed shift offset for retrieving the prefix.
   */
  const uint64_t _prefix_shift;

  /**
   * Precomputed binary mask for retrieving the suffix.
   */
  const uint64_t _suffix_mask;

  /**
   * This method generates a random permutation of the range [0; k[.
   *
   * \param k The length of the range.
   *
   * \return Return a random permutation of the range [0; k[.
   */
  static std::vector<size_t> _generateRandomPermutation(size_t k);

  /**
   * This method computes the reverse permutation of the given one.
   *
   * \param p Some permutation of the range [0; |p|[
   *
   * \return Returns the reverse permutation of the given one.
   */
  static std::vector<size_t> _computeReversePermutation(const std::vector<size_t> &p);

public:

  /**
   * Builds a Transformer depending on the k-mer length and the prefix
   * length.
   *
   * \param s The global settings.
   *
   * \param permutation The bit permutation to apply (default is to generate a random permutation).
   *
   * \param description If given, then the description string is used
   * instead of the "Permutation[<details>]" auto-generated string.
   */
  PermutationBitTransformer(const Settings &s,
                            const std::vector<size_t> &permutation = std::vector<size_t>(),
                            const std::string &description = "");
  /**
   * Encode some given k-mer into a prefix/suffix code.
   *
   * Each derived class must overload this operator.
   *
   * \param kmer The k-mer to encode.
   *
   * \return Returns the EncodedKmer corresponding to the given k-mer.
   */
  virtual EncodedKmer operator()(const std::string &kmer) const override;

  /**
   * Decode some given encoded k-mer.
   *
   * Each derived class must overload this operator.
   *
   * \param e The encoded k-mer to decode.
   *
   * \return Returns the k-mer corresponding to the given encoding.
   */
  virtual std::string operator()(const EncodedKmer &e) const override;

};

#endif

