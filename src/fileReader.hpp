#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <string>
#include <fstream>
#include <settings.hpp>

/**
 * File reader to extract k-mers from fasta/fastq files
 */
class FileReader {

public:

  /**
   * The file format
   */
  enum Format {
        FASTA,     /**< File is fasta formatted */
        FASTQ,     /**< File is fastq formatted */
        UNDEFINED, /**< File format is not detected */
  };

private:

  Settings _settings;
  std::ifstream _is;
  size_t _line;
  size_t _column;
  Format _format;

  std::string _current_sequence_description;
  size_t _current_sequence_length;
  std::string _current_kmer;
  size_t _kmer_id_offset;
  size_t _current_kmer_id;

  void _reset();
  bool _nextKmerFromFasta();
  bool _nextKmerFromFastq();

public:

  /**
   * Constructs a new FileReader instance (and internally call the open() method)
   *
   * \param k The k-mer length used to parse the file.
   *
   * \param filename The name of the file to parse.
   */
  FileReader(const Settings &s);

  /**
   * Open the given filename.
   *
   * This method closes the opened file if any.
   *
   * \param filename The name of the file to parse.
   *
   * \return This returns true if and only if the file exists and is
   * correctly opened (format is correctly detected).
   */
  bool open(const std::string &filename);

  /**
   * Close the currently opened file.
   */
  void close();

  /**
   * Check whether a file is associated to this reader and open.
   *
   * \return Returns true if the reader is associated to some file.
   */
  inline bool isOpen() const {
    return _is.is_open();
  }

  /**
   * Retrieve the name of the currently opened file.
   *
   * \return Returns the name of the currently opened file or an empty
   * string if no file is opened.
   */
  inline std::string getFilename() const {
    return _settings.filename;
  }

  /**
   * Get the currently processed line number.
   *
   * \return Returns the current line number or 0 if no file is
   * opened.
   */
  inline size_t getLineNumber() const {
    return _line;
  }

  /**
   * Get the currently processed column number.
   *
   * \return Returns the current column number or 0 if no file is
   * opened.
   */
  inline size_t getColumnNumber() const {
    return _column;
  }

  /**
   * Get the length of extracted k-mers.
   *
   * \return Returns the length of the extracted k-mers for this reader.
   */
  inline size_t k() const {
    return _settings.length;
  }

  /**
   * Get the file format.
   *
   * \return This returns the detected file format. If file is not open, then UNDEFINED is returned
   */
  inline Format getFormat() const {
    return _format;
  }

  /**
   * Get the description header of the currently processed sequence.
   *
   * \return Returns the header description of the currently processed
   * sequence or the empty string if no sequence is being processed.
   */
  inline std::string getCurrentSequenceDescription() const {
    return _current_sequence_description;
  }

  /**
   * Get the current k-mer of the currently processed sequence.
   *
   * \return Returns the current k-mer of the currently processed
   * sequence or the empty string if no sequence is being processed.
   */
  inline std::string getCurrentKmer() const {
    return _current_kmer;
  }

  /**
   * Get the current k-mer ID.
   *
   * \param absolute If true, returns the absolute ID (among all
   * sequences of the file [default]). If false, returns the relative
   * ID of the k-mer (the first k-mer of some sequence has relative ID
   * 1).
   *
   * \return Returns the current k-mer ID starting from 1 or 0 if no file is
   * opened.
   */
  inline size_t getCurrentKmerID(bool absolute = true) const {
    return (absolute
            ? _current_kmer_id
            : _current_kmer_id - _kmer_id_offset);
  }

  /**
   * Compute the next available k-mer.
   *
   * \return Returns true if some k-mer is available and false
   * otherwise.
   */
  inline bool nextKmer() {
    switch (_format) {
    case FASTA: return _nextKmerFromFasta();
    case FASTQ: return _nextKmerFromFastq();
    default: return false;
    }
  }

};

#endif // FILEREADER_HPP

