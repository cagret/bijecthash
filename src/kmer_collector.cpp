#include "kmer_collector.hpp"

#include "file_reader.hpp"
#include "transformer.hpp"
#include "locker.hpp"

#include <cassert>
#ifdef DEBUG
#  include <bitset>
#endif

using namespace std;

atomic_size_t KmerCollector::_counter = 0;
atomic_size_t KmerCollector::_running = 0;

KmerCollector::KmerCollector(const Settings &settings,
                             const string &filename,
                             CircularQueue<string> &queue):
  _queue(queue),
  id(++_counter), settings(settings), filename(filename)
{
  ++_running;
}

#ifdef __builtin_clzll_available
#  undef __builtin_clzll_available
#endif
#ifdef __has_builtin
#  if __has_builtin(__builtin_clzll)
#    define __builtin_clzll_available
#  endif
#endif

#ifdef DEBUG
#define BIT_MASK_POS(v, mask, offset)           \
  io_mutex.lock();                              \
  cerr << "v = " << bitset<64>(v) << endl;      \
  cerr << "m = " << bitset<64>(mask) << endl;   \
  io_mutex.unlock();                            \
  if (v & mask) {                               \
    v &= mask;                                  \
  } else {                                      \
    p += offset;                                \
  }                                             \
  io_mutex.lock();                              \
  cerr << "v = " << bitset<64>(v) << endl;      \
  cerr << "p = " << p << endl;                  \
  io_mutex.unlock();                            \
  (void)0
#else
#define BIT_MASK_POS(v, mask, offset)           \
  if (v & mask) {                               \
    v &= mask;                                  \
  } else {                                      \
    p += offset;                                \
  }                                             \
  (void)0
#endif


static int clz(uint64_t v) {
#ifdef __builtin_clzll_available
  cerr << "__builtin_clzll(" << v << ") = " << __builtin_clzll(v) << endl;
  return __builtin_clzll(v);
#else
  int p = 0;
  BIT_MASK_POS(v, 0xFFFFFFFF00000000, 32);
  BIT_MASK_POS(v, 0xFFFF0000FFFF0000, 16);
  BIT_MASK_POS(v, 0xFF00FF00FF00FF00, 8);
  BIT_MASK_POS(v, 0xF0F0F0F0F0F0F0F0, 4);
  BIT_MASK_POS(v, 0xCCCCCCCCCCCCCCCC, 2);
  BIT_MASK_POS(v, 0xAAAAAAAAAAAAAAAA, 1);
  return p;
#endif
}

static size_t _LCP(const Transformer::EncodedKmer &e1, const Transformer::EncodedKmer &e2, size_t k, size_t k1) {
  const size_t nb_bits = sizeof(uint64_t) << 3;
  uint64_t v = (e1.prefix ^ e2.prefix);
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:" << endl
       << "  e1.prefix = " << bitset<64>(e1.prefix) << endl
       << "  e2.prefix = " << bitset<64>(e2.prefix) << endl
       << "  e1 ^ e2   = " << bitset<64>(e1.prefix ^ e2.prefix) << endl
       << "  v         = " << bitset<64>(v) << endl;
  io_mutex.unlock();
#endif
  size_t res = 0;
  if (v) {
    int p = nb_bits - clz(v);
#ifdef DEBUG
    io_mutex.lock();
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "first leftmost bit set is at position " << p << endl;
    io_mutex.unlock();
#endif
    res = ((k1 << 1) - p) >> 1;
  } else {
    size_t k2 = k - k1;
    uint64_t m = (((k2 << 1) < nb_bits) ? (1ull << (k2 << 1)) - 1 : uint64_t(-1));
    v = (e1.suffix ^ e2.suffix) & m;
#ifdef DEBUG
    io_mutex.lock();
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:" << endl
         << "  k = " << k << " = " << k1 << " + " << k2 << endl
         << "  e1.suffix = " << bitset<64>(e1.suffix) << endl
         << "  e2.suffix = " << bitset<64>(e2.suffix) << endl
         << "  e1 ^ e2   = " << bitset<64>(e1.suffix ^ e2.suffix) << endl
         << "  mask      = " << bitset<64>(m) << endl
         << "  v         = " << bitset<64>(v) << endl;
    io_mutex.unlock();
#endif
    if (v) {
      int p = nb_bits - clz(v);
#ifdef DEBUG
      io_mutex.lock();
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "[thread " << this_thread::get_id() << "]:"
           << "first leftmost bit set is at position " << p << endl;
      io_mutex.unlock();
#endif
      res = (((k2 << 1) - p) >> 1) + k1;
    } else {
      res = k;
    }
  }
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:"
       << "res = " << res << endl;
  io_mutex.unlock();
#endif
  return res;
}

void KmerCollector::_run() {
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:"
       << "KmerCollector_" << id << ":"
       << "Starting file = '" << filename << "' processing." << endl;
  io_mutex.unlock();
#endif
  FileReader reader(settings, filename);

  if (!reader.isOpen()) {
    io_mutex.lock();
    cerr << "Error: Unable to open fasta/fastq file '" << filename << "'" << endl;
    io_mutex.unlock();
    exit(1);
  }

#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG]" << __FILE__ << ":" << __FUNCTION__ << ":"
       << "Test of the LCP" << endl;
  Transformer::EncodedKmer e1, e2;
  e1.prefix = e1.suffix = e2.prefix = e2.suffix = 0;
  size_t expected_lcp = (settings.length << 1) - 1;
  for (e2.suffix = 1; e2.suffix < (1ull << ((settings.length - settings.prefix_length) << 1)); e2.suffix <<= 1) {
    io_mutex.unlock();
    size_t lcp = _LCP(e1, e2, settings.length, settings.prefix_length);
    io_mutex.lock();
    cerr << "[DEBUG]" << __FILE__ << ":" << __FUNCTION__ << ":"
         << "_LCP({" << e1.prefix << ", " << e1.suffix << "},"
         << " {" << e2.prefix << ", " << e2.suffix << "}) = " << lcp
         << " (expecting " << (expected_lcp >> 1) << ")" << endl;
    assert(lcp == (expected_lcp >> 1));
    --expected_lcp;
  }
  cerr << "[DEBUG]" << __FILE__ << ":" << __FUNCTION__ << ":"
       << "================================" << endl;
  for (e2.prefix = 1; e2.prefix < (1ull << (settings.prefix_length << 1)); e2.prefix <<= 1) {
    io_mutex.unlock();
    size_t lcp = _LCP(e1, e2, settings.length, settings.prefix_length);
    io_mutex.lock();
    cerr << "[DEBUG]" << __FILE__ << ":" << __FUNCTION__ << ":"
         << "_LCP({" << e1.prefix << ", " << e1.suffix << "},"
         << " {" << e2.prefix << ", " << e2.suffix << "}) = " << lcp
         << " (expecting " << (expected_lcp >> 1) << ")" << endl;
    assert(lcp == (expected_lcp >> 1));
    --expected_lcp;
  }
  cerr << "[DEBUG]" << __FILE__ << ":" << __FUNCTION__ << ":"
       << "================================" << endl;
  io_mutex.unlock();
#endif

  Transformer::EncodedKmer prev_transformed_kmer, current_transformed_kmer;
  prev_transformed_kmer.prefix = prev_transformed_kmer.suffix = 0;
  shared_ptr<const Transformer> transformer = settings.transformer();
  size_t LCP_avg = 0;
  size_t LCP_var = 0;
  size_t nb_kmers = 0;
  while (reader.nextKmer()) {
    const string &kmer = reader.getCurrentKmer();
    current_transformed_kmer = (*transformer)(kmer);
#ifdef DEBUG
    if (reader.getCurrentKmerID(false) == 1) {
      io_mutex.lock();
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "[thread " << this_thread::get_id() << "]:"
           << "KmerCollector " << id << ":"
           << "New sequence: '" << reader.getCurrentSequenceDescription() << "'" << endl;
      io_mutex.unlock();
    }
    io_mutex.lock();
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "KmerCollector_" << id << ":"
         << "k-mer '" << reader.getCurrentKmer()
         << " (abs_ID: " << reader.getCurrentKmerID()
         << ",  rel_ID: " << reader.getCurrentKmerID(false)
         << ")" << endl;
    io_mutex.unlock();
#endif

    if (reader.getCurrentKmerID(false) != 1) {
      size_t lcp = _LCP(prev_transformed_kmer, current_transformed_kmer, settings.length, settings.prefix_length);
      LCP_avg += lcp;
      LCP_var += lcp * lcp;
      ++nb_kmers;
    }

    while (!_queue.push(kmer)) {
#ifdef DEBUG
      io_mutex.lock();
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "[thread " << this_thread::get_id() << "]:"
           << "KmerCollector_" << id << ":"
           << "Unable to push k-mer '" << kmer << "." << endl;
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "[thread " << this_thread::get_id() << "]:"
           << "KmerCollector_" << id << ":"
           << "queue size: " << _queue.size()
           << " (" << (_queue.empty() ? "empty" : "not empty")
           << ", " << (_queue.full() ? "full" : "not full") << ")." << endl;
      io_mutex.unlock();
#endif
      this_thread::yield();
      this_thread::sleep_for(10ns);
    }
#ifdef DEBUG
    io_mutex.lock();
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "KmerCollector_" << id << ":"
         << "k-mer '" << kmer << " pushed successfully." << endl;
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "KmerCollector_" << id << ":"
         << "queue size: " << _queue.size()
         << " (" << (_queue.empty() ? "empty" : "not empty")
         << ", " << (_queue.full() ? "full" : "not full") << ")." << endl;
    io_mutex.unlock();
    // cerr << _queue;
#endif
    prev_transformed_kmer = current_transformed_kmer;
  }

  _average_lcp = double(LCP_avg) / nb_kmers;
  _variance_lcp = double(LCP_var) / nb_kmers;
  _variance_lcp -= _average_lcp * _average_lcp;
  io_mutex.lock();
  cerr << "[thread " << this_thread::get_id() << "]:"
       << "KmerCollector_" << id << ":"
       << "file '" << filename << "':"
       << "LCP ~ N(" << _average_lcp << ", " << _variance_lcp << ")"
       << endl;
  io_mutex.unlock();

  --_running;
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:"
       << "KmerCollector_" << id << ":"
       << "_running = " << _running << ":"
       << "file '" << filename << "' processed." << endl;
  io_mutex.unlock();
#endif
}
