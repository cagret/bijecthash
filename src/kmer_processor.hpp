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

#ifndef __KMER_PROCESSOR_HPP__
#define __KMER_PROCESSOR_HPP__

#include <string>

#include <threaded_processor_helper.hpp>

namespace bijecthash {

  /**
   * A k-mer processor helper that load k-mers from a circular queue.
   *
   * This helper class allows to run the k-mer processor in a dedicated
   * thread.
   */
  class KmerProcessor: public ThreadedProcessorHelper<KmerProcessor, std::string> {

  private:

    /**
     * Load the k-mers from the queue and process them.
     *
     * This method will exit only when there is no more running k-mer
     * collector (see KmerCollector class) AND if the queue is empty. If
     * one of these two condition is not met, it waits.
     */
    void _run() override final;

    /**
     * Perform some processing on the given k-mer after having been
     * dequeued.
     *
     * By default, this does nothing but any derived class should
     * override this method.
     *
     * \param kmer The k-mer to process after having been dequeued.
     */
    virtual void _process(std::string &kmer);

  public:

    /**
     * Builds a k-mer processor.
     *
     * \param queue The queue storing the k-mers to process.
     */
    KmerProcessor(CircularQueue<std::string> &queue);

  };

}

#endif
