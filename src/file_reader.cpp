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

#include "file_reader.hpp"

#include "common.hpp"
#include "locker.hpp"

#include <iostream>

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

FileReader::FileReader(size_t kmer_length, const string &filename, bool verbose):
  _k(kmer_length), _filename(), verbose(verbose)
{
  open(filename);
}

bool FileReader::open(const string &filename) {
  close();
  if (!filename.empty()) {
    _is.open(filename);
    if (_is.is_open()) {
      _filename = filename;
      _line = 1;
      char c = _is.peek();
      switch (c) {
      case '>': _format = FASTA; break;
      case '@': _format = FASTQ; break;
      default: close();
      }
    } else {
      close();
    }
  }
  return (_format != UNDEFINED);
}

void FileReader::close() {
  _filename.clear();
  if (_is.is_open()) {
    _is.close();
  }
  _is.clear();
  _line = 0;
  _column = 0;
  _format = UNDEFINED;
  _current_sequence_description.clear();
  _current_sequence_length = 0;
  _current_kmer.clear();
  _current_kmer.reserve(_k);
  _kmer_id_offset = 0;
  _current_kmer_id = 0;
}

bool FileReader::_nextKmerFromFasta() {

  assert(_format == FASTA);
  assert(isOpen());

  size_t k = _current_kmer.length();
  if (k >= _k) {
    _current_kmer.erase(0, k - _k + 1);
    k = _current_kmer.length();
    assert(k == _k - 1);
  }

  while (_is && (k < _k)) {

    char c = _is.get();
    bool warn = verbose;
    DEBUG_MSG("Processing char '" << c << "'");
    ++_column;
    if (_current_sequence_description.empty()) {
      // Expects a new sequence description header
      assert(c == '>');
      assert(_column == 1);
      getline(_is, _current_sequence_description);
      _kmer_id_offset = _current_kmer_id;
      _current_kmer.clear();
      k = 0;
      _current_sequence_length = 0;
      ++_line;
      _column = 0;
    } else {
      switch (c) {
      case '\n':
        ++_line;
        _column = 0;
        if (_is.peek() == '>') {
          _current_sequence_description.clear();
        }
        /* FALLTHROUGH */
      case ' ':
      case '.':
      case '-':
      case -1:
        warn = false;
        break;
      case 'a':
      case 'A':
      case 'c':
      case 'C':
      case 'g':
      case 'G':
      case 't':
      case 'T':
        _current_kmer += toupper(c);
        ++k;
        if (++_current_sequence_length >= _k) {
          ++_current_kmer_id;
        }
        break;
      case 'w':
      case 'W':
      case 's':
      case 'S':
      case 'p':
      case 'P':
      case 'y':
      case 'Y':
      case 'k':
      case 'K':
      case 'm':
      case 'M':
      case 'b':
      case 'B':
      case 'd':
      case 'D':
      case 'h':
      case 'H':
      case 'v':
      case 'V':
      case 'n':
      case 'N':
        _current_kmer.clear();
        k = 0;
        if (verbose) {
          io_mutex.lock();
          cerr << "Warning: "
               << "file '" << _filename
               << "' (line " << _line << ", column " << _column << "):"
               << " degeneracy symbol '" << c << "'"
               << " found in sequence '" << _current_sequence_description << "'."
               << endl;
          io_mutex.unlock();
        }
        if (++_current_sequence_length >= _k) {
          ++_current_kmer_id;
        }
        break;
      default:
        if (verbose) {
          io_mutex.lock();
          cerr << "Warning: "
               << "file '" << _filename
               << "' (line " << _line << ", column " << _column << "):"
               << " unexpected symbol '" << c << "'"
               << " for sequence '" << _current_sequence_description << "'."
               << endl;
          io_mutex.unlock();
        }
        warn = false;
      }
    }
    warn &= (k < _k);
    warn &= (_current_sequence_length >= _k);
    if (warn) {
      io_mutex.lock();
      cerr << "The k-mer with absolute ID " << getCurrentKmerID()
           << " and relative ID " << getCurrentKmerID(false)
           << " is ignored since it contains some degeneracy symbols."
           << endl;
      io_mutex.unlock();
    }
  }

  if (k != _k) {
    _current_sequence_description.clear();
    _current_sequence_length = 0;
    _current_kmer.clear();
    _kmer_id_offset = 0;
    _current_kmer_id = 0;
  } else {
    DEBUG_MSG("current sequence description: " << getCurrentSequenceDescription());
    DEBUG_MSG("current kmer: " << getCurrentKmer() << " (abs. ID: " << getCurrentKmerID() << ", rel. ID: " << getCurrentKmerID(false) << ")");
    DEBUG_MSG("k = " << k << " and _k = " << _k);
  }

  return (k == _k);

}

bool FileReader::_nextKmerFromFastq() {

  assert(_format == FASTQ);
  assert(isOpen());

  size_t k = _current_kmer.length();
  if (k >= _k) {
    _current_kmer.erase(0, k -_k + 1);
    k = _current_kmer.length();
    assert(k == _k - 1);
  }
  int state = !_current_sequence_description.empty();

  size_t nb = 0;

  while (_is && (k < _k)) {

    char c = _is.get();
    bool warn = verbose;
    DEBUG_MSG("Processing char '" << c << "'");
    ++_column;

    switch (state) {

    case 0: {
      DEBUG_MSG("State 0 (expecting sequence header)");
      assert(_current_sequence_description.empty());
      // Expects a new sequence description header
      assert(c == '@');
      assert(_column == 1);
      getline(_is, _current_sequence_description);
      _kmer_id_offset = _current_kmer_id;
      _current_kmer.clear();
      k = 0;
      _current_sequence_length = 0;
      ++_line;
      _column = 0;
      state = 1;
      break;
    }

    case 1: {
      DEBUG_MSG("State 1 (processing nucl. sequence)");
      switch (c) {
      case '\n':
        ++_line;
        _column = 0;
        if (_is.peek() == '+') {
          nb = _current_sequence_length;
          state = 2;
        }
        /* FALLTHROUGH */
      case ' ':
      case '.':
      case '-':
        warn = false;
        break;
      case 'a':
      case 'A':
      case 'c':
      case 'C':
      case 'g':
      case 'G':
      case 't':
      case 'T':
        _current_kmer += toupper(c);
        ++k;
        if (++_current_sequence_length >= _k) {
          ++_current_kmer_id;
        }
        break;
      case 'w':
      case 'W':
      case 's':
      case 'S':
      case 'p':
      case 'P':
      case 'y':
      case 'Y':
      case 'k':
      case 'K':
      case 'm':
      case 'M':
      case 'b':
      case 'B':
      case 'd':
      case 'D':
      case 'h':
      case 'H':
      case 'v':
      case 'V':
      case 'n':
      case 'N':
        _current_kmer.clear();
        k = 0;
        if (verbose) {
          io_mutex.lock();
          cerr << "Warning: "
               << "file '" << _filename
               << "' (line " << _line << ", column " << _column << "):"
               << " degeneracy symbol '" << c << "'"
               << " found in sequence '" << _current_sequence_description << "'."
               << endl;
          io_mutex.unlock();
        }
        if (++_current_sequence_length >= _k) {
          ++_current_kmer_id;
        }
        break;
      default:
        if (verbose) {
          io_mutex.lock();
          cerr << "Warning: "
               << "file '" << _filename
               << "' (line " << _line << ", column " << _column << "):"
               << " unexpected symbol '" << c << "'"
               << " for sequence '" << _current_sequence_description << "'."
               << endl;
          io_mutex.unlock();
        }
        warn = false;
      }
      break;
    }

    case 2: {
      DEBUG_MSG("State 2 (sequence separator)");
      assert(c == '+');
      assert(_column == 1);
      string desc;
      getline(_is, desc);
      _column += desc.length();
      if (verbose && !(desc.empty() || (desc == _current_sequence_description))) {
        io_mutex.lock();
        cerr << "Warning: "
             << "file '" << _filename
             << "' (line " << _line << ", column " << _column << "):"
             << " repeated sequence description '" << desc << "'"
             << " doesn't match with '" << _current_sequence_description << "'."
             << endl;
        io_mutex.unlock();
      }
      state = 3;
      break;
    }

    case 3: {
      DEBUG_MSG("State 3 (processing quality for the remaining " << nb << "symbols)");
      if (c == '\n') {
        ++_line;
        _column = 0;
        if ((nb == 0) && (_is.peek() == '@')) {
          state = 0;
          _current_sequence_description.clear();
        }
      } else if (c > ' ') {
        assert(nb);
          --nb;
      } else if (verbose && (c != ' ') && (c != -1)) {
        io_mutex.lock();
        cerr << "Warning: "
             << "file '" << _filename
             << "' (line " << _line << ", column " << _column << "):"
             << " unexpected symbol with ASCII code " << hex << (int) c << dec
             << " for sequence '" << _current_sequence_description << "'."
             << endl;
        io_mutex.unlock();
      }
      break;
    }

    default:
      io_mutex.lock();
      cerr << "BUG: this situation should never occur!!! "
           << "     "
           << "file '" << _filename
           << "' (line " << _line << ", column " << _column << "):"
           << " character '" << c << "'"
           << " for sequence '" << _current_sequence_description << "'."
           << endl;
      exit(1);
      io_mutex.unlock();
    }

    warn &= (state == 1);
    warn &= (k < _k);
    warn &= (_current_sequence_length >= _k);
    if (warn) {
      io_mutex.lock();
      cerr << "The k-mer with absolute ID " << getCurrentKmerID()
           << " and relative ID " << getCurrentKmerID(false)
           << " is ignored since it contains some degeneracy symbols."
           << endl;
      io_mutex.unlock();
    }
  }

  if (k != _k) {
    _current_sequence_description.clear();
    _current_sequence_length = 0;
    _current_kmer.clear();
    _kmer_id_offset = 0;
    _current_kmer_id = 0;
  } else {
    DEBUG_MSG("current sequence description: " << getCurrentSequenceDescription());
    DEBUG_MSG("current kmer: " << getCurrentKmer() << " (abs. ID: " << getCurrentKmerID() << ", rel. ID: " << getCurrentKmerID(false) << ")");
    DEBUG_MSG("k = " << k << " and _k = " << _k);
  }

  return (k == _k);
}

END_BIJECTHASH_NAMESPACE
