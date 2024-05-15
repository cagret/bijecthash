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

#ifndef __COMPOSITION_TRANSFORMER_HPP__
#define __COMPOSITION_TRANSFORMER_HPP__

#include <memory>
#include <string>

#include <transformer.hpp>

namespace bijecthash {

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
     * \param kmer_length The length of the \f$k\f$-mer (*i.e.* the
     * value of \f$k\f$).
     *
     * \param prefix_length The length of the \f$k\f$-mer prefix.
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
    CompositionTransformer(size_t kmer_length, size_t prefix_length,
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
    virtual std::string getTransformedKmer(const EncodedKmer &e) const override;

  };

  std::shared_ptr<const CompositionTransformer> operator*(std::shared_ptr<const Transformer> &t2, std::shared_ptr<const Transformer> &t1);

}

#endif
