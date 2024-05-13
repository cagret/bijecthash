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

#ifndef __TRANSFORMER_HPP__
#define __TRANSFORMER_HPP__

#include <cstdint>
#include <string>

namespace bijecthash {

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
     * The transformer k-mer length (i.e., the value of \f$k\f$).
     */
    const size_t kmer_length;

    /**
     * The transformer k-mer prefix length.
     */
    const size_t prefix_length;

    /**
     * The transformer k-mer suffix length.
     */
    const size_t suffix_length;

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
     * @brief Decodes an empty string of a given length.
     *
     * This function decodes a string of length `n` that would be encoded as 0.
     * Essentially, it returns a string of `n` 'A' characters, as 'A' is decoded from 0.
     *
     * @param n The length of the string to decode.
     * @return The decoded string of length `n`, consisting of all 'A' characters.
     */
    static std::string decode_empty(size_t n) {
      return _decode(0, n);
    }

    /**
     * Builds a Transformer depending on the k-mer length and the prefix
     * length.
     *
     * \param kmer_length The length of the \f$k\f$-mer (*i.e.* the
     * value of \f$k\f$).
     *
     * \param prefix_length The length of the \f$k\f$-mer prefix.
     *
     * \param description The transformer description.
     */
    Transformer(size_t kmer_length, size_t prefix_length, const std::string &description);

    /**
     * Destructor
     */
    inline virtual ~Transformer() {}

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
