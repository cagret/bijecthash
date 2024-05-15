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

#ifdef NDEBUG
#  undef NDEBUG
#endif
#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>

#include "file_reader.hpp"

using namespace std;
using namespace bijecthash;

ostream &operator<<(ostream &os, FileReader::Format f) {
  switch (f) {
  case FileReader::FASTA:
    os << "FASTA";
    break;
  case FileReader::FASTQ:
    os << "FASTQ";
    break;
  default:
    os << "UNDEFINED";
  }
  return os;
}

struct FileReaderTester {
  FileReader &reader;
  bool expected_open_status;
  string expected_filename;
  size_t expected_line_number;
  size_t expected_column_number;
  FileReader::Format expected_format;
  string expected_sequence_description;
  const size_t expected_kmer_length;
  size_t expected_kmer_id;
  string expected_kmer;

  FileReaderTester(FileReader &r):
    reader(r),
    expected_open_status(false),
    expected_filename(),
    expected_line_number(0),
    expected_column_number(0),
    expected_format(FileReader::UNDEFINED),
    expected_sequence_description(),
    expected_kmer_length(r.k()),
    expected_kmer_id(0),
    expected_kmer() {}

  void test() const {

    cout << std::boolalpha;
    cout << "Reader file status: " << endl
         << "  - open: " << reader.isOpen() << " "
         << "(expecting " << expected_open_status << ")" << endl;
    assert(reader.isOpen() == expected_open_status);

    cout << "  - filename: '" << reader.getFilename() << "' "
         << "(expecting '" << expected_filename << "')" << endl;
    assert(reader.getFilename() == expected_filename);

    cout << "  - line " << reader.getLineNumber() << " "
         << "(expecting " << expected_line_number << ")" << endl;
    assert(reader.getLineNumber() == expected_line_number);

    cout << "  - column " << reader.getColumnNumber() << " "
         << "(expecting " << expected_column_number << ")" << endl;
    assert(reader.getColumnNumber() == expected_column_number);

    cout << "  - format " << reader.getFormat() << " "
         << "(expecting " << expected_format << ")" << endl;
    assert(reader.getFormat() == expected_format);

    cout << "  - current sequence '" << reader.getCurrentSequenceDescription() << "' "
         << "(expecting '" << expected_sequence_description << "')" << endl;
    assert(reader.getCurrentSequenceDescription() == expected_sequence_description);

    cout << "Current k-mer: " << endl;
    cout << "  - length: " << reader.k() << " "
         << "(expecting " << expected_kmer_length << ")" << endl;
    assert(reader.k() == expected_kmer_length);

    cout << "  - ID: " << reader.getCurrentKmerID() << " "
         << "(expecting " << expected_kmer_id << ")" << endl;
    assert(reader.getCurrentKmerID() == expected_kmer_id);

    cout << "  - value: '" << reader.getCurrentKmer() << "' "
         << "(expecting '" << expected_kmer << "')" << endl;
    assert(reader.getCurrentKmer() == expected_kmer);

    cout << endl;

  }

  void next_kmer(bool expected_result = true) {
    bool next_kmer_result = reader.nextKmer();
    cout << "Next k-mer computation returns " << next_kmer_result << " "
         << "(expecting " << expected_result << ")" << endl;
    assert(next_kmer_result == expected_result);
    if (next_kmer_result) {
      ++expected_kmer_id;
    } else {
      expected_kmer_id = 0;
    }
    cout << endl;
  }

  void skip_kmers(size_t id) {
    cout << "Skipping k-mers until ID " << id << " => ";
    bool ok = reader.isOpen() && !reader.getCurrentKmer().empty();
    while (ok && (reader.getCurrentKmerID() < id)) {
      ok = reader.nextKmer();
    }
    if (ok) {
      cout << "current k-mer exists and has ID " << reader.getCurrentKmerID() << " "
           << "(expecting " << id << ")" << endl;
      assert(reader.getCurrentKmerID() == id);
      expected_kmer_id = id;
    } else {
      cout << "End of file encountered before parsing k-mer with ID " << id << endl;
      assert(false);
    }
    cout << endl;
  }

};

int main() {

  FileReader reader(5);
  FileReaderTester t(reader);


  //////////////////////////////
  // Tests with invalid files //
  //////////////////////////////

  cout << "*** File Reader with an empty filename ***" << endl << endl;
  t.test();
  t.next_kmer(false);

  string fname = "inexistant.filename";
  cout << "*** File Reader with an inexistant filename ('" << fname << "') ***" << endl << endl;
  reader.open(fname);
  t.test();
  t.next_kmer(false);

  fname = "../Makefile.am";
  cout << "*** File Reader with an invalid file ('" << fname << "') ***" << endl << endl;
  reader.open(fname);
  t.test();
  t.next_kmer(false);


  ////////////////////////////
  // Tests with example1.fa //
  ////////////////////////////

  fname = "../resources/example1.fa";
  cout << "*** File Reader with a valid FASTA file ('" << fname << "') ***" << endl << endl;
  reader.open(fname);

  t.expected_open_status = true;
  t.expected_filename = fname;
  t.expected_line_number = 1;
  t.expected_column_number = 0;
  t.expected_format = FileReader::FASTA;
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " My first sequence which has 113 nucl.";
  t.expected_line_number = 2;
  t.expected_column_number = 5;
  t.expected_kmer = "ACGCT";
  t.test();

  t.next_kmer();
  t.expected_column_number = 6;
  t.expected_kmer = "CGCTG";
  t.test();

  t.skip_kmers(61);
  t.expected_line_number = 3;
  t.expected_column_number = 16;
  t.expected_kmer = "GCATA";
  t.test();

  t.skip_kmers(109);
  t.expected_line_number = 4;
  t.expected_column_number = 14;
  t.expected_kmer = "ACGAC";
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " My second sequence which has 70 x 50 = 3500 nucl.";
  t.expected_line_number = 7;
  t.expected_column_number = 5;
  t.expected_kmer = "TGTCG";
  t.test();

  t.skip_kmers(3605);
  t.expected_line_number = 56;
  t.expected_column_number = 76;
  t.expected_kmer = "AGATC";
  t.test();

  t.next_kmer(false);
  t.expected_line_number = 57;
  t.expected_column_number = 0;
  t.expected_sequence_description.clear();
  t.expected_kmer.clear();
  t.test();

  cout << "End of file '" << fname << "'" << endl << endl;


  ////////////////////////////
  // Tests with example2.fa //
  ////////////////////////////

  fname = "../resources/example2.fa";
  cout << "*** File Reader with a valid FASTA file but strange content ('" << fname << "') ***" << endl << endl;
  reader.open(fname);

  t.expected_open_status = true;
  t.expected_filename = fname;
  t.expected_line_number = 1;
  t.expected_column_number = 0;
  t.expected_format = FileReader::FASTA;
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " A first strange sequence with ignored symbols and gaps";
  t.expected_line_number = 3;
  t.expected_column_number = 7;
  t.expected_kmer = "ACGCT";
  t.test();

  t.skip_kmers(109);
  t.expected_line_number = 7;
  t.expected_column_number = 21;
  t.expected_kmer = "ACGAC";
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " A second sequence with ignored symbols (at positions 11 and 21) as well as degeneracy symbols (at even pos positions from 2 to 26)";
  t.expected_line_number = 10;
  t.expected_column_number = 34;
  // The first "valid" k-mer starts at the 'C' nucleotide at line 10,
  // column 27. Thus the ending column is 34. Since the 11th ('X') and
  // 21th ('Z') symbols are invalid (thus ignored), this k-mer is the
  // 23th of the sequence and then has ID 109+23 = 132.
  t.expected_kmer_id = 132;
  t.expected_kmer = "CACAT";
  t.test();

  // Going to the last k-mer of this second sequence.
  t.skip_kmers(142);
  t.expected_line_number = 10;
  t.expected_column_number = 44;
  t.expected_kmer = "ATCGA";
  t.test();

  // The next k-mer is located in the TuX bubble
  t.next_kmer();
  t.expected_sequence_description = " A third sequence with very strange content...";
  t.expected_line_number = 15;
  t.expected_column_number = 7;
  t.expected_kmer = "ACGTC";
  t.test();

  // The last k-mer located in the TuX bubble (AcUGT) has ID 142 + 53 = 195.
  // This is the last k-mer of the file since the other symbols are in
  // comments and the last sequence is too short.
  t.skip_kmers(196);
  t.expected_line_number = 16;
  t.expected_column_number = 34;
  t.expected_kmer = "ACUGT";
  t.test();

  t.next_kmer(false);
  t.expected_line_number = 29;
  t.expected_column_number = 4;
  t.expected_sequence_description.clear();
  t.expected_kmer.clear();
  t.test();

  cout << "End of file '" << fname << "'" << endl << endl;


  ////////////////////////////
  // Tests with example1.fq //
  ////////////////////////////

  fname = "../resources/example1.fq";
  cout << "*** File Reader with a valid FASTQ file ('" << fname << "') ***" << endl << endl;
  reader.open(fname);

  t.expected_open_status = true;
  t.expected_filename = fname;
  t.expected_line_number = 1;
  t.expected_column_number = 0;
  t.expected_format = FileReader::FASTQ;
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " read 1 with 40 nucl. and a single '+' as separator";
  t.expected_line_number = 2;
  t.expected_column_number = 5;
  t.expected_kmer = "ACGTT";
  t.test();

  t.skip_kmers(36);
  t.expected_column_number = 40;
  t.expected_kmer = "CGACT";
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " read 2 with 40 nucl. and a '+' followed by this repeated header as separator";
  t.expected_line_number = 7;
  t.expected_column_number = 5;
  t.expected_kmer = "ACGTT";
  t.test();

  t.skip_kmers(72);
  t.expected_column_number = 40;
  t.expected_kmer = "CGACT";
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " read 3 with 40 nucl. and a single '+' as separator but with spaces in both the sequence and the quality string";
  t.expected_line_number = 12;
  t.expected_column_number = 5;
  t.expected_kmer = "ACGTT";
  t.test();

  t.skip_kmers(108);
  t.expected_column_number = 43;
  t.expected_kmer = "CGACT";
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " read 4 with 40 nucl. and a single '+' as separator but with newlines and spaces in both the sequence and the quality string (but no quality line starting by an '@')";
  t.expected_line_number = 17;
  t.expected_column_number = 5;
  t.expected_kmer = "ACGTT";
  t.test();

  t.skip_kmers(144);
  t.expected_line_number = 23;
  t.expected_column_number = 14;
  t.expected_kmer = "CGACT";
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " read 5 with 40 nucl. and a single '+' as separator but with newlines and spaces in both the sequence and the quality string AND some quality line starting by an '@'";
  t.expected_line_number = 36;
  t.expected_column_number = 5;
  t.expected_kmer = "ACGTT";
  t.test();

  t.skip_kmers(180);
  t.expected_line_number = 42;
  t.expected_column_number = 14;
  t.expected_kmer = "CGACT";
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " read 6 with 40 nucl. and a single '+' as separator";
  t.expected_line_number = 57;
  t.expected_column_number = 5;
  t.expected_kmer = "ACGTT";
  t.test();

  t.skip_kmers(216);
  t.expected_line_number = 57;
  t.expected_column_number = 40;
  t.expected_kmer = "CGACT";
  t.test();

  t.next_kmer(false);
  t.expected_line_number = 60;
  t.expected_column_number = 0;
  t.expected_sequence_description.clear();
  t.expected_kmer.clear();
  t.test();

  cout << "End of file '" << fname << "'" << endl << endl;


  ////////////////////////////
  // Tests with example2.fq //
  ////////////////////////////

  fname = "../resources/example2.fq";
  cout << "*** File Reader with a valid FASTQ file but strange content ('" << fname << "') ***" << endl << endl;
  reader.open(fname);

  t.expected_open_status = true;
  t.expected_filename = fname;
  t.expected_line_number = 1;
  t.expected_column_number = 0;
  t.expected_format = FileReader::FASTQ;
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " read 1 with 40 nucl. and a '+' followed by a wrongly repeated header as separator";
  t.expected_line_number = 2;
  t.expected_column_number = 5;
  t.expected_kmer = "ACGTT";
  t.test();

  t.skip_kmers(36);
  t.expected_column_number = 40;
  t.expected_kmer = "CGACT";
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " read 2 with 40 nucl (including degeneracy symbols at pos 1, 11 and 35)";
  t.expected_line_number = 7;
  t.expected_column_number = 7;
  t.expected_kmer = "CGTAC";
  ++t.expected_kmer_id;
  t.test();

  t.skip_kmers(42);
  t.expected_column_number = 11;
  t.expected_kmer = "CGACT";
  t.test();

  t.next_kmer();
  t.expected_column_number = 19;
  t.expected_kmer_id += reader.k();
  t.expected_kmer = "CGACT";
  t.test();

  t.skip_kmers(66);
  t.expected_column_number = 40;
  t.expected_kmer = "CAGCA";
  t.test();

  t.next_kmer();
  t.expected_column_number = 47;
  t.expected_kmer_id += reader.k();
  t.expected_kmer = "TGACA";
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " read 3 with 40 nucl (including degeneracy symbols at positions 1, 12 and 37 but also spurious symbols to ignore at positions 5, 25 and 40)";
  t.expected_line_number = 12;
  t.expected_column_number = 8;
  ++t.expected_kmer_id;
  t.expected_kmer = "CGTAC";
  t.test();

  t.skip_kmers(108);
  t.expected_column_number = 51;
  t.expected_kmer = "TGACA";
  t.test();

  t.next_kmer();
  t.expected_sequence_description = " read 4 (the last) with 40 nucl. and a single '+' as separator and spaces and newlines after the quality string. The last kmer should end with ...ACT";
  t.expected_line_number = 17;
  t.expected_column_number = 5;
  t.expected_kmer = "ACGTT";
  t.test();

  t.skip_kmers(144);
  t.expected_column_number = 40;
  t.expected_kmer = "CGACT";
  t.test();

  t.next_kmer(false);
  t.expected_line_number = 23;
  t.expected_column_number = 1;
  t.expected_sequence_description.clear();
  t.expected_kmer.clear();
  t.test();

  cout << "End of file '" << fname << "'" << endl << endl;

  return 0;
}
