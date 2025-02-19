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

#ifndef __KMER_COLLECTOR_HPP__
#define __KMER_COLLECTOR_HPP__

#include <cstddef>
#include <string>

#include <threaded_processor_helper.hpp>
#include <file_reader.hpp>

namespace bijecthash {

  /**
   * A k-mer collector helper that stores k-mers in a circular queue.
   *
   * This helper class allows to run the k-mer collector in a dedicated
   * thread.
   */
  class KmerCollector: public ThreadedProcessorHelper<KmerCollector, std::string> {

  private:

    /**
     * Read the k-mers from the associated file and store them into the
     * queue.
     *
     * This method will exit only when the file will be entirely
     * parsed. When the queue is full, it waits until some other thread
     * consumes some k-mers to release some space.
     */
    void _run() override final;

    /**
     * Perform some processing on the given k-mer before enqueuing it.
     *
     * By default, this does nothing but any derived class can
     * override this method.
     *
     * \param kmer The k-mer to process before enqueuing it.
     */
    virtual void _process(std::string &kmer);

  protected:

    /**
     * The reader handled by this k-mer collector.
     */
    FileReader _reader;

  public:

    /**
     * Builds a k-mer collector.
     *
     * \param k The length of k-mer to extract (*i.e.*, the
     * value of \f$k\f$).
     *
     * \param filename The name of the file to parse (see open() method).
     *
     * \param queue The queue to feed with k-mers.
     *
     * \param verbose Don't emit warnings when is set to \c
     * false.
     */
    KmerCollector(size_t k, const std::string &filename, CircularQueue<std::string> &queue, bool verbose = true);

  };

}

#endif
