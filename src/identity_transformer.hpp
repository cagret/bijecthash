#ifndef __IDENTITY_TRANSFORMER_HPP__
#define __IDENTITY_TRANSFORMER_HPP__

#include <transformer.hpp>

/**
 * The "no-op" transformer that just encodes k-mer without any
 * transformation.
 */
class IdentityTransformer: public Transformer {

public:

  /**
   * Builds a Transformer depending on the k-mer length and the prefix
   * length.
   *
   * \param s The global settings.
   */
  inline IdentityTransformer(const Settings &s):
    Transformer(s, "Identity") {}

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
