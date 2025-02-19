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

#ifndef __BH_KMER_PROCESSOR_HPP__
#define __BH_KMER_PROCESSOR_HPP__

#include <string>

#include <kmer_processor.hpp>
#include <bh_kmer_index.hpp>

namespace bijecthash {

  /**
   * A k-mer processor helper that load k-mers from a circular queue
   * and store them in a k-mer index.
   *
   * This helper class allows to run the k-mer processor in a dedicated
   * thread.
   */
  class BhKmerProcessor: public KmerProcessor {

  private:

    /**
     * The (thread-safe) k-mer index.
     */
    BhKmerIndex &_index;

    /**
     * Store the given k-mer in the k-mer index.
     *
     * \param kmer The k-mer to process after having been dequeued.
     */
    virtual void _process(std::string &kmer) override;

  public:

    /**
     * Builds a Bijecthash k-mer processor.
     *
     * \param index The (thread-safe) k-mer index.
     *
     * \param queue The queue storing the k-mers to process.
     */
    BhKmerProcessor(BhKmerIndex &index, CircularQueue<std::string> &queue);

  };

}

#endif
