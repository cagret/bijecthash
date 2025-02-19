/******************************************************************************
*                                                                             *
*  Copyright © 2024-2025 -- LIRMM/CNRS/UM                                     *
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

#ifndef __MINIMIZER_TRANSFORMER_HPP__
#define __MINIMIZER_TRANSFORMER_HPP__

#include <cstddef>
#include <cstdint>
#include <string>

#include <transformer.hpp>

namespace bijecthash {

  /**
   * Transformer that calculates a minimizer from a k-mer and uses it for encoding.
   */
  class MinimizerTransformer : public Transformer {

  public:

    /**
     * Constructs a MinimizerTransformer with a specified minimizer length.
     *
     * \param kmer_length The length of the \f$k\f$-mer (*i.e.* the
     * value of \f$k\f$).
     *
     * \param prefix_length The length of the \f$k\f$-mer prefix.
     */
    MinimizerTransformer(size_t kmer_length, size_t prefix_length);

    /**
     * Encodes a given k-mer into a prefix/suffix code using a minimizer.
     *
     * Each derived class must overload this operator.
     *
     * \param kmer The k-mer to encode.
     *
     * \return Returns the EncodedKmer corresponding to the given k-mer.
     */
    virtual EncodedKmer operator()(const std::string &kmer) const override;

    /**
     * Decodes a given encoded k-mer back to its original string representation.
     *
     * Each derived class must overload this operator.
     *
     * \param e The encoded k-mer to decode.
     *
     * \return Returns the k-mer corresponding to the given encoding.
     */
    virtual std::string operator()(const EncodedKmer &e) const override;

  private:

    /**
     * Internal method to calculate the xorshift hash of a substring.
     *
     * \param x The value to hash hash.
     *
     * \return The hash of the given value.
     */
    uint64_t xorshift(uint64_t x) const;

  };

}

#endif
