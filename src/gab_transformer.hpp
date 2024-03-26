#ifndef GAB_TRANSFORMER_HPP
#define GAB_TRANSFORMER_HPP

#include <transformer.hpp>

/**
 * Random Hashing function that operates a permutation based on a bit
 * rotation (general case of inthash64).
 *
 * More formally, this defined a bijective permutation on \f$[0;
 * 2^{\sigma}[\f$ such that for some odd multiplciative value \f$a\f$
 * and some \f$b\f$ a \f$\sigma\f$ bit offset, \f$G_{a,b}(s) = \left(a
 * \times \left(\mathrm{rot}(s)\oplus
 * b\right)\right)\;\mathrm{mod}\;{2^\sigma}\f$ (where
 * \f$\mathrm{rot}(s)\f$ denotes an inversion of the two halves of the
 * bits representing \f$s\f$.
 */
class GaBTransformer : public Transformer {

private:

  /**
   * Precomputed rotation offset.
   */
  const uint64_t _rotation_offset;

  /**
   * Precomputed rotation mask.
   */
  const uint64_t _rotation_mask;

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
   * Odd multiplier.
   */
  const uint64_t _a;

  /**
   * The odd multiplier multiplicative inverse
   * (\f$\mathrm{mod}\;2^\sigma\f$.
   */
  const uint64_t _rev_a;

  /**
   * Offset on _sigma bits.
   */
  const uint64_t _b;

  /**
   * Compute a cyclic rotation of half of the bits of
   * the s binary value.
   *
   * \param s The bits to rotate.
   *
   * \return Returns the rotated bits of the s binary value.
   */
  uint64_t _rotate(uint64_t s) const;

  /**
   * The static function that computes the permutation of s given a,
   * b, and sigma.
   *
   * \param s The bits to permute
   *
   * \return Return the image of s by the permutation.
   */
  uint64_t _G(uint64_t s) const;

  /**
   * The static function that computes the inverse permutation of s
   * given rev_a, b, and sigma.
   *
   * \param s The bits to restore (reverse permute)
   *
   * \return Return the preimage of s by the permutation.
   */
  uint64_t _G_rev(uint64_t s) const;

public:

  /**
   * Builds a Transformer depending on the k-mer length and the prefix
   * length.
   *
   * \param s The global settings.
   *
   * \param a The odd multiplier (if zero, then generate a random
   * number, if non zero but even, then use a + 1).
   *
   * \param b The bits to permute using a bitwise xor operation (if
   * zero, then generate a random mask).
   */
  GaBTransformer(const Settings &s, uint64_t a = 0, uint64_t b = 0);

  /**
   * Encode some given k-mer into a prefix/suffix code.
   *
   * Each derived class must overload this operator.
   *
   * \param kmer The k-mer to encode.
   *
   * \return Returns the EncodedKmer corresponding to the given k-mer.
   */
  virtual Transformer::EncodedKmer operator()(const std::string &kmer) const override;

  /**
   * Decode some given encoded k-mer.
   *
   * Each derived class must overload this operator.
   *
   * \param e The encoded k-mer to decode.
   *
   * \return Returns the k-mer corresponding to the given encoding.
   */
  virtual std::string operator()(const Transformer::EncodedKmer &e) const override;

  /**
   * Get the transformed k-mer from its encoding.
   *
   * Each derived class that operates a transformation at the bit
   * level instead of the nucleotide level should overload this
   * operator.
   *
   * \param e The encoded k-mer to restitute.
   *
   * \return Returns the k-mer corresponding to the given encoding.
   */
  virtual std::string getTransformedKmer(const EncodedKmer &e) const;

};

#endif
