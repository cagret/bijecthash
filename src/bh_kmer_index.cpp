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

#include "bh_kmer_index.hpp"

#include "common.hpp"
#include "exception.hpp"

#include <algorithm>
#include <iostream>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

///////////////////////////
// BhKmerIndex::Subindex //
///////////////////////////

BhKmerIndex::Subindex &BhKmerIndex::Subindex::operator=(const BhKmerIndex::Subindex &subindex) {
  if (this != &subindex) {
    _rw_lock.requestWriteAccess();
    base_t::operator=(subindex);
    _rw_lock.releaseWriteAccess();
  }
  return *this;
}

size_t BhKmerIndex::Subindex::size() const {
  _rw_lock.requestReadAccess();
  size_t s = base_t::size();
  _rw_lock.releaseReadAccess();
  return s;
}

bool BhKmerIndex::Subindex::insert(const value_type& value) {
  DEBUG_MSG(" suffix value is " << value << " for subindex at " << this << "." << '\n'
            << MSG_DBG_HEADER <<  "The subindex size was " /*<< base_t::size()*/);
  _rw_lock.requestWriteAccess();
  pair<base_t::iterator, bool> res = set<uint64_t>::insert(value);
  DEBUG_MSG("(" << value << "):" << this << "." << '\n'
            <<  MSG_DBG_HEADER << "Now, subindex size is " << base_t::size());
  _rw_lock.releaseWriteAccess();
  return res.second;
}


/////////////////
// BhKmerIndex //
/////////////////

BhKmerIndex::BhKmerIndex(const Settings &s):
  _rw_lock(),
  _subindexes(1ul << (2 * s.prefix_length)),
  _size(0), _transformer(s.transformer()),
  settings(s)
{
  DEBUG_MSG("Creation of " << _subindexes.size() << " subindexes "
            << "for each possible prefix of length " << s.prefix_length
            << " of this new index (" << this << ")");
  if (!_transformer) {
    Exception e;
    e << "Error: Unable to find valid transformer for settings:\n"
      << s << "\n";
    throw e;
  }
  DEBUG_MSG("Transformer is " << _transformer->description << "_{" << _transformer->kmer_length << " = " << _transformer->prefix_length << " + " << _transformer->suffix_length << "}");
}

BhKmerIndex::BhKmerIndex(const BhKmerIndex &index):
  _rw_lock(),
  _subindexes(index.size()),
  _size(index._size.load()),
  _transformer(index._transformer),
  settings(index.settings)
{
  DEBUG_MSG("Copying existing index having " << _size
            << " elements in this new index (" << this << ")");
  index._rw_lock.requestReadAccess();
  _subindexes = index._subindexes;
  index._rw_lock.releaseReadAccess();
}

BhKmerIndex &BhKmerIndex::operator=(const BhKmerIndex &index) {
  if (this != &index) {
    DEBUG_MSG("Assigning k-mers from " << &index
              << " to this current index (" << this << ")");
    index._rw_lock.requestReadAccess();
    _rw_lock.requestWriteAccess();
    _subindexes = index._subindexes;
    _size.store(index._size.load());
    *(const_cast<shared_ptr<const Transformer> *>(&_transformer)) = index._transformer;
    _rw_lock.releaseWriteAccess();
    index._rw_lock.releaseReadAccess();
  }
  return *this;
}

bool BhKmerIndex::insert(const string &kmer) {
  Transformer::EncodedKmer encoded = (*_transformer)(kmer);
#if defined(DEBUG) || not(defined(NDEBUG))
  string decoded = (*_transformer)(encoded);
  DEBUG_MSG("original kmer: '" << kmer << "'" << '\n'
            << MSG_DBG_HEADER << "decoded kmer:  '" << decoded << "'");
  assert(decoded == kmer);
#endif
  bool res = _subindexes[encoded.prefix].insert(encoded.suffix);
  if (res) {
    ++_size;
  }
  return res;
}

static string fmt(string w, size_t i, size_t max) {
  string m = to_string(max);
  string s = to_string(i);
  if (s.length() < m.length()) {
    string p(m.length() - s.length(), '0');
    s = p + s;
  }
  return s + " " + w;
}

map<string, double> BhKmerIndex::statistics() const {

  map<string, double> stats;

  _rw_lock.requestReadAccess();

  vector<size_t> sizes;
  size_t n = _subindexes.size();
  sizes.reserve(n);
  size_t nb_bins = settings.nb_bins;
  if (nb_bins > n) {
    nb_bins = n;
  }
  size_t m = nb_bins + 6;

  double mean = 0;
  double variance = 0;

  for (const auto &s: _subindexes) {
    sizes.push_back(s.size());
    mean += s.size();
    variance += s.size() * s.size();
  }
  assert(mean == _size);
  _rw_lock.releaseReadAccess();

  sort(sizes.begin(), sizes.end());
  stats[fmt("min", 1, m)] = sizes.front();
  stats[fmt("med", 2, m)] = sizes[n / 2];
  stats[fmt("max", 3, m)] = sizes.back();

  mean /= n;

  stats[fmt("mean", 4, m)] = mean;

  variance /= n;
  variance -= mean * mean;
  stats[fmt("var", 5, m)] = variance;

  vector<size_t> bins(nb_bins, 0);
  size_t bin_size = n / bins.size() + (n % bins.size() > 0);
  stats[fmt("bin_size", 6, m)] = bin_size;

  for (size_t i = 0; i < n; ++i) {
    bins[i / bin_size] += sizes[i];
  }
  sizes.clear();

  n = bins.size();
  for (size_t i = 0; i < n; ++i) {
    stats[fmt("bin_" + to_string(i+1), i + 7, m)] = bins[i];
  }
  return stats;

}

void BhKmerIndex::toStream(ostream &os) const {
  _rw_lock.requestReadAccess();
  size_t n = size();
  size_t nb = _subindexes.size();
  os << "Index (" << n << " k-mers in " << nb << " subindexes using transformer " << _transformer->description << "):\n";
  Transformer::EncodedKmer encoded;
  for (encoded.prefix = 0; encoded.prefix < nb; ++encoded.prefix) {
    _subindexes[encoded.prefix]._rw_lock.requestReadAccess();
    for (const auto &e: _subindexes[encoded.prefix]) {
      encoded.suffix = e;
      os << "- '" << (*_transformer)(encoded) << "'\n";
    }
    _subindexes[encoded.prefix]._rw_lock.releaseReadAccess();
  }
  _rw_lock.releaseReadAccess();
}

END_BIJECTHASH_NAMESPACE
