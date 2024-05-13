/******************************************************************************
*                                                                             *
*  Copyright © 2024      -- LIRMM/CNRS/UM                                     *
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

#include "program_options.hpp"

#include "common.hpp"

#include <algorithm> // find_if()
#include <libgen.h> // basename()
#include <iostream>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

const vector<string> ProgramOptions::available_methods =
  {
   "identity",
   "canonical",
   "random_bits",
   "random_nucl",
   "cyclic",
   "lyndon",
   "bwt",
   "inverse",
   "zigzag",
   "inthash",
   "minimizer",
   "GAB[=a,b]",
   "composition=(t2*t1)"
  };

const Settings ProgramOptions::default_settings = Settings(21 /* length */,
                                                           10 /* prefix_length */,
                                                           available_methods[0] /* method */);

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
       << " -m | --method <method>" << "\t\t" << "The k-mer transformation method to use (default: " << default_settings.getMethod() << ")." << endl
       << endl
       << "Available methods are:" << endl;
  for (auto m: available_methods) {
    cerr << "  - " << m << endl;
  }
  cerr << "Notice none of the method can handle k-mers having a suffix size (i.e. the length minus the prefix length) greater than 32." << endl
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
  string method;
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
          method = argv[++i];
          if (find_if(available_methods.begin(), available_methods.end(), sameMethod(method)) == available_methods.end()) {
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

  if (!method.empty()) {
    _settings.setMethod(method);
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

END_BIJECTHASH_NAMESPACE
