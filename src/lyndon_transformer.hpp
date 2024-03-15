#ifndef __LYNDON_TRANSFORMER_HPP__
#define __LYNDON_TRANSFORMER_HPP__

#include "transformer.hpp"
#include <algorithm>

/**
 * The transformer that encodes k-mer using the Lyndon rotation.
 */
class LyndonTransformer: public Transformer {

public:

  /**
   * Builds a Transformer depending on the global settings.
   *
   * \param s The global settings.
   */
  LyndonTransformer(const Settings &s);

  /**
   * Encode some given k-mer into its Lyndon rotation.
   *
   * \param kmer The k-mer to encode.
   *
   * \return Returns the EncodedKmer corresponding to the given k-mer.
   */
  virtual EncodedKmer operator()(const std::string &kmer) const override;

  /**
   * Decode some given encoded k-mer.
   *
   * \param e The encoded k-mer to decode.
   *
   * \return Returns the k-mer corresponding to the given encoding.
   */
  virtual std::string operator()(const EncodedKmer &e) const override;

};

#endif
