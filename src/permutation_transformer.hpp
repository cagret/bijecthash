#ifndef __PERMUTATION_TRANSFORMER_HPP__
#define __PERMUTATION_TRANSFORMER_HPP__

#include <transformer.hpp>
#include <vector>
#include <string>

/**
 * The permutation transformer encodes k-mer after applying some given
 * permutation.
 */
class PermutationTransformer: public Transformer {

protected:

  /**
   * The permutation to apply to some k-mer before encoding it.
   */
  const std::vector<size_t> _permutation;

  /**
   * The reverse permutation wich allow to retrieve the original k-mer
   * from the permuted one.
   */
  const std::vector<size_t> _reverse_permutation;

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

  /**
   * This applies the given permutation to the given string.
   *
   * \param s The string to permute.
   *
   * \param p The permutation of [0; |s|[ to apply.
   *
   * \return Returns the permuted string.
   */
  static std::string _applyPermutation(const std::string &s, const std::vector<size_t> &p);

public:

  /**
   * Builds a Transformer depending on the k-mer length and the prefix
   * length.
   *
   * \param s The global settings.
   *
   * \param permutation The k-mer permutation to apply (default is to generate a random permutation).
   *
   * \param description If given, then the description string is used
   * instead of the "Permutation[<details>]" auto generated string.
   */
  PermutationTransformer(const Settings &s,
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
  virtual EncodedKmer operator()(const std::string &kmer) const;

  /**
   * Decode some given encoded k-mer.
   *
   * Each derived class must overload this operator.
   *
   * \param e The encoded k-mer to decode.
   *
   * \return Returns the k-mer corresponding to the given encoding.
   */
  virtual std::string operator()(const EncodedKmer &e) const;

};

#endif

