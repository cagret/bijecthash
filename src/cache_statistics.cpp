#include "cache_statistics.hpp"

#include <iostream>
// For getting cache performances (see perf_event_open() manpage for
// more explanations).
#include <linux/perf_event.h>    /* PERF_* constants */
#include <linux/hw_breakpoint.h> /* HW_* constants */
#include <sys/syscall.h> /* SYS_* constants */
#include <sys/ioctl.h> /* ioctl() */
#include <unistd.h> /* read() */
#include <cerrno> /* errno */
#include <cstring> /* strerror() */

using namespace std;

static uint32_t PERF_AVAILABLE_CACHE_LEVEL[] = {PERF_COUNT_HW_CACHE_L1D, PERF_COUNT_HW_CACHE_LL};
static uint32_t PERF_AVAILABLE_CACHE_OP[] = {PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_OP_WRITE, PERF_COUNT_HW_CACHE_OP_PREFETCH};
static uint32_t PERF_AVAILABLE_CACHE_RESULT[] = {PERF_COUNT_HW_CACHE_RESULT_ACCESS, PERF_COUNT_HW_CACHE_RESULT_MISS};
static const size_t NB_AVAILABLE_CACHE_LEVEL = sizeof(PERF_AVAILABLE_CACHE_LEVEL) / sizeof(uint32_t);
static const size_t NB_AVAILABLE_CACHE_OP = sizeof(PERF_AVAILABLE_CACHE_OP) / sizeof(uint32_t);
static const size_t NB_AVAILABLE_CACHE_RESULT = sizeof(PERF_AVAILABLE_CACHE_RESULT) / sizeof(uint32_t);

struct perf_event_cache_data_format {
  uint64_t nr;
  uint64_t time_enabled;  /* if PERF_FORMAT_TOTAL_TIME_ENABLED */
  uint64_t time_running;  /* if PERF_FORMAT_TOTAL_TIME_RUNNING */
  struct {
    uint64_t value;
    uint64_t id;
  } values[NB_AVAILABLE_CACHE_LEVEL * NB_AVAILABLE_CACHE_OP * NB_AVAILABLE_CACHE_RESULT];
};

enum PerfConfigFields {
                       PERF_CONFIG_LEVEL_ONLY,
                       PERF_CONFIG_OPERATION_ONLY,
                       PERF_CONFIG_RESULT_ONLY,
                       PERF_CONFIG_FULL
};

static string perfConfig2string(uint32_t config, PerfConfigFields fields = PERF_CONFIG_FULL) {
  string level, op, result, full;
  if ((fields == PERF_CONFIG_LEVEL_ONLY) || (fields == PERF_CONFIG_FULL)) {
    switch (config & 255) {
    case PERF_COUNT_HW_CACHE_L1D:
      level += "L1-dcache";
      break;
    case PERF_COUNT_HW_CACHE_LL:
      level += "LLC";
      break;
    default:
      level += "<unknown>";
    }
  }
  if ((fields == PERF_CONFIG_OPERATION_ONLY) || (fields == PERF_CONFIG_FULL)) {
    switch ((config >> 8) & 255) {
    case PERF_COUNT_HW_CACHE_OP_READ:
      op = "load";
      full = "s";
      break;
    case PERF_COUNT_HW_CACHE_OP_WRITE:
      op = "store";
      full = "s";
      break;
    case PERF_COUNT_HW_CACHE_OP_PREFETCH:
      op = "prefetch";
      full = "es";
      break;
    default:
      op = "<unknown>";
    }
  }
  if ((fields == PERF_CONFIG_RESULT_ONLY) || (fields == PERF_CONFIG_FULL)) {
    switch ((config >> 16) & 255) {
    case PERF_COUNT_HW_CACHE_RESULT_ACCESS:
      result = "access";
      full = level + "-" + op + full;
      break;
    case PERF_COUNT_HW_CACHE_RESULT_MISS:
      result = "miss";
      full = level + "-" + op + "-misses";
      break;
    default:
      result = "<unknown>";
      full = level + "-" + op + "-" + result;
    }
  }
  switch (fields) {
  case PERF_CONFIG_LEVEL_ONLY: return level;
  case PERF_CONFIG_OPERATION_ONLY: return op;
  case PERF_CONFIG_RESULT_ONLY: return result;
  case PERF_CONFIG_FULL: return full;
  }
  return "<unknown>";
}

bool CacheStatistics::_createPerfEventWatcher(uint32_t config) {
  struct perf_event_attr pea;
  memset(&pea, 0, sizeof(struct perf_event_attr));
  pea.type = PERF_TYPE_HW_CACHE;
  pea.size = sizeof(struct perf_event_attr);
  pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID | PERF_FORMAT_TOTAL_TIME_RUNNING | PERF_FORMAT_TOTAL_TIME_ENABLED;
  pea.config = config;
  pea.disabled = 1;
  pea.inherit = 0;
  pea.exclude_user = 0;
  pea.exclude_kernel = 1;
  pea.exclude_hv = 1;
  pea.exclude_idle = 1;
  pea.task = 0;
  int cur_fd = syscall(__NR_perf_event_open, &pea,
                       0 /* pid */, -1 /* cpu */, _fd /* group_fd */,
                       PERF_FLAG_FD_CLOEXEC /* flags */);
  uint64_t event_id;
  string label = perfConfig2string(pea.config);
  if (cur_fd == -1) {
    event_id = 0;
    cerr << "WARNING: Unable to collect informations for " << label << ": ";
    switch (errno) {
    case EACCES: cerr << "CAP_SYS_ADMIN permissions required" << endl; break;
    case ENODEV: cerr << "Feature not supported by your CPU" << endl; break;
    case ENOENT: cerr << "Unsupported event on your platform" << endl; break;
    case EPERM: cerr << "Unsupported configuration on your platform" << endl; break;
    default:  cerr << strerror(errno) << endl; break;
    }
  } else {
    if (_fd == -1) {
      _fd = cur_fd;
    }
    ioctl(cur_fd, PERF_EVENT_IOC_ID, &event_id);
    _cache_evt_id_labels[event_id] = label;
  }
  return event_id;
}

CacheStatistics::CacheStatistics(): _fd(-1), _started(false), _cache_evt_id_labels() {
  for (auto cache_level: PERF_AVAILABLE_CACHE_LEVEL) {
    for (auto cache_op: PERF_AVAILABLE_CACHE_OP) {
      for (auto cache_result: PERF_AVAILABLE_CACHE_RESULT) {
        const uint32_t config = cache_level | (cache_op << 8) | (cache_result << 16);
#ifdef DEBUG
        bool res =
#endif
          _createPerfEventWatcher(config);
#ifdef DEBUG
        cerr << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "(" << value << "):" << this << "."
             << "cache event watcher "
             << (res ? "successfully initialized" : "initialization failure")
             << "." << endl;
#endif
      }
    }
  }
  if (_cache_evt_id_labels.empty()) {
     cerr << "WARNING: Unable to open file descriptors to collect cache statistics." << endl
         << "Please, check the system kernel 'perf_event_paranoid' parameter"
         << " (by running something like 'sysctl kernel.perf_event_paranoid'"
         << " or 'cat /proc/sys/kernel/perf_event_paranoid')." << endl
         << "This parameter value should be set to at most 2 in order to be able"
         << " to collect cache statistics." << endl;
  }
  reset();
}

void CacheStatistics::reset() {
  if (_fd == -1) return;
  ioctl(_fd, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
  base_t::clear();
}

void CacheStatistics::start() {
  if (started()) return;
  ioctl(_fd, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
  _started = true;
}

void CacheStatistics::stop() {
  if (!started()) return;
  ioctl(_fd, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
  _started = false;
}

void CacheStatistics::update() {
  if (_fd == -1) return;
  bool _started_orig = started();
  stop();
  base_t::clear();
  struct perf_event_cache_data_format cache_data;
  if (read(_fd, &cache_data, sizeof(cache_data)) == -1) {
    cerr << "Error while reading cache statistics using file descriptor " << _fd << ": " << strerror(errno) << endl;
  } else {
#ifdef DEBUG
    cerr << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "(" << value << "):" << this << "."
         << "[cache data] {"
         << "number of records = " << cache_data.nr << ", "
         << "time enabled = " << cache_data.time_enabled << ", "
         << "time running = " << cache_data.time_running
         << "}" << endl;
#endif
    for (size_t i = 0; i < cache_data.nr; ++i) {
      const uint64_t id = cache_data.values[i].id;
      const string label = _cache_evt_id_labels[id];
      const uint64_t count = cache_data.values[i].value;
      base_t::insert({label, count});
    }
  }
  if (_started_orig) {
    start();
  }
}
