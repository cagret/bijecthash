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

#ifndef __INTHASH_TRANSFORMER_HPP__
#define __INTHASH_TRANSFORMER_HPP__

#include <cstdint>
#include <string>

#include <transformer.hpp>

namespace bijecthash {

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
     * \param kmer_length The length of the \f$k\f$-mer (*i.e.* the
     * value of \f$k\f$).
     *
     * \param prefix_length The length of the \f$k\f$-mer prefix.
     */
    IntHashTransformer(size_t kmer_length, size_t prefix_length);

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
