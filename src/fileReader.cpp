#include "fileReader.hpp"
#include <cassert>
#include <iostream>

using namespace std;

FileReader::FileReader(size_t k, const string &filename): _k(k) {
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
  _current_kmer.reserve(_k);
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
  if (k >= _k) {
    _current_kmer.erase(0, k -_k + 1);
    k = _current_kmer.length();    
    assert(k == _k - 1);
  }
  while (_is && (k < _k)) {
    char c = _is.get();
    bool warn = true;
    // cerr << "Processing char '" << c << "'" << endl;
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
        cerr << "Warning: "
             << "file '" << _filename
             << "' (line " << _line << ", column " << _column << "):"
             << " degeneracy symbol '" << c << "'"
             << " found in sequence '" << _current_sequence_description << "'."
             << endl;
        if (++_current_sequence_length >= _k) {
          ++_current_kmer_id;
        }
        break;
      default:
        cerr << "Warning: "
             << "file '" << _filename
             << "' (line " << _line << ", column " << _column << "):"
             << " unexpected symbol '" << c << "'"
             << " for sequence '" << _current_sequence_description << "'."
             << endl;
        warn = false;
      }
    }
    warn &= (k < _k);
    warn &= (_current_sequence_length >= _k);
    if (warn) {
      cerr << "The k-mer with absolute ID " << getCurrentKmerID()
           << " and relative ID " << getCurrentKmerID(false)
           << " is ignored since it contains some degeneracy symbols."
           << endl;
    }
  }
  return (k == _k);
}

bool FileReader::_nextKmerFromFastq() {
  assert(_format == FASTQ);
  return false;
}
