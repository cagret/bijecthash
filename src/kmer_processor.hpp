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

#ifndef __KMER_PROCESSOR_HPP__
#define __KMER_PROCESSOR_HPP__

#include <atomic>
#include <string>
#include <thread>

#include <circular_queue.hpp>
#include <kmer_index.hpp>
#include <settings.hpp>

namespace bijecthash {

  /**
   * A k-mer processor helper that load k-mers from a circular queue.
   *
   * This helper class allows to run the k-mer processor in a dedicated
   * thread.
   */
  class KmerProcessor {

  private:

    /**
     * The total number of created instances.
     */
    static std::atomic_size_t _counter;

    /**
     * The total number of running instances (even if thread is not
     * started).
     */
    static std::atomic_size_t _running;

    /**
     * The circular queue feed by this k-mer collector.
     */
    CircularQueue<std::string> &_queue;

    /**
     * The thread running this collector.
     */
    std::thread _thread;

    /**
     * The (thread-safe) k-mer index.
     */
    KmerIndex &_index;

    /**
     * Load the k-mers from the queue and add them to the given index.
     *
     * This method will exit only when there is no more running k-mer
     * collector (see KmerCollector class) AND if the queue is empty. If
     * one of these two condition is not met, it waits.
     */
    void _run();

  public:

    /**
     * This k-mer processor id
     */
    const size_t id;

    /**
     * The settings used by this k-mer processor.
     */
    const Settings &settings;

    /**
     * Builds a k-mer processor.
     *
     * \param settings The settings to use for the file collector.
     *
     * \param index The (thread-safe) k-mer index.
     *
     * \param queue The queue storing the k-mers to process.
     */
    KmerProcessor(const Settings &settings,
                  KmerIndex &index,
                  CircularQueue<std::string> &queue);


    /**
     * Start the dedicated thread.
     *
     * \see The k-mer dequeuing and processing is performed by the
     * private _run() method.
     */
    inline void run() {
      _thread = std::thread(&KmerProcessor::_run, this);
    }

    /**
     * This join the thread when it ends.
     */
    inline void join() {
      _thread.join();
    }

    /**
     * Get the number of create instances.
     *
     * \return Return the number of created instances.
     */
    static inline size_t counter() {
      return _counter;
    }

    /**
     * Get the total number of running instances.
     *
     * \return Return the total number of running instances. An instance
     * is considered as running from its creation and until it reaches
     * the end of the _run() method.
     */
    static inline size_t running() {
      return _running;
    }

  };

}

#endif
