#ifndef __CACHE_STATISTICS_H__
#define __CACHE_STATISTICS_H__

#include <map>
#include <string>

/**
 * Wrapper around the perf_event_open() function in order to gather
 * cache statistics.
 */
class CacheStatistics: private std::map<std::string, uint64_t> {

 private:

  /**
   * The group file descriptor on which cache statistics are
   * collected.
   */
  int _fd;

  /**
   * Status of this instance (whether this is currently catching cache
   * events or not).
   */
  bool _started;

  /**
   * Association mapping between event id and the configuration of the
   * opened file descriptors.
   */
  std::map<uint64_t, std::string> _cache_evt_id_labels;

  /**
   * Create a file descriptor to catch cache events corresponding the
   * given configuration.
   *
   * \param config The config flag (see perf_event_open() manpage for
   * more details).
   *
   * \return Returns true if the watcher was successfully created
   * (i.e., if the wanted config is available on the current platform).
   */
  bool _createPerfEventWatcher(uint32_t config);

 public:

  /**
   * The base type alias.
   */
  typedef std::map<std::string, uint64_t> base_t;

  /**
   * Create a cache event observer.
   *
   * This automatically call the reset() method but doesn't start the
   * monitoring (you need to explicitely call the start() method).
   */
  CacheStatistics();

  /**
   * Check whether this instance is collecting data or not.
   *
   * \return Returns true if the current instance is currently collecting data.
   */
  inline bool started() const {
    return ((_fd != -1) && _started);
  }

  /**
   * Reset the observed cache event counters.
   */
  void reset();

  /**
   * Start monitoring cache events.
   */
  void start();

  /**
   * Stop monitoring cache events.
   */
  void stop();

  /**
   * Update counters associated to monitored cache events.
   */
  void update();

  using base_t::const_iterator;
  using base_t::begin;
  using base_t::end;

};

#endif

