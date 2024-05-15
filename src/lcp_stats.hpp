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

#ifndef __LCP_STATS_HPP__
#define __LCP_STATS_HPP__

#include <transformer.hpp>

namespace bijecthash {

  /**
   * Structure to handle Longuest Common prefixes statistics.
   */
  struct LcpStats {
    /**
     * The total number of k-mers used to compute the statistics.
     */
    size_t nb_kmers;

    /**
     * The average length of the longuest common prefix between two
     * consecutive transformed k-mers (and overlapping k-mers in the
     * data).
     */
    double average;

    /**
     * The observed variance of the length the longuest common prefix
     * between two consecutive transformed k-mers (and overlapping
     * k-mers in the data).
     */
    double variance;

    /**
     * Builds a default LCP statistics structure where all values are
     * set to 0.
     */
    inline LcpStats(): nb_kmers(0), average(0), variance(0) {
    }

    size_t LCP(const Transformer::EncodedKmer &e1, const Transformer::EncodedKmer &e2, size_t k, size_t k1);

    inline void start() {
      nb_kmers = 0;
      average = 0;
      variance = 0;
    }

    inline void stop() {
      if (nb_kmers) {
        average /= (double) nb_kmers;
        variance /= (double) nb_kmers;
        variance -= average * average;
      }
    }

  };

}

#endif
