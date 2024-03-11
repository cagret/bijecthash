#include "kmer_collector.hpp"

#include "file_reader.hpp"

#include <locker.hpp>

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

  while (reader.nextKmer()) {
    const string &kmer = reader.getCurrentKmer();
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
  }
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
