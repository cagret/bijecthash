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

#ifndef __BH_KMER_COLLECTOR_HPP__
#define __BH_KMER_COLLECTOR_HPP__

#include <kmer_collector.hpp>
#include <lcp_stats.hpp>
#include <settings.hpp>

namespace bijecthash {

  /**
   * A k-mer collector helper that stores k-mers in a circular queue
   * and computes the LCP between consecutive k-mer transformations.
   *
   * This helper class allows to run the k-mer collector in a dedicated
   * thread.
   */
  class BhKmerCollector: public KmerCollector {

  private:

    /**
     * Longuest Common prefixes statistics.
     */
    LcpStats _lcp_stats;

    /**
     * The k-mer transformer used.
     */
    std::shared_ptr<const Transformer> _transformer;

    /**
     * The previously encoded k-mer (needed to computing the LCP statistics)
     */
    Transformer::EncodedKmer _prev_transformed_kmer;

    /**
     * Perform some processing on the given k-mer before enqueuing it.
     *
     * By default, this does nothing but any derived class can
     * override this method.
     *
     * \param kmer The k-mer to process before enqueuing it.
     */
    virtual void _process(std::string &kmer) override;

  public:

    /**
     * Builds a k-mer collector.
     *
     * \param settings The settings to use for the file collector.
     *
     * \param filename The name of the file to parse (see open() method).
     *
     * \param queue The queue to feed with k-mers.
     */
    BhKmerCollector(const Settings &s, const std::string &filename, CircularQueue<std::string> &queue);

    LcpStats getLcpStats(bool reset = false);

  };

}

#endif
