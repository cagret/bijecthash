#include "settings.hpp"

#include "identity_transformer.hpp"
#include "inthash_transformer.hpp"
#include "gab_transformer.hpp"
#include "minimizer_transformer.hpp"
#include "bwt_transformer.hpp"
#include "lyndon_transformer.hpp"
#include "permutation_transformer.hpp"
#include "permutation_bit_transformer.hpp"

#include <cstdlib>
#include <algorithm>

using namespace std;

const shared_ptr<const Transformer> Settings::transformer() const {
  shared_ptr<const Transformer> t(NULL);
  if (method == "identity") {
    t = make_shared<const IdentityTransformer>(IdentityTransformer(*this));
  } else if (method == "inthash") {
    t = make_shared<const IntHashTransformer>(IntHashTransformer(*this));
  } else if (method == "minimizer") {
    t = make_shared<const MinimizerTransformer>(MinimizerTransformer(*this));
  } else if (method == "bwt") {
    t = make_shared<const BwtTransformer>(BwtTransformer(*this));
  } else if (method.substr(0,3) == "GAB") {
    uint64_t a = 0, b = 0;
    if (method[3] == '=') {
      char *ptr;
      a = strtoul(method.c_str() + 4, &ptr, 10);
      if (*ptr == ',') {
        ++ptr;
        b = strtoul(ptr, &ptr, 10);
      }
      if (*ptr != '\0') {
        cerr << "Error: unable to parse the GAB method parameters." << endl;
        exit(1);
      }
    }
    t = make_shared<const GaBTransformer>(GaBTransformer(*this, a, b));
  } else if (method == "random_nucl") {
    t = make_shared<const PermutationTransformer>(PermutationTransformer(*this));
  } else if (method == "random_bits") {
    t = make_shared<const PermutationBitTransformer>(PermutationBitTransformer(*this));
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
  } else if (method == "lyndon") {
    t = make_shared<const LyndonTransformer>(LyndonTransformer(*this));
  } else if (method == "zigzag") {
    vector<size_t> p(length);
    for (size_t i = 0; i < length; ++i) {
      p[i] = ((i & 1) ? (length - i - (length & 1)) : i);
    }

    t = make_shared<const PermutationTransformer>(PermutationTransformer(*this, p, "ZigZag"));
  }
  else {
    cerr << "Error: Unsupported transformation method '" << method << "'." << endl;
    exit(1);
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

