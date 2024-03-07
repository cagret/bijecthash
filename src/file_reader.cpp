#include "file_reader.hpp"
#include <cassert>
#include <iostream>

using namespace std;

FileReader::FileReader(const Settings &s, const string &filename): _settings(s) {
  open(filename);
}

void FileReader::_reset() {
  _filename.clear();
  _is.clear();
  _line = 0;
  _column = 0;
  _format = UNDEFINED;
  _current_sequence_description.clear();
  _current_sequence_length = 0;
  _current_kmer.clear();
  _current_kmer.reserve(_settings.length);
  _kmer_id_offset = 0;
  _current_kmer_id = 0;
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
      _reset();
    }
  }
  return (_format != UNDEFINED);
}

void FileReader::close() {
  if (_is.is_open()) {
    _is.close();
  }
  _reset();
}

bool FileReader::_nextKmerFromFasta() {

  assert(_format == FASTA);
  assert(isOpen());

  size_t k = _current_kmer.length();
  if (k >= _settings.length) {
    _current_kmer.erase(0, k -_settings.length + 1);
    k = _current_kmer.length();
    assert(k == _settings.length - 1);
  }

  while (_is && (k < _settings.length)) {

    char c = _is.get();
    bool warn = _settings.verbose;
#ifdef DEBUG
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "Processing char '" << c << "'" << endl;
#endif
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
        if (++_current_sequence_length >= _settings.length) {
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
        if (_settings.verbose) {
          cerr << "Warning: "
               << "file '" << _filename
               << "' (line " << _line << ", column " << _column << "):"
               << " degeneracy symbol '" << c << "'"
               << " found in sequence '" << _current_sequence_description << "'."
               << endl;
        }
        if (++_current_sequence_length >= _settings.length) {
          ++_current_kmer_id;
        }
        break;
      default:
        if (_settings.verbose) {
          cerr << "Warning: "
               << "file '" << _filename
               << "' (line " << _line << ", column " << _column << "):"
               << " unexpected symbol '" << c << "'"
               << " for sequence '" << _current_sequence_description << "'."
               << endl;
        }
        warn = false;
      }
    }
    warn &= (k < _settings.length);
    warn &= (_current_sequence_length >= _settings.length);
    if (warn) {
      cerr << "The k-mer with absolute ID " << getCurrentKmerID()
           << " and relative ID " << getCurrentKmerID(false)
           << " is ignored since it contains some degeneracy symbols."
           << endl;
    }
  }

  if (k != _settings.length) {
    _current_sequence_description.clear();
    _current_sequence_length = 0;
    _current_kmer.clear();
    _kmer_id_offset = 0;
    _current_kmer_id = 0;
  }
#ifdef DEBUG
  else {
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "current sequence description: " << getCurrentSequenceDescription() << endl;
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "current kmer: " << getCurrentKmer() << " (abs. ID: " << getCurrentKmerID() << ", rel. ID: " << getCurrentKmerID(false) << ")" << endl;
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "k = " << k << " and _settings.length = " << _settings.length << endl;
  }
#endif

  return (k == _settings.length);

}

bool FileReader::_nextKmerFromFastq() {

  assert(_format == FASTQ);
  assert(isOpen());

  size_t k = _current_kmer.length();
  if (k >= _settings.length) {
    _current_kmer.erase(0, k -_settings.length + 1);
    k = _current_kmer.length();
    assert(k == _settings.length - 1);
  }
  int state = !_current_sequence_description.empty();

  size_t nb = 0;

  while (_is && (k < _settings.length)) {

    char c = _is.get();
    bool warn = _settings.verbose;
#ifdef DEBUG
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "Processing char '" << c << "'" << endl;
#endif
    ++_column;

    switch (state) {

    case 0: {
#ifdef DEBUG
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "State 0 (expecting sequence header)" << endl;
#endif
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
#ifdef DEBUG
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "State 1 (processing nucl. sequence)" << endl;
#endif
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
        if (++_current_sequence_length >= _settings.length) {
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
        if (_settings.verbose) {
          cerr << "Warning: "
               << "file '" << _filename
               << "' (line " << _line << ", column " << _column << "):"
               << " degeneracy symbol '" << c << "'"
               << " found in sequence '" << _current_sequence_description << "'."
               << endl;
        }
        if (++_current_sequence_length >= _settings.length) {
          ++_current_kmer_id;
        }
        break;
      default:
        if (_settings.verbose) {
          cerr << "Warning: "
               << "file '" << _filename
               << "' (line " << _line << ", column " << _column << "):"
               << " unexpected symbol '" << c << "'"
               << " for sequence '" << _current_sequence_description << "'."
               << endl;
        }
        warn = false;
      }
      break;
    }

    case 2: {
#ifdef DEBUG
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "State 2 (sequence separator)" << endl;
#endif
      assert(c == '+');
      assert(_column == 1);
      string desc;
      getline(_is, desc);
      _column += desc.length();
      if (_settings.verbose && !(desc.empty() || (desc == _current_sequence_description))) {
        cerr << "Warning: "
             << "file '" << _filename
             << "' (line " << _line << ", column " << _column << "):"
             << " repeated sequence description '" << desc << "'"
             << " doesn't match with '" << _current_sequence_description << "'."
             << endl;
      }
      state = 3;
      break;
    }

    case 3: {
#ifdef DEBUG
      cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
           << "State 3 (processing quality for the remaining " << nb << "symbols)" << endl;
#endif
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
      } else if (_settings.verbose && (c != ' ') && (c != -1)) {
        cerr << "Warning: "
             << "file '" << _filename
             << "' (line " << _line << ", column " << _column << "):"
             << " unexpected symbol with ASCII code " << hex << (int) c << dec
             << " for sequence '" << _current_sequence_description << "'."
             << endl;
      }
      break;
    }

    default:
      cerr << "BUG: this situation should never occur!!! "
           << "     "
           << "file '" << _filename
           << "' (line " << _line << ", column " << _column << "):"
           << " character '" << c << "'"
           << " for sequence '" << _current_sequence_description << "'."
           << endl;
      terminate();
    }

    warn &= (state == 1);
    warn &= (k < _settings.length);
    warn &= (_current_sequence_length >= _settings.length);
    if (warn) {
      cerr << "The k-mer with absolute ID " << getCurrentKmerID()
           << " and relative ID " << getCurrentKmerID(false)
           << " is ignored since it contains some degeneracy symbols."
           << endl;
    }
  }

  if (k != _settings.length) {
    _current_sequence_description.clear();
    _current_sequence_length = 0;
    _current_kmer.clear();
    _kmer_id_offset = 0;
    _current_kmer_id = 0;
  }
#ifdef DEBUG
  else {
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "current sequence description: " << getCurrentSequenceDescription() << endl;
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "current kmer: " << getCurrentKmer() << " (abs. ID: " << getCurrentKmerID() << ", rel. ID: " << getCurrentKmerID(false) << ")" << endl;
    cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
         << "k = " << k << " and _settings.length = " << _settings.length << endl;
  }
#endif

  return (k == _settings.length);
}
