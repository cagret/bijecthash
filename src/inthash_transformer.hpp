#ifndef __INTHASH_TRANSFORMER_HPP__
#define __INTHASH_TRANSFORMER_HPP__

#include <transformer.hpp>

/**
 * The transformer that encodes k-mer using the hash64 function
 * (limited to k <= 32).
 */
class IntHashTransformer: public Transformer {

private:

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

public:

  /**
   * Builds a Transformer depending on the k-mer length and the prefix
   * length.
   *
   * \param s The global settings.
   */
  IntHashTransformer(const Settings &s);

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

