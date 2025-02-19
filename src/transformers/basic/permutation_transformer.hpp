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

#ifndef __PERMUTATION_TRANSFORMER_HPP__
#define __PERMUTATION_TRANSFORMER_HPP__

#include <cstddef>
#include <string>
#include <vector>

#include <transformer.hpp>

namespace bijecthash {

  /**
   * The permutation transformer encodes k-mer after applying some given
   * permutation.
   */
  class PermutationTransformer: public Transformer {

  protected:

    /**
     * The permutation to apply to some k-mer before encoding it.
     */
    const std::vector<size_t> _permutation;

    /**
     * The reverse permutation wich allow to retrieve the original k-mer
     * from the permuted one.
     */
    const std::vector<size_t> _reverse_permutation;

    /**
     * This method generates a random permutation of the range [0; k[.
     *
     * \param k The length of the range.
     *
     * \return Return a random permutation of the range [0; k[.
     */
    static std::vector<size_t> _generateRandomPermutation(size_t k);

    /**
     * This method computes the reverse permutation of the given one.
     *
     * \param p Some permutation of the range [0; |p|[
     *
     * \return Returns the reverse permutation of the given one.
     */
    static std::vector<size_t> _computeReversePermutation(const std::vector<size_t> &p);

    /**
     * This applies the given permutation to the given string.
     *
     * \param s The string to permute.
     *
     * \param p The permutation of [0; |s|[ to apply.
     *
     * \return Returns the permuted string.
     */
    static std::string _applyPermutation(const std::string &s, const std::vector<size_t> &p);

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
     * \param permutation The k-mer permutation to apply (default is to generate a random permutation).
     *
     * \param description If given, then the description string is used
     * instead of the "Permutation[<details>]" auto generated string.
     */
    PermutationTransformer(size_t kmer_length, size_t prefix_length,
                           const std::vector<size_t> &permutation = std::vector<size_t>(),
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

  };

}

#endif
