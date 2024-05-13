/******************************************************************************
*                                                                             *
*  Copyright © 2024      -- LIRMM/CNRS/UM                                     *
*                           (Laboratoire d'Informatique, de Robotique et de   *
*                           Microélectronique de Montpellier /                *
*                           Centre National de la Recherche Scientifique /    *
*                           Université de Montpellier)                        *
*                           CRIStAL/CNRS/UL                                   *
*                           (Centre de Recherche en Informatique, Signal et   *
*                           Automatique de Lille /                            *
*                           Centre National de la Recherche Scientifique /    *
*                           Université de Lille)                              *
*                                                                             *
*  Auteurs/Authors:                                                           *
*                   Clément AGRET      <cagret@mailo.com>                     *
*                   Annie   CHATEAU    <annie.chateau@lirmm.fr>               *
*                   Antoine LIMASSET   <antoine.limasset@univ-lille.fr>       *
*                   Alban   MANCHERON  <alban.mancheron@lirmm.fr>             *
*                   Camille MARCHET    <camille.marchet@univ-lille.fr>        *
*                                                                             *
*  Programmeurs/Programmers:                                                  *
*                   Clément AGRET      <cagret@mailo.com>                     *
*                   Alban   MANCHERON  <alban.mancheron@lirmm.fr>             *
*                                                                             *
*  -------------------------------------------------------------------------  *
*                                                                             *
*  This file is part of BijectHash.                                           *
*                                                                             *
*  BijectHash is free software: you can redistribute it and/or modify it      *
*  under the terms of the GNU General Public License as published by the      *
*  Free Software Foundation, either version 3 of the License, or (at your     *
*  option) any later version.                                                 *
*                                                                             *
*  BijectHash is distributed in the hope that it will be useful, but WITHOUT  *
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      *
*  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   *
*  more details.                                                              *
*                                                                             *
*  You should have received a copy of the GNU General Public License along    *
*  with BijectHash. If not, see <https://www.gnu.org/licenses/>.              *
*                                                                             *
******************************************************************************/

#ifndef GAB_TRANSFORMER_HPP
#define GAB_TRANSFORMER_HPP

#include <cstdint>
#include <string>

#include <transformer.hpp>

namespace bijecthash {

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
     * \param kmer_length The length of the \f$k\f$-mer (*i.e.* the
     * value of \f$k\f$).
     *
     * \param prefix_length The length of the \f$k\f$-mer prefix.
     *
     * \param a The odd multiplier (if zero, then generate a random
     * number, if non zero but even, then use a + 1).
     *
     * \param b The bits to permute using a bitwise xor operation (if
     * zero, then generate a random mask).
     */
    GaBTransformer(size_t kmer_length, size_t prefix_length,
                   uint64_t a = 0, uint64_t b = 0);

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

}

#endif
