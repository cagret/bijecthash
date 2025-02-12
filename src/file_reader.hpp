/******************************************************************************
*                                                                             *
*  Copyright © 2024-2025 -- LIRMM/CNRS/UM                                     *
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

#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <string>
#include <fstream>

namespace bijecthash {

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

    /**
     * The length of k-mers to extract from the file (*i.e.*, the value of \f$k\f$).
     */
    size_t _k;

    /**
     * The currently associated filename.
     */
    std::string _filename;

    /**
     * The input stream associated to the filename (if any).
     */
    std::ifstream _is;

    /**
     * The number of the current line in the file (starting at 1, if no
     * file is open then 0).
     */
    size_t _line;

    /**
     * The number of the current column in the file (starting at 1, if
     * no file is open then 0).
     */
    size_t _column;

    /**
     * The file format.
     */
    Format _format;

    /**
     * The current sequence description (empty if no k-mer is
     * available).
     */
    std::string _current_sequence_description;

    /**
     * The current sequence length (until the end of the current k-mer)
     */
    size_t _current_sequence_length;

    /**
     * The current k-mer.
     */
    std::string _current_kmer;

    /**
     * The offset needed to compute the relative ID of the current k-mer
     * using its absolute ID (this is the absolute ID of the last k-mer
     * of the previous sequence).
     */
    size_t _kmer_id_offset;

    /**
     * The absolute ID of the current k-mer.
     */
    size_t _current_kmer_id;

    /**
     * Load the next available k-mer from the current file assuming it
     * is Fasta formatted.
     *
     * \return Returns true if a k-mer has been loaded (if the file is
     * opened and has a k-mer) and false otherwise (if no file is
     * opened, if the file is opened but is not fasta formatted or if
     * the file is opened, fasta formatted but contains no more k-mer).
     */
    bool _nextKmerFromFasta();

    /**
     * Load the next available k-mer from the current file assuming it
     * is Fastq formatted.
     *
     * \return Returns true if a k-mer has been loaded (if the file is
     * opened and has a k-mer) and false otherwise (if no file is
     * opened, if the file is opened but is not fastq formatted or if
     * the file is opened, fastq formatted but contains no more k-mer).
     */
    bool _nextKmerFromFastq();

  public:

    /**
     * Constructs a new FileReader instance (and internally call the open() method)
     *
     * \param kmer_length The length of k-mer to extract (*i.e.*, the
     * value of \f$k\f$).
     *
     * \param filename The name of the file to parse (see open() method).
     *
     * \param verbose Don't emit warnings when is set to \c
     * false. This can be changed at any time (see verbose
     * attribute).
     */
    FileReader(const size_t kmer_length, const std::string &filename = "", bool verbose = true);
    /**
     * The verbosity status of the reader.
     */
    bool verbose;

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
      return _filename;
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
      return _k;
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
     * opened (or no k-mer has been read yet).
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

}

#endif // FILEREADER_HPP
