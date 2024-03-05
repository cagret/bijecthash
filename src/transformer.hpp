#ifndef __TRANSFORMER_HPP__
#define __TRANSFORMER_HPP__

#include <string>
#include <cstdint>
#include <settings.hpp>

/**
 * A transformer is a bijective application that can transform some
 * k-mers into an equivalent prefix/suffix code combination and given
 * such a prefix/suffix code combination retrieve the original k-mer.
 *
 * This is the abstract class description.
 */
class Transformer {

protected:

  /**
   * Encodes a DNA string of length n into a 64 bits integer.
   *
   * \param dna_str The C string containing only A, C, G or T symbols.
   *
   * \param n The length of the DNA string to encode (the given string
   * must have at least n characters). The value n is limited to 32
   * since each nucleotide is encoded on 2 bits).
   *
   * \return Returns the binary encoded string such that each
   * nucleotide is encoded using two bits with A <=> 00, C <=> 01, G
   * <=> 10 and T <=> 11. The bits are right aligned.
   */
  static uint64_t _encode(const char *dna_str, size_t n);

  /**
   * Deodes a DNA string of length n from a 64 bits integer.
   *
   * \param v The encoded value to decode.
   *
   * \param n The length of the DNA string to decode (this length is
   * imited to 32 characters since each nucleotide encoding uses 2
   * bits).
   *
   * \return Returns the DNA string such that each 2 bits of the value
   * (right aligned) represent some nucleotide using A <=> 00, C <=>
   * 01, G <=> 10 and T <=> 11.
   */
  static std::string _decode(uint64_t v, size_t n);

public:


  /**
   * The tranformer settings.
   */
  const Settings settings;

  /**
   * The tranformer description.
   */
  const std::string description;

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
   * \param s The global settings.
   *
   * \param description The transformer description.
   */
  Transformer(const Settings &s, const std::string &description);

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

};

#endif
