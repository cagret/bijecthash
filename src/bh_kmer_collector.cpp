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

#include "bh_kmer_collector.hpp"

#include "common.hpp"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

BhKmerCollector::BhKmerCollector(const Settings &s, const string &filename, CircularQueue<string> &queue):
  KmerCollector(s.length, filename, queue, s.verbose),
  _lcp_stats(), _transformer(s.transformer()), _prev_transformed_kmer()
{
  _lcp_stats.start();
}

void BhKmerCollector::_process(string &kmer) {
  DEBUG_MSG("Computing encoded k-mer for '" << kmer << " for LCP statistics");
  Transformer::EncodedKmer transformed_kmer;
  transformed_kmer = (*_transformer)(kmer);
  if (_reader.getCurrentKmerID(false) != 1) {
#ifdef DEBUG
    size_t lcp =
#endif
      _lcp_stats.LCP(_prev_transformed_kmer, transformed_kmer, _transformer->kmer_length, _transformer->prefix_length);
    DEBUG_MSG("LCP between previous encoded k-mer and '" << kmer << "' is " << lcp);
  }
  _prev_transformed_kmer = transformed_kmer;
}

LcpStats BhKmerCollector::getLcpStats(bool reset) {
  LcpStats stats = _lcp_stats;
  stats.stop();
  if (reset) {
    _lcp_stats.start();
  }
  return stats;
}

END_BIJECTHASH_NAMESPACE
