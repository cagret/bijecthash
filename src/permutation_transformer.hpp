#ifndef __PERMUTATION_TRANSFORMER_HPP__
#define __PERMUTATION_TRANSFORMER_HPP__

#include <transformer.hpp>
#include <vector>
#include <string>

class PermutationTransformer: public Transformer {

protected:

  const std::vector<size_t> _permutation;
  const std::vector<size_t> _reverse_permutation;

  static std::vector<size_t> _generateRandomPermutation(size_t k);
  static std::vector<size_t> _computeReversePermutation(const std::vector<size_t> &p);
  static std::string _applyPermutation(const std::string &s, const std::vector<size_t> &p);

public:

  /**
   * Builds a Transformer depending on the k-mer length and the prefix
   * length.
   *
   * \param k The k-mer length.
   *
   * \param prefix_length The k-mer prefix length.
   *
   * \param permutation The k-mer permutation to apply (default is to generate a random permutation).
   *
   * \param description If given, then the description string is used
   * instead of the "Permutation[<details>]" auto generated string.
   */
  PermutationTransformer(size_t k, size_t prefix_length,
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

