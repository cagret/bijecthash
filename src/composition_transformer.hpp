#ifndef __COMPOSITION_TRANSFORMER_HPP__
#define __COMPOSITION_TRANSFORMER_HPP__

#include <transformer.hpp>
#include <string>

/**
 * The composition transformer encodes k-mer after applying the
 * composition of two transformers.
 */
class CompositionTransformer: public Transformer {

protected:

  /**
   * First transformer
   */
  std::shared_ptr<const Transformer> _t1;

  /**
   * First transformer
   */
  std::shared_ptr<const Transformer> _t2;

public:

  /**
   * Builds a transformer which is a composition of transformers.
   *
   * \param s The global settings.
   *
   * \param t1 The k-mer transformer to apply to the k-mer.
   *
   * \param t2 The k-mer transformer to apply to the k-mer transformed
   * by transformer1.
   *
   * \param description If given, then the description string is used
   * instead of the "Composition(<transformer1>,<transformer2>)" auto
   * generated string.
   */
  CompositionTransformer(const Settings &s,
                         std::shared_ptr<const Transformer> &t1,
                         std::shared_ptr<const Transformer> &t2,
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

  /**
   * Get the transformed k-mer from its encoding.
   *
   * \param e The encoded k-mer to restitute.
   *
   * \return Returns the k-mer corresponding to the given encoding.
   */
  virtual std::string getTransformedKmer(const EncodedKmer &e) const;

};

std::shared_ptr<const CompositionTransformer> operator*(std::shared_ptr<const Transformer> &t2, std::shared_ptr<const Transformer> &t1);

#endif
