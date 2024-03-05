#include "settings.hpp"

#include <libgen.h>
#include <algorithm>

using namespace std;

const vector<string> Settings::available_methods = {
                                                    "identity",
                                                    "random",
                                                    "cyclic",
                                                    "inverse",
                                                    "zigzag",
                                                    "inthash",
                                                    "GAB"
};

const Settings Settings::default_settings;


Settings::Settings():
  program_name("<progname>"),
  filename(""),
  length(21),
  prefix_length(10),
  method(available_methods[0]),
  nb_bins(100),
  verbose(true)
{
}

void Settings::usage() const {
  cerr << endl
       << "Usage: " << program_name << " [options] [-[-]] <filename>" << endl
       << "Where available options are:" << endl
       << " -h | --help" << "\t\t\t" << "Show this message then exit." << endl
       << " -  | --" << "\t\t\t" << "Can be use to process file whose name starts by a dash." << endl
       << " -q | --quiet" << "\t\t\t" << "Don't print warnings and informations about ignored k-mers." << endl
       << " -V | --verbose" << "\t\t\t" << "Print warnings and informations about ignored k-mers." << endl
       << " -k | --length <value>" << "\t\t" << "Set the k-mer length (default: " << default_settings.length << ")." << endl
       << " -p | --prefix-length <value>" << "\t" << "Set the prefix length of k-mers (default: " << default_settings.prefix_length << ")." << endl
       << " -n | --nb-bins <value>" << "\t" << "Number of bins for the computed statistics (default: " << default_settings.nb_bins << ")." << endl
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

Settings::Settings(int argc, char** argv):
  program_name(default_settings.program_name),
  filename(default_settings.filename),
  length(default_settings.length),
  prefix_length(default_settings.prefix_length),
  method(default_settings.method),
  nb_bins(default_settings.nb_bins),
  verbose(default_settings.verbose)
{

  bool options_accepted = true;
  int i = 0;

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
        verbose = false;
      } else if ((opt == "verbose") || (opt == "V")) {
        verbose = true;
      } else if ((opt == "length") || (opt == "k")) {
        if ((i + 1) < argc) {
          char *ptr;
          length = strtoul(argv[++i], &ptr, 10);
          if ((length == 0) || (*ptr != '\0')) {
            err = 2;
            --i;
          }
        } else {
          err = 1;
        }
      } else if ((opt == "prefix-length") || (opt == "p")) {
        if ((i + 1) < argc) {
          char *ptr;
          prefix_length = strtoul(argv[++i], &ptr, 10);
          if ((length == 0) || (*ptr != '\0')) {
            err = 2;
            --i;
          } else {
            // Don't allow a prefix length greater than 13 (since 4^13 > 67M subtrees, which is already enormeous)
            if (prefix_length > 13) {
              if (verbose) {
                cerr << "The k-mer prefix length is limited to 13." << endl;
              }
              prefix_length = 13;
            }
          }
        } else {
          err = 1;
        }
      } else if ((opt == "nb-bins") || (opt == "n")) {
        if ((i + 1) < argc) {
          char *ptr;
          nb_bins = strtoul(argv[++i], &ptr, 10);
          if ((nb_bins == 0) || (*ptr != '\0')) {
            err = 2;
            --i;
          }
        } else {
          err = 1;
        }
      } else if ((opt == "method") || (opt == "m")) {
        if ((i + 1) < argc) {
          method = argv[++i];
          if (find(available_methods.begin(), available_methods.end(), method) == available_methods.end()) {
            err = 3;
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

      if (!filename.empty()) {
        cerr << "Error: don't know what to do with argument '" << argv[i] << "'" << endl;
        usage();
      }
      filename = argv[i];
    }

  }

  if (filename.empty()) {
    cerr << "Error: A file name must be provided." << endl;
    usage();
  }

}


ostream &operator<<(ostream &os, const Settings &s) {
  os << "Settings:" << endl
     << "- filename: " << s.filename << endl
     << "- length: " << s.length << endl
     << "- prefix_length: " << s.prefix_length << endl
     << "- method: " << s.method << endl
     << "- nb_bins: " << s.nb_bins << endl
     << "- verbose: " << s.verbose << endl
     << endl;
  return os;
}

