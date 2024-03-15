#include "program_options.hpp"

#include <libgen.h>
#include <cstdlib>
#include <algorithm>

using namespace std;

const vector<string> ProgramOptions::available_methods =
  {
   "identity",
   "random",
   "cyclic",
   "lyndon",
   "bwt",
   "inverse",
   "zigzag",
   "inthash",
   "GAB[=a,b]"
  };

const Settings ProgramOptions::default_settings =
  {
   21 /* length */,
   10 /* prefix_length */,
   available_methods[0] /* method */,
   "" /* tag */,
   100 /* nb_bins */,
   1024 /* queue_size */,
   true /* verbose */
  };

// Build a filtering comparison operator based on the given method
// name.
//
// Method names are truncated to the first occurence of either '[' or
// '=' before being compared.
struct sameMethod {
  const string m;
  static string basename(const string &s) {
    return s.substr(0, s.find_first_of("[="));
  }
  sameMethod(const string &m): m(basename(m)) {}
  bool operator()(const string &method) {
    return basename(method) == m;
  }
};

void ProgramOptions::usage() const {
  cerr << endl
       << "Usage: " << _program_name << " [options] [-[-]] <filename> [<filename> ...]" << endl
       << "Where available options are:" << endl
       << " -h | --help" << "\t\t\t" << "Show this message then exit." << endl
       << " - | --" << "\t\t\t" << "Can be use to process file whose name starts by a dash." << endl
       << " -q | --quiet" << "\t\t\t" << "Don't print warnings and informations about ignored k-mers." << endl
       << " -V | --verbose" << "\t\t\t" << "Print warnings and informations about ignored k-mers." << endl
       << " -k | --length <value>" << "\t\t" << "Set the k-mer length (default: " << default_settings.length << ")." << endl
       << " -p | --prefix-length <value>" << "\t" << "Set the prefix length of k-mers (default: " << default_settings.prefix_length << ")." << endl
       << " -n | --nb-bins <value>" << "\t" << "Number of bins for the computed statistics (default: " << default_settings.nb_bins << ")." << endl
       << " -s | --queue-size <value>" << "\t" << "Size of the circular queue (rounded to the ceiling power of two) used to share k-mers between collectors and processors (default: " << default_settings.queue_size << " -k-mers)." << endl
       << " -t | --tag <string>" << "\t" << "The experiment tag (default is the coma separated list of input files)." << endl
       << " -m | --method <method>" << "\t\t" << "The k-mer transformation method to use (default: " << default_settings.method << ")." << endl
       << endl
       << "Available methods are:" << endl;
  for (auto m: available_methods) {
    cerr << "  - " << m << endl;
  }
  cerr << endl
       << "Default verbosity is set to '" << (default_settings.verbose ? "verbose" : "quiet") << "'." << endl
       << endl;
  exit(1);
}

ProgramOptions::ProgramOptions(int argc, char** argv):
  _program_name(basename(argv[0])),
  _settings(default_settings),
  _filenames()
{

  bool options_accepted = true;
  string tag;
  int i = 0;
  _filenames.reserve(argc - 1);

  while (++i < argc) {

    if (options_accepted && (argv[i][0] == '-')) {
      // An option is provided
      string opt;
      int err = 0;

      if (argv[i][1] == '-') {
        // A long option is provided
        opt = &argv[i][2];
      } else {
        // A short option is provided
        opt = &argv[i][1];
      }

      if ((opt == "help") || (opt == "h")) {
        usage();
      } else if (opt.empty()) {
        options_accepted = false;
      } else if ((opt == "quiet") || (opt == "q")) {
        _settings.verbose = false;
      } else if ((opt == "verbose") || (opt == "V")) {
        _settings.verbose = true;
      } else if ((opt == "length") || (opt == "k")) {
        if ((i + 1) < argc) {
          char *ptr;
          _settings.length = strtoul(argv[++i], &ptr, 10);
          if ((_settings.length == 0) || (*ptr != '\0')) {
            err = 2;
            --i;
          }
        } else {
          err = 1;
        }
      } else if ((opt == "prefix-length") || (opt == "p")) {
        if ((i + 1) < argc) {
          char *ptr;
          _settings.prefix_length = strtoul(argv[++i], &ptr, 10);
          if ((_settings.length == 0) || (*ptr != '\0')) {
            err = 2;
            --i;
          } else {
            // Don't allow a prefix length greater than 13 (since 4^13 > 67M subtrees, which is already enormeous)
            if (_settings.prefix_length > 13) {
              if (_settings.verbose) {
                cerr << "The k-mer prefix length is limited to 13." << endl;
              }
              _settings.prefix_length = 13;
            }
          }
        } else {
          err = 1;
        }
      } else if ((opt == "nb-bins") || (opt == "n")) {
        if ((i + 1) < argc) {
          char *ptr;
          _settings.nb_bins = strtoul(argv[++i], &ptr, 10);
          if ((_settings.nb_bins == 0) || (*ptr != '\0')) {
            err = 2;
            --i;
          }
        } else {
          err = 1;
        }
      } else if ((opt == "queue-size") || (opt == "s")) {
        if ((i + 1) < argc) {
          char *ptr;
          _settings.queue_size = strtoul(argv[++i], &ptr, 10);
          if ((_settings.queue_size == 0) || (*ptr != '\0')) {
            err = 2;
            --i;
          }
        } else {
          err = 1;
        }
      } else if ((opt == "tag") || (opt == "t")) {
        if ((i + 1) < argc) {
          _settings.tag = argv[++i];
        } else {
          err = 1;
        }
      } else if ((opt == "method") || (opt == "m")) {
        if ((i + 1) < argc) {
          _settings.method = argv[++i];
          if (find_if(available_methods.begin(), available_methods.end(), sameMethod(_settings.method)) == available_methods.end()) {
            err = 3;
            --i;
          }
        } else {
          err = 1;
        }
      } else {
        err = -1;
      }

      switch (err) {
      case -1:
        cerr << "Invalid option '" << argv[i] << "'." << endl;
        break;
      case 1:
        cerr << "Option '" << argv[i] << "' expects an argument." << endl;
        break;
      case 2:
        cerr << "Option '" << argv[i] << "' expects a strictly positive value as argument but "
             << "'" << argv[i + 1] << "' was given." << endl;
        break;
      case 3:
        cerr << "Option '" << argv[i] << "' expects a valid k-mer transformation method but '"
             << argv[i + 1] << "' was given." << endl;
        break;
      default:
        break;
      }
      if (err) {
        usage();
      }

    } else {
      // The argument is not an option
      _filenames.push_back(argv[i]);
      if (!tag.empty()) {
        tag += ',';
      }
      tag += argv[i];
    }

  }

  if(_settings.prefix_length >= _settings.length) {
    cerr << "Error: The prefix length (" << _settings.prefix_length << ")"
         << " must be strictly less than the length (" << _settings.length << ")."
         << endl;
    usage();
  }

  _filenames.shrink_to_fit();
  if (_filenames.empty()) {
    cerr << "Error: A file name must be provided." << endl;
    usage();
  }
  if (_settings.tag.empty()) {
    _settings.tag = tag;
  }

}
