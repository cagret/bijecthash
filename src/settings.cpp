#include "settings.hpp"

#include "identity_transformer.hpp"
#include "inthash_transformer.hpp"
#include "gab_transformer.hpp"
#include "permutation_transformer.hpp"

using namespace std;

const shared_ptr<const Transformer> Settings::transformer() const {
  shared_ptr<const Transformer> t(NULL);
  if (method == "identity") {
    t = make_shared<const IdentityTransformer>(IdentityTransformer(*this));
  } else if (method == "inthash") {
    t = make_shared<const IntHashTransformer>(IntHashTransformer(*this));
  } else if (method == "GAB") {
    t = make_shared<const GaBTransformer>(GaBTransformer(*this, 17, 42));
  } else if (method == "random") {
    t = make_shared<const PermutationTransformer>(PermutationTransformer(*this));
  } else if (method == "inverse") {
    vector<size_t> p(length);
    for (size_t i = 0; i < length; ++i) {
      p[i] = length - i - 1;
    }
    t = make_shared<const PermutationTransformer>(PermutationTransformer(*this, p, "Inverse"));
  } else if (method == "cyclic") {
    vector<size_t> p(length);
    for (size_t i = 0; i < length; ++i) {
      p[i] = (i + 1) % length;
    }
    t = make_shared<const PermutationTransformer>(PermutationTransformer(*this, p, "Cyclic"));
  } else if (method == "zigzag") {
    vector<size_t> p(length);
    for (size_t i = 0; i < length; ++i) {
      p[i] = ((i & 1) ? (length - i - (length & 1)) : i);
    }
    t = make_shared<const PermutationTransformer>(PermutationTransformer(*this, p, "ZigZag"));
  }
  return t;
}

ostream &operator<<(ostream &os, const Settings &s) {
  os << "- length: " << s.length << " nucleotides" << endl
     << "- prefix_length: " << s.prefix_length << " nucleotides" << endl
     << "- method: " << s.method << endl
     << "- nb_bins: " << s.nb_bins << " bins" << endl
     << "- queue_size: " << s.queue_size << " k-mers" << endl
     << "- tag: " << s.tag << endl
     << "- verbosity: " << (s.verbose ? "verbose" : "quiet") << endl;
  return os;
}

