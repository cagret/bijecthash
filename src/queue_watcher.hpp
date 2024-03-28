#ifndef __QUEUE_WATCHER_H__
#define __QUEUE_WATCHER_H__

#include <circular_queue.hpp>

template <typename T>
void queueWatcher(const CircularQueue<T> &queue);

#include <queue_watcher.tpp>

#endif
