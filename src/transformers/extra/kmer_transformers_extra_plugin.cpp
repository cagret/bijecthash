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

#include "bwt_transformer.hpp"
#include "gab_transformer.hpp"
#include "inthash_transformer.hpp"
#include "lyndon_transformer.hpp"
#include "minimizer_transformer.hpp"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

////////////////////////////////////////////////////////////////

#define BWT_TRANSFORMER_LABEL          "bwt" SUFFIX
#define BWT_TRANSFORMER_EXTRA          ""
#define BWT_TRANSFORMER_DESCRIPTION                                     \
  "The '" BWT_TRANSFORMER_LABEL BWT_TRANSFORMER_EXTRA "' "              \
  "computes the Burrows-Wheeler transform of the given the k-mer."

#define LYNDON_TRANSFORMER_LABEL       "lyndon" SUFFIX
#define LYNDON_TRANSFORMER_EXTRA       ""
#define LYNDON_TRANSFORMER_DESCRIPTION                             \
  "The '" LYNDON_TRANSFORMER_LABEL LYNDON_TRANSFORMER_EXTRA "' "   \
  "encodes the given k-mer using the Lyndon rotation."

#define INTHASH_TRANSFORMER_LABEL       "inthash" SUFFIX
#define INTHASH_TRANSFORMER_EXTRA       ""
#define INTHASH_TRANSFORMER_DESCRIPTION                             \
  "The '" INTHASH_TRANSFORMER_LABEL INTHASH_TRANSFORMER_EXTRA "' " \
  "encodes the given k-mer using the hash64 function."

#define GAB_TRANSFORMER_LABEL         "Gab" SUFFIX
#define GAB_TRANSFORMER_EXTRA         "[=a,b]"
#define GAB_TRANSFORMER_DESCRIPTION                                     \
  "The '" GAB_TRANSFORMER_LABEL GAB_TRANSFORMER_EXTRA "' "              \
  "is a generalization of the inthash64. The a parameter must be an odd " \
  "value (if an even value is provided, then the next value is used), the " \
  "b value acts as a bit mask of length 2 * k."

#define MINIMIZER_TRANSFORMER_LABEL           "minimizer" SUFFIX
#define MINIMIZER_TRANSFORMER_EXTRA           ""
#define MINIMIZER_TRANSFORMER_DESCRIPTION                               \
  "The '" MINIMIZER_TRANSFORMER_LABEL MINIMIZER_TRANSFORMER_EXTRA "' "  \
  "uses the minimizer of the k-mer to encode its transform."


////////////////////////////////////////////////////////////////


shared_ptr<const Transformer> _transformerFactory(size_t kmer_length,
                                                  size_t prefix_length,
                                                  const string &label,
                                                  const string &extra) {
  DEBUG_MSG("Extra plugin factory for '" << label << "' with args '" << extra << "'");
  shared_ptr<const Transformer> t(NULL);
  if (label == BWT_TRANSFORMER_LABEL) {
    t = make_shared<const BwtTransformer>(kmer_length, prefix_length);
  } else if (label == LYNDON_TRANSFORMER_LABEL) {
    t = make_shared<const LyndonTransformer>(kmer_length, prefix_length);
  } else if (label == INTHASH_TRANSFORMER_LABEL) {
    t = make_shared<const IntHashTransformer>(kmer_length, prefix_length);
  } else if (label == GAB_TRANSFORMER_LABEL) {
    uint64_t a = 0, b = 0;
    if (!extra.empty()) {
      char *ptr;
      a = strtoul(extra.c_str(), &ptr, 10);
      if (*ptr == ',') {
        ++ptr;
        b = strtoul(ptr, &ptr, 10);
      }
      if (*ptr != '\0') {
        throw Exception("Error: unable to parse the GAB method parameters\n.");
      }
    }
    t = make_shared<const GaBTransformer>(kmer_length, prefix_length, a, b);
  } else if (label == MINIMIZER_TRANSFORMER_LABEL) {
    t = make_shared<const MinimizerTransformer>(kmer_length, prefix_length);
  } else {
    Exception e;
    e << "Error: Unsupported transformation method '" << label << extra << "'.\n";
    throw e;
  }
  return t;
}

Transformer::TransformerInformations _transformerList[] = {
  { BWT_TRANSFORMER_LABEL,
    BWT_TRANSFORMER_EXTRA,
    BWT_TRANSFORMER_DESCRIPTION },
  { LYNDON_TRANSFORMER_LABEL,
    LYNDON_TRANSFORMER_EXTRA,
    LYNDON_TRANSFORMER_DESCRIPTION },
  { INTHASH_TRANSFORMER_LABEL,
    INTHASH_TRANSFORMER_EXTRA,
    INTHASH_TRANSFORMER_DESCRIPTION },
  { GAB_TRANSFORMER_LABEL,
    GAB_TRANSFORMER_EXTRA,
    GAB_TRANSFORMER_DESCRIPTION },
  { MINIMIZER_TRANSFORMER_LABEL,
    MINIMIZER_TRANSFORMER_EXTRA,
    MINIMIZER_TRANSFORMER_DESCRIPTION },
  { "", "", "" }
};

////////////////////////////////////////////////////////////////

extern "C" {

  SPHINXPP_SET_PLUGIN_NAME("extra-transformers" SUFFIX);
  SPHINXPP_SET_PLUGIN_TAG(Transformer::SPHINXPP_PLUGIN_TAG);
  SPHINXPP_SET_PLUGIN_AUTHORS("Antoine LIMASSET <antoine.limasset@univ-lille.fr>, " PACKAGE_AUTHORS);
  SPHINXPP_SET_PLUGIN_SUMMARY("Extra k-mer transformers implementations");
  SPHINXPP_SET_PLUGIN_DESCRIPTION
  ("This plugin provides the following transformers which corresponds to "
   "usual text transformations in the field of stringology:\n"
   BWT_TRANSFORMER_DESCRIPTION "\n"
   LYNDON_TRANSFORMER_DESCRIPTION "\n"
   INTHASH_TRANSFORMER_DESCRIPTION "\n"
   GAB_TRANSFORMER_DESCRIPTION "\n"
   MINIMIZER_TRANSFORMER_DESCRIPTION "\n"
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
