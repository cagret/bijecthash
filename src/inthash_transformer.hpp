#ifndef __INTHASH_TRANSFORMER_HPP__
#define __INTHASH_TRANSFORMER_HPP__

#include <transformer.hpp>

class IntHashTransformer: public Transformer {

private:

  const uint64_t _kmer_mask;
  const uint64_t _prefix_shift;
  const uint64_t _suffix_mask;

public:

  /**
   * Builds a Transformer depending on the k-mer length and the prefix
   * length.
   *
   * \param k The k-mer length.
   *
   * \param prefix_length The k-mer prefix length.
   */
  IntHashTransformer(size_t k, size_t prefix_length);

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

