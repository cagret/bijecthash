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

#include "composition_transformer.hpp"

#include "common.hpp"
#include "identity_transformer.hpp"

using namespace std;

BEGIN_BIJECTHASH_NAMESPACE

CompositionTransformer::CompositionTransformer(size_t kmer_length, size_t prefix_length, shared_ptr<const Transformer> &t1, shared_ptr<const Transformer> &t2, const string &description):
  Transformer(kmer_length, prefix_length, description), _t1(t1), _t2(t2)
{
  if (description.empty()) {
    string *desc_ptr = const_cast<string *>(&(this->description));
    desc_ptr->clear();
    *desc_ptr += "(";
    *desc_ptr += t1->description;
    *desc_ptr += " ° ";
    *desc_ptr += t2->description;
    *desc_ptr += ")";
  }
  DEBUG_MSG("description: '" << description << "'");
}

Transformer::EncodedKmer CompositionTransformer::operator()(const string &kmer) const {
  EncodedKmer e1 = (*_t1)(kmer);
  string s1 = _t1->getTransformedKmer(e1);
  EncodedKmer e2 = (*_t2)(s1);
#ifdef DEBUG
  string orig_kmer = (*this)(e2);
  string s2 = _t2->getTransformedKmer(e2);
  DEBUG_MSG("_t1(" << kmer << "):   '" << s1 << "'");
  DEBUG_MSG("_t2(" << s1 << "): '" << s2 << "'");
  DEBUG_MSG("orig_kmer: '" << orig_kmer << "'");
  if (orig_kmer != kmer) {
    io_mutex.lock();
    cerr << "Error: the unpermuted k-mer"
         << " differs from the original k-mer"
         << endl;
    io_mutex.unlock();
    exit(1);
  }
#endif
  return e2;
}

string CompositionTransformer::operator()(const Transformer::EncodedKmer &e) const {
  // string s2 = (*_t2)(e);
  // EncodedKmer e1;
  // e1.prefix = _encode(s2.c_str(), prefix_length);
  // e1.suffix = _encode(s2.c_str() + prefix_length, suffix_length);
  // string s1 = (*_t1)(e1);
  string s2 = (*_t2)(e);
  EncodedKmer e1 = IdentityTransformer(kmer_length, prefix_length)(s2);
  string s1 = (*_t1)(e1);
  return s1;
}

string CompositionTransformer::getTransformedKmer(const Transformer::EncodedKmer &e) const {
  string s2 = _t2->getTransformedKmer(e);
  EncodedKmer e1 = IdentityTransformer(kmer_length, prefix_length)(s2);
  string s1 = _t1->getTransformedKmer(e1);
  return s1;
}

shared_ptr<const CompositionTransformer> operator*(shared_ptr<const Transformer> &t2, shared_ptr<const Transformer> &t1) {
  assert(t1->kmer_length == t2->kmer_length);
  assert(t1->prefix_length == t2->prefix_length);
  shared_ptr<const CompositionTransformer> t = make_shared<const CompositionTransformer>(CompositionTransformer(t1->kmer_length, t1->prefix_length, t1, t2));
  return t;
}

END_BIJECTHASH_NAMESPACE
