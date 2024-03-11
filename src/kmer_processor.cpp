#include "kmer_processor.hpp"

#include "kmer_collector.hpp"
#include "locker.hpp"

#include <cassert>

using namespace std;


atomic_size_t KmerProcessor::_counter = 0;
atomic_size_t KmerProcessor::_running = 0;


KmerProcessor::KmerProcessor(const Settings &settings,
                             KmerIndex &index,
                             CircularQueue<string> &queue):
  _queue(queue), _index(index),
  id(++_counter), settings(settings)
{
  ++_running;
}

void KmerProcessor::_run() {
  while ((KmerCollector::running() > 0) || !_queue.empty()) {
    string kmer;
#ifdef DEBUG
    io_mutex.lock();
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "KmerProcessor_" << id << ":"
         << "Running KmerProcessor: " << _running << "/" << _counter << endl;
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "[thread " << this_thread::get_id() << "]:"
         << "KmerProcessor_" << id << ":"
         << "queue size: " << _queue.size() << endl;
    io_mutex.unlock();
#endif
    bool ok = _queue.pop(kmer);
    while (!_queue.empty() && !ok) {
      ok = _queue.pop(kmer);
#ifdef DEBUG
      io_mutex.lock();
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "[thread " << this_thread::get_id() << "]:"
           << "KmerProcessor_" << id << ":"
           << "Unable to pop any kmer (queue size: " << _queue.size() << ")." << endl;
      io_mutex.unlock();
#endif
      this_thread::yield();
      this_thread::sleep_for(1ns);
    }
    if (ok) {
#ifdef DEBUG
      io_mutex.lock();
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "[thread " << this_thread::get_id() << "]:"
           << "KmerProcessor_" << id << ":"
           << "k-mer '" << kmer << "' successfully popped." << endl;
      io_mutex.unlock();
#endif

      _index.insert(kmer);

    } else {
      this_thread::yield();
      this_thread::sleep_for(1ns);
    }
  }
  --_running;
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "[thread " << this_thread::get_id() << "]:"
       << "KmerProcessor_" << id << ":"
       << "_running = " << _running << ":"
       << "KmerProcessor_" << id << " has finished." << endl;
  io_mutex.unlock();
  cerr << _queue;
#endif
}
