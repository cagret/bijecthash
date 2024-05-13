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

#ifndef __KMER_COLLECTOR_HPP__
#define __KMER_COLLECTOR_HPP__

#include <atomic>
#include <string>
#include <thread>

#include <circular_queue.hpp>
#include <settings.hpp>

namespace bijecthash {

  /**
   * A k-mer collector helper that stores k-mers in a circular queue.
   *
   * This helper class allows to run the k-mer collector in a dedicated
   * thread.
   */
  class KmerCollector {

  public:

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

    };

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
     * Longuest Common prefixes statistics.
     */
    LcpStats &_lcp_stats;

    /**
     * The circular queue feed by this k-mer collector.
     */
    CircularQueue<std::string> &_queue;

    /**
     * The thread running this collector.
     */
    std::thread _thread;

    /**
     * Read the k-mers from the associated file and store them into the
     * queue.
     *
     * This method will exit only when the file will be entirely
     * parsed. When the queue is full, it waits until some other thread
     * consumes some k-mers to release some space.
     */
    void _run();

  public:

    /**
     * This k-mer collector id
     */
    const size_t id;

    /**
     * The settings used by this k-mer collector.
     */
    const Settings &settings;

    /**
     * The filename associated to this k-mer collector.
     */
    const std::string &filename;

    /**
     * Builds a k-mer collector.
     *
     * \param settings The settings to use for the file collector.
     *
     * \param filename The name of the file to read.
     *
     * \param queue The queue to feed with k-mers.
     *
     * \param lcp_stats The variable to fill with the LCP computed
     * statistics.
     */
    KmerCollector(const Settings &settings,
                  const std::string &filename,
                  CircularQueue<std::string> &queue,
                  LcpStats &lcp_stats);

    /**
     * Start the dedicated thread.
     *
     * \see The k-mer queue feeding is performed by the private _run()
     * method.
     */
    inline void run() {
      _thread = std::thread(&KmerCollector::_run, this);
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
