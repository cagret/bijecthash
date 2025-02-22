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

#ifndef __BH_KMER_INDEX_HPP__
#define __BH_KMER_INDEX_HPP__

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <locker.hpp>
#include <settings.hpp>
#include <transformer.hpp>

namespace bijecthash {

  /**
   * The k-mer index is merely a vector of sets of unsigned integers but
   * we need to redefine some methods of the set template for
   * thread-safety (and to forbid other methods for the sake of
   * simplicity).
   *
   * The k-mer index is currently represented by \f$4^{k_1}\f$ sets of
   * unsigned integers that belongs to \f$[k - k_1[\f$, with \f$1 \leq
   * k_1 < k\f$.
   *
   * This k-mer index class is expected to thread safe.
   */
  class BhKmerIndex {

  private:

    /**
     * A k-mer index sub-index is simply a set of 64 bits integers.
     */
    class Subindex: private std::set<uint64_t> {

    private:

      /**
       * Multiple Reader-Single writer lock
       */
      mutable ReadWriteLock _rw_lock;

      /**
       * The BhKmerIndex class needs to access the _rw_lock.
       */
      friend BhKmerIndex;

    public:

      /**
       * The base type alias.
       */
      typedef std::set<uint64_t> base_t;

      /**
       * Builds an empty sub-index.
       */
      inline Subindex(): base_t(), _rw_lock() {
      }

      /**
       * Copy constructor of sub-index.
       *
       * Notice that the reader-writer lock of this sub-index is reset.
       *
       * \param subindex The sub-index to copy.
       */
      inline Subindex(const Subindex &subindex):
        base_t(subindex), _rw_lock() {
      }

      /**
       * The assignment operator.
       *
       * \param subindex The sub-index to copy.
       *
       * \return Returns this sub-index.
       */
      Subindex &operator=(const Subindex &subindex);

      /**
       * Check if sub-index is empty.
       *
       * \return Returns true if this sub-index is empty and false
       * otherwise.
       */
      inline bool empty() const {
        return size() == 0;
      }

      /**
       * Get this sub-index size.
       *
       * \return Returns the number of elements in this sub-index.
       */
      size_t size() const;

      /**
       * Inserts the given value in the sub-index if not already present.
       *
       * \param value The value to insert.
       *
       * \return Returns true if the value was inserted and false if it
       * was already present in this sub-index.
       */
      bool insert(const value_type& value);

    };

    /**
     * Multiple Reader-Single writer lock
     */
    mutable ReadWriteLock _rw_lock;

    /**
     * The sub-indexes
     */
    std::vector<BhKmerIndex::Subindex> _subindexes;

    /**
     * The number of indexed k-mers (not the size of the vector)
     */
    std::atomic_size_t _size;

    /**
     * The transformer associated to this index.
     */
    const std::shared_ptr<const Transformer> _transformer;

  public:

    /**
     * This index/program settings
     */
    const Settings &settings;

    /**
     * Builds a new k-mer index using the given settings.
     *
     * \param s The settings to use for initialize this index.
     */
    BhKmerIndex(const Settings &s);

    /**
     * Creates an index by copying the given index.
     *
     * Notice that the reader-writer lock of this index is reset.
     *
     * \param index The k-mer index to clone.
     */
    BhKmerIndex(const BhKmerIndex &index);

    /**
     * The assignment operator.
     *
     * \param index The k-mer index to copy.
     *
     * \return Returns this index.
     */
    BhKmerIndex &operator=(const BhKmerIndex &index);

    /**
     * Check if index is empty.
     *
     * \return Returns true if this sub-index is empty and false
     * otherwise.
     */
    inline bool empty() const {
      return size() == 0;
    }

    /**
     * Get this index size.
     *
     * \return Returns the number of elements in this index.
     */
    inline size_t size() const {
      return _size.load();
    }

    /**
     * Inserts the given k-mer in this index if not already present.
     *
     * \param kmer The k-mer to insert.
     *
     * \return Returns true if the k-mer was inserted and false if it
     * was already present in this index.
     */
    bool insert(const std::string &kmer);

    /**
     * Print this index on the given stream.
     *
     * \param os The output stream on which to print this index.
     */
    void toStream(std::ostream &os) const;

    /**
     * compute statistics about the index structure.
     *
     * \return Returns a set of (key, value) where each key is prefixd
     * by an integer (with enough leading 0) in order to sort them
     * lexicographicaly.
     *
     * The number of bins defined is settings are bounded by \f$4^p\f$
     * where \f$p\f$ denotes the prefix length for the statistics
     * computations.
     *
     * Let us suppose that we have 20 bins, then the keys are:
     * - '01 min' (the lowest number of indexed k-mers in a sub-index).
     * - '02 med' (the median number of indexed k-mers in a sub-index).
     * - '03 max' (the greatest number of indexed k-mers in a sub-index).
     * - '04 mean' (the average number of indexed k-mers per sub-indexe).
     * - '05 var' (the variance of the number of indexed k-mers in sub-indexes).
     * - '06 bin_size' (the number of sub-indexes per bin).
     * - '07 bin_1' (the number of k-mers indexed in sub-indexes of the first bin).
     * - '08 bin_2' (the number of k-mers indexed in sub-indexes of the second bin).
     *    ...
     * - '26 bin_20' (the number of k-mers indexed in sub-indexes of the last bin).
     *
     * Obviously, the average number of k-mers per bin is the average
     * number of kmer per sub-index times the number of sub-indexes per
     * bin (i.e., the product of values associated to '04 mean' and '06 bin_size'.
     */
    std::map<std::string, double> statistics() const;

    /**
     * Return the index transformer in use.
     *
     * \return Returns the k-mer index transformer in use.
     */
    inline const Transformer &transformer() const {
      return *_transformer;
    }

  };

  /**
   * Print the given k-mer index on the given stream.
   *
   * \param os The output stream on which to print the given index.
   *
   * \param index The k-mer index to print.
   *
   * \return Returns the updated output stream.
   */
  inline std::ostream &operator<<(std::ostream &os, const BhKmerIndex &index) {
    index.toStream(os);
    return os;
  }

}

#endif
