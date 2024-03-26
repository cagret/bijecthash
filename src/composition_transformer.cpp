#include "composition_transformer.hpp"

#include "identity_transformer.hpp"

#include <cassert>

#ifdef DEBUG
#include <iostream>
#include "locker.hpp"
#endif

using namespace std;

CompositionTransformer::CompositionTransformer(const Settings &s, shared_ptr<const Transformer> &t1, shared_ptr<const Transformer> &t2, const std::string &description):
  Transformer(s, description), _t1(t1), _t2(t2)
{
  // assert(settings.length <= 32);
  if (description.empty()) {
    string *desc_ptr = const_cast<string *>(&(this->description));
    desc_ptr->clear();
    *desc_ptr += "(";
    *desc_ptr += t1->description;
    *desc_ptr += " ° ";
    *desc_ptr += t2->description;
    *desc_ptr += ")";
  }
#ifdef DEBUG
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "description: '" << description << "'" << endl;
  cerr << endl;
  io_mutex.unlock();
#endif
}

Transformer::EncodedKmer CompositionTransformer::operator()(const std::string &kmer) const {
  EncodedKmer e1 = (*_t1)(kmer);
  string s1 = _t1->getTransformedKmer(e1);
  EncodedKmer e2 = (*_t2)(s1);
#ifdef DEBUG
  string orig_kmer = (*this)(e2);
  string s2 = _t2->getTransformedKmer(e2);
  io_mutex.lock();
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "_t1(" << kmer << "):   '" << s1 << "'" << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "_t2(" << s1 << "): '" << s2 << "'" << endl;
  cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << "orig_kmer: '" << orig_kmer << "'" << endl;

  if (orig_kmer != kmer) {
    cerr << "Error: the unpermuted k-mer"
         << " differs from the original k-mer"
         << endl;
    exit(1);
  }
  io_mutex.unlock();
#endif
  return e2;
}

string CompositionTransformer::operator()(const Transformer::EncodedKmer &e) const {
  // string s2 = (*_t2)(e);
  // EncodedKmer e1;
  // e1.prefix = _encode(s2.c_str(), settings.prefix_length);
  // e1.suffix = _encode(s2.c_str() + settings.prefix_length, settings.length - settings.prefix_length);
  // string s1 = (*_t1)(e1);
  string s2 = (*_t2)(e);
  EncodedKmer e1 = IdentityTransformer(settings)(s2);
  string s1 = (*_t1)(e1);
  return s1;
}

string CompositionTransformer::getTransformedKmer(const Transformer::EncodedKmer &e) const {
  string s2 = _t2->getTransformedKmer(e);
  EncodedKmer e1 = IdentityTransformer(settings)(s2);
  string s1 = _t1->getTransformedKmer(e1);
  return s1;
}

shared_ptr<const CompositionTransformer> operator*(shared_ptr<const Transformer> &t2, shared_ptr<const Transformer> &t1) {
  assert(t1->settings.length == t2->settings.length);
  assert(t1->settings.prefix_length == t2->settings.prefix_length);
  shared_ptr<const CompositionTransformer> t = make_shared<const CompositionTransformer>(CompositionTransformer(t1->settings, t1, t2));
  return t;
}
