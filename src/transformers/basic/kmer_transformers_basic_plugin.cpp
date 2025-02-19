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

#include <sphinx++/macros.h>

#include <memory>
#include <string>
#include <vector>

#include <common.hpp>
#include <exception.hpp>
#include <transformer.hpp>

#include "canonical_transformer.hpp"
#include "composition_transformer.hpp"
#include "identity_transformer.hpp"
#include "permutation_bit_transformer.hpp"
#include "permutation_transformer.hpp"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

////////////////////////////////////////////////////////////////

#define IDENTITY_TRANSFORMER_LABEL          "identity" SUFFIX
#define IDENTITY_TRANSFORMER_EXTRA          ""
#define IDENTITY_TRANSFORMER_DESCRIPTION                                \
  "The '" IDENTITY_TRANSFORMER_LABEL IDENTITY_TRANSFORMER_EXTRA "' "    \
  "k-mer is not really a transformation since the k-mer isn't "         \
  "modified. This \"transformer\" provides the reference to compare to."

#define CANONICAL_TRANSFORMER_LABEL         "canonical" SUFFIX
#define CANONICAL_TRANSFORMER_EXTRA         ""
#define CANONICAL_TRANSFORMER_DESCRIPTION                              \
  "The '" CANONICAL_TRANSFORMER_LABEL CANONICAL_TRANSFORMER_EXTRA "' "   \
  "k-mer of some given k-mer is the lowest between the k-mer itself "  \
  "and its reverse complement according to the lexicographic order."

#define RANDOM_NUCL_TRANSFORMER_LABEL       "random_nucl" SUFFIX
#define RANDOM_NUCL_TRANSFORMER_EXTRA       ""
#define RANDOM_NUCL_TRANSFORMER_DESCRIPTION                             \
  "The '" RANDOM_NUCL_TRANSFORMER_LABEL RANDOM_NUCL_TRANSFORMER_EXTRA "' " \
  "is a (fixed) permutation at the nucleotide level of the given k-mer."

#define RANDOM_BITS_TRANSFORMER_LABEL       "random_bits" SUFFIX
#define RANDOM_BITS_TRANSFORMER_EXTRA       ""
#define RANDOM_BITS_TRANSFORMER_DESCRIPTION                             \
  "The '" RANDOM_BITS_TRANSFORMER_LABEL RANDOM_BITS_TRANSFORMER_EXTRA "' " \
  "is a (fixed) permutation at the bit level of the given k-mer."

#define INVERSE_TRANSFORMER_LABEL           "inverse" SUFFIX
#define INVERSE_TRANSFORMER_EXTRA           ""
#define INVERSE_TRANSFORMER_DESCRIPTION                                 \
  "The '" INVERSE_TRANSFORMER_LABEL INVERSE_TRANSFORMER_EXTRA "' "      \
  "k-mer is just the k-mer which is spelled from right to left."

#define CYCLIC_TRANSFORMER_LABEL            "cyclic" SUFFIX
#define CYCLIC_TRANSFORMER_EXTRA            "[=<pos>]"
#define CYCLIC_TRANSFORMER_DESCRIPTION                                  \
  "The '" CYCLIC_TRANSFORMER_LABEL CYCLIC_TRANSFORMER_EXTRA "' "        \
  "k-mer is a cyclic permutation of the k-mer from the given position " \
  "(default pos: 1)."

#define ZIGZAG_TRANSFORMER_LABEL            "zigzag" SUFFIX
#define ZIGZAG_TRANSFORMER_EXTRA            ""
#define ZIGZAG_TRANSFORMER_DESCRIPTION                                  \
  "The '" ZIGZAG_TRANSFORMER_LABEL ZIGZAG_TRANSFORMER_EXTRA "' "        \
  "transform switches the first and last odd positions, then the second " \
  "and the before last odd positions and so on."

#define COMPOSITION_TRANSFORMER_LABEL       "composition" SUFFIX
#define COMPOSITION_TRANSFORMER_EXTRA       "(t1*t2)"
#define COMPOSITION_TRANSFORMER_DESCRIPTION                             \
  "The '" COMPOSITION_TRANSFORMER_LABEL COMPOSITION_TRANSFORMER_EXTRA "' " \
  "allows to build a complex transform which results from a composition " \
  "of simpler transforms (it appies t2 to the given k-mer, then t1 to the " \
  "result of this transform)."

////////////////////////////////////////////////////////////////


shared_ptr<const Transformer> _transformerFactory(size_t kmer_length,
                                                  size_t prefix_length,
                                                  const string &label,
                                                  const string &extra) {
  DEBUG_MSG("Basic plugin factory for '" << label << "' with args '" << extra << "'");
  shared_ptr<const Transformer> t(NULL);
  if (label == IDENTITY_TRANSFORMER_LABEL) {
    t = make_shared<const IdentityTransformer>(kmer_length, prefix_length);
  } else if (label == CANONICAL_TRANSFORMER_LABEL) {
    t = make_shared<const CanonicalTransformer>(kmer_length, prefix_length);
  } else if (label == RANDOM_NUCL_TRANSFORMER_LABEL) {
    t = make_shared<const PermutationTransformer>(kmer_length, prefix_length);
  } else if (label == RANDOM_BITS_TRANSFORMER_LABEL) {
    t = make_shared<const PermutationBitTransformer>(kmer_length, prefix_length);
  } else if (label == INVERSE_TRANSFORMER_LABEL) {
    vector<size_t> p(kmer_length);
    for (size_t i = 0; i < kmer_length; ++i) {
      p[i] = kmer_length - i - 1;
    }
    t = make_shared<const PermutationTransformer>(kmer_length, prefix_length, p, "Inverse");
  } else if (label == CYCLIC_TRANSFORMER_LABEL) {
    vector<size_t> p(kmer_length);
    size_t pivot = extra.empty() ? 1 : stoul(extra);
    for (size_t i = 0; i < kmer_length; ++i) {
      p[i] = (i + pivot) % kmer_length;
    }
    t = make_shared<const PermutationTransformer>(kmer_length, prefix_length, p, "Cyclic");
  } else if (label == ZIGZAG_TRANSFORMER_LABEL) {
    vector<size_t> p(kmer_length);
    for (size_t i = 0; i < kmer_length; ++i) {
      p[i] = ((i & 1) ? (kmer_length - i - (kmer_length & 1)) : i);
    }

    t = make_shared<const PermutationTransformer>(kmer_length, prefix_length, p, "ZigZag");
  } else if (label == COMPOSITION_TRANSFORMER_LABEL) {
    size_t count = (extra[0] == '(');
    size_t p = 1;
    size_t separator = 0;
    size_t n = extra.length();
    string t1_name;
    string t2_name;
    while ((p < n) && count) {
      switch (extra[p]) {
      case '(':
        ++count;
        break;
      case ')':
        --count;
        break;
      case '*':
        if ((count == 1) && (separator == 0)) {
          assert(t1_name.empty());
          t1_name = extra.substr(1, p - 1);
          separator = p;
        }
        break;
      default:
        break;
      }
      ++p;
    }
    if (count != 0) {
      Exception e;
      e << "Error: unable to parse the Composition method parameters." << endl
        << "       Missing " << count << " closing parenthesis." << endl
        << "=> '" << label << extra << "'" << endl
        << "   " << string(label.size() + n + 1, ' ') << "^\n";
      throw e;
    }
    if (separator == 0) {
      Exception e;
      e << "Error: unable to parse the Composition method parameters." << endl
        << "       Missing '*'." << endl
        << "=> '" << label << extra << "'" << endl
        << "   " << string(label.size() + p, ' ') << "^\n";
      throw e;
    }
    if (separator == 1) {
      Exception e;
      e << "Error: unable to parse the Composition method parameters." << endl
        << "       Missing first transformer." << endl
        << "=> '" << label << extra << "'" << endl
        << "   " << string(label.size() + separator + 1, ' ') << "^\n";
      throw e;
    }
    if (p != n) {
      Exception e;
      e << "Error: unable to parse the Composition method parameters." << endl
        << "       Unexpected character." << endl
        << "=> '" << label << extra << "'" << endl
        << "   " << string(label.size() + p + 1, ' ') << "^\n";
      throw e;
    }
    if (separator + 2 >= n) {
      Exception e;
      e << "Error: unable to parse the Composition method parameters." << endl
        << "       Missing second operand." << endl
        << "=> '" << label << extra << "'" << endl
        << "   " << string(label.size() + separator + 2, ' ') << "^\n";
      throw e;
    }
    t2_name = extra.substr(separator + 1, n - separator - 2);
    shared_ptr<const Transformer> t1 = Transformer::string2transformer(kmer_length, prefix_length, t1_name);
    shared_ptr<const Transformer> t2 = Transformer::string2transformer(kmer_length, prefix_length, t2_name);
    DEBUG_MSG("Making composed of '" << t1_name << "' and '" << t2_name << "'");
    t = make_shared<const CompositionTransformer>(kmer_length, prefix_length, t1, t2);
  } else {
    Exception e;
    e << "Error: Unsupported transformation method '" << label << extra << "'.\n";
    throw e;
  }
  return t;
}

Transformer::TransformerInformations _transformerList[] = {
  { IDENTITY_TRANSFORMER_LABEL,
    IDENTITY_TRANSFORMER_EXTRA,
    IDENTITY_TRANSFORMER_DESCRIPTION },
  { CANONICAL_TRANSFORMER_LABEL,
    CANONICAL_TRANSFORMER_EXTRA,
    CANONICAL_TRANSFORMER_DESCRIPTION },
  { RANDOM_BITS_TRANSFORMER_LABEL,
    RANDOM_BITS_TRANSFORMER_EXTRA,
    RANDOM_BITS_TRANSFORMER_DESCRIPTION },
  { RANDOM_NUCL_TRANSFORMER_LABEL,
    RANDOM_NUCL_TRANSFORMER_EXTRA,
    RANDOM_NUCL_TRANSFORMER_DESCRIPTION },
  { INVERSE_TRANSFORMER_LABEL,
    INVERSE_TRANSFORMER_EXTRA,
    INVERSE_TRANSFORMER_DESCRIPTION },
  { CYCLIC_TRANSFORMER_LABEL,
    CYCLIC_TRANSFORMER_EXTRA,
    CYCLIC_TRANSFORMER_DESCRIPTION },
  { ZIGZAG_TRANSFORMER_LABEL,
    ZIGZAG_TRANSFORMER_EXTRA,
    ZIGZAG_TRANSFORMER_DESCRIPTION },
  { COMPOSITION_TRANSFORMER_LABEL,
    COMPOSITION_TRANSFORMER_EXTRA,
    COMPOSITION_TRANSFORMER_DESCRIPTION },
  { "", "", "" }
};

////////////////////////////////////////////////////////////////

extern "C" {

  SPHINXPP_SET_PLUGIN_NAME("basic-transformers" SUFFIX);
  SPHINXPP_SET_PLUGIN_TAG(Transformer::SPHINXPP_PLUGIN_TAG);
  SPHINXPP_SET_PLUGIN_AUTHORS(PACKAGE_AUTHORS);
  SPHINXPP_SET_PLUGIN_SUMMARY("Basic k-mer transformers implementations");
  SPHINXPP_SET_PLUGIN_DESCRIPTION
  ("This plugin provides the following transformers which are the "
   "classical transformers one can expect.\n"
   IDENTITY_TRANSFORMER_DESCRIPTION "\n"
   CANONICAL_TRANSFORMER_DESCRIPTION "\n"
   RANDOM_NUCL_TRANSFORMER_DESCRIPTION "\n"
   RANDOM_BITS_TRANSFORMER_DESCRIPTION "\n"
   INVERSE_TRANSFORMER_DESCRIPTION "\n"
   CYCLIC_TRANSFORMER_DESCRIPTION "\n"
   ZIGZAG_TRANSFORMER_DESCRIPTION "\n"
   COMPOSITION_TRANSFORMER_DESCRIPTION "\n"
   DESC_COMPLEMENT
   );
  SPHINXPP_SET_PLUGIN_VERSION(PLUGIN_VERSION);
  SPHINXPP_PLUGIN_DECLARE_SYMBOLS("transformerFactory", "transformerList");

  Transformer::TransformerInformations *transformerList = _transformerList;

  extern Transformer::TransformerInformations *transformerList;

  void transformerFactory(size_t kmer_length,
                          size_t suffix_length,
                          const string &label,
                          const string &extra,
                          shared_ptr<const Transformer> &ptr) {
    ptr = _transformerFactory(kmer_length, suffix_length, label, extra);
  }

}

END_BIJECTHASH_NAMESPACE
