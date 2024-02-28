#ifndef __TRANSFORMER_HPP__
#define __TRANSFORMER_HPP__

#include <string>
#include <cstdint>

/**
 * A transformer is a bijective application that can transform some
 * k-mers into an equivalent prefix/suffix code combination and given
 * such a prefix/suffix code combination retrieve the original k-mer.
 *
 * This is the abstract class description.
 */
class Transformer {

protected:

  const size_t _prefix_length;
  const size_t _suffix_length;
  const std::string _description;

  static uint64_t _encode(const char *dna_str, size_t n);
  static std::string _decode(uint64_t v, size_t n);

public:

  /**
   * Data type of a encoded k-mer.
   */
  struct EncodedKmer {
    uint64_t prefix; /**< The encoded prefix */
    uint64_t suffix; /**< The encoded suffix */
  };

  /**
   * Builds a Transformer depending on the k-mer length and the prefix
   * length.
   *
   * \param k The k-mer length.
   *
   * \param prefix_length The k-mer prefix length.
   *
   * \param description The transformer description.
   */
  Transformer(size_t k, size_t prefix_length, const std::string &description);

  /**
   * Get this transformer k-mer length.
   *
   * \return Returns this transformer k-mer length.
   */
  inline size_t getKmerLength() const {
    return _prefix_length + _suffix_length;
  }

  /**
   * Get this transformer prefix length.
   *
   * \return Returns this transformer prefix length.
   */
  inline size_t getKmerPrefixLength() const {
    return _prefix_length;
  }

  /**
   * Encode some given k-mer into a prefix/suffix code.
   *
   * Each derived class must overload this operator.
   *
   * \param kmer The k-mer to encode.
   *
   * \return Returns the EncodedKmer corresponding to the given k-mer.
   */
  virtual EncodedKmer operator()(const std::string &kmer) const = 0;

  /**
   * Decode some given encoded k-mer.
   *
   * Each derived class must overload this operator.
   *
   * \param e The encoded k-mer to decode.
   *
   * \return Returns the k-mer corresponding to the given encoding.
   */
  virtual std::string operator()(const EncodedKmer &e) const = 0;

  /**
   * Return the transformer description.
   *
   * \return Returns the transformer description.
   */
  inline std::string description() const {
    return _description;
  }
  
};

#endif
