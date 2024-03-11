#include "kmer_index.hpp"

#include <cassert>
#include <thread>
#include <iostream>
#include <algorithm>

using namespace std;

/////////////////////////
// KmerIndex::Subindex //
/////////////////////////

KmerIndex::Subindex &KmerIndex::Subindex::operator=(const KmerIndex::Subindex &subindex) {
  if (this != &subindex) {
    _rw_lock.requestWriteAccess();
    base_t::operator=(subindex);
    _rw_lock.releaseWriteAccess();
  }
  return *this;
}

size_t KmerIndex::Subindex::size() const {
  _rw_lock.requestReadAccess();
  size_t s = base_t::size();
  _rw_lock.releaseReadAccess();
  return s;
}

bool KmerIndex::Subindex::insert(const value_type& value) {
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "(" << value << "):" << this << "."
       <<  " The subindex size was " << base_t::size()
       << endl;
  io_mutex.unlock();
#endif
  _rw_lock.requestWriteAccess();
  pair<base_t::iterator, bool> res = set<uint64_t>::insert(value);
#ifdef DEBUG
  io_mutex.lock();
  cerr << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "(" << value << "):" << this << "."
       <<  " Now, subindex size is " << base_t::size()
       << endl;
  io_mutex.unlock();
#endif
  _rw_lock.releaseWriteAccess();
  return res.second;
}


///////////////
// KmerIndex //
///////////////

KmerIndex::KmerIndex(const Settings &s):
  _rw_lock(),
  _subindexes(1ul << (2 * s.prefix_length)),
  _size(0), _transformer(s.transformer()),
  settings(s)
{
  if (!_transformer) {
    cerr << "Error: Unable to find valid transformer for settings:" << endl
         << s
         << endl;;
    exit(1);
  }
}

KmerIndex::KmerIndex(const KmerIndex &index):
  _rw_lock(),
  _subindexes(index.size()),
  _size(index._size.load()),
  _transformer(index._transformer),
  settings(index.settings)
{
  index._rw_lock.requestReadAccess();
  _subindexes = index._subindexes;
  index._rw_lock.releaseReadAccess();
}

KmerIndex &KmerIndex::operator=(const KmerIndex &index) {
  if (this != &index) {
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

bool KmerIndex::insert(const string &kmer) {
  Transformer::EncodedKmer encoded = (*_transformer)(kmer);
#ifdef DEBUG
  string decoded = (*_transformer)(encoded);
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:"
       << "original kmer: '" << kmer << "'" << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:"
       << "decoded kmer:  '" << decoded << "'" << endl;
  io_mutex.unlock();
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

map<string, double> KmerIndex::statistics() const {

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

void KmerIndex::toStream(ostream &os) const {
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
