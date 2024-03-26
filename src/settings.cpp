#include "settings.hpp"

#include "canonical_transformer.hpp"
#include "identity_transformer.hpp"
#include "inthash_transformer.hpp"
#include "gab_transformer.hpp"
#include "minimizer_transformer.hpp"
#include "lyndon_transformer.hpp"
#include "permutation_transformer.hpp"
#include "permutation_bit_transformer.hpp"

#include <cstdlib>
#include <algorithm>

using namespace std;

Settings::Settings(size_t length, size_t prefix_length, const std::string &method,
                   const std::string &tag,
                   size_t nb_bins, size_t queue_size,
                   bool verbose):
  _transformer(), _method(method),
  length(length), prefix_length(prefix_length),
  tag(tag),
  nb_bins(nb_bins), queue_size(queue_size),
  verbose(verbose)
{
  assert(prefix_length > 0);
  assert(prefix_length < 14);
  assert(prefix_length < length);
  assert(length - prefix_length <= 64);
  _transformer = _string2transformer(method);
}

shared_ptr<const Transformer> Settings::_string2transformer(const string &name) const {
  shared_ptr<const Transformer> t(NULL);
  if (name == "identity") {
    t = make_shared<const IdentityTransformer>(*this);
  } else if (name == "canonical") {
    t = make_shared<const CanonicalTransformer>(*this);
  } else if (name == "inthash") {
    t = make_shared<const IntHashTransformer>(*this);
  } else if (name == "minimizer") {
    t = make_shared<const MinimizerTransformer>(*this);
  } else if (name.substr(0,3) == "GAB") {
    uint64_t a = 0, b = 0;
    if (name[3] == '=') {
      char *ptr;
      a = strtoul(name.c_str() + 4, &ptr, 10);
      if (*ptr == ',') {
        ++ptr;
        b = strtoul(ptr, &ptr, 10);
      }
      if (*ptr != '\0') {
        cerr << "Error: unable to parse the GAB method parameters." << endl;
        exit(1);
      }
    }
    t = make_shared<const GaBTransformer>(*this, a, b);
  } else if (name == "random_nucl") {
    t = make_shared<const PermutationTransformer>(*this);
  } else if (name == "random_bits") {
    t = make_shared<const PermutationBitTransformer>(*this);
  } else if (name == "inverse") {
    vector<size_t> p(length);
    for (size_t i = 0; i < length; ++i) {
      p[i] = length - i - 1;
    }
    t = make_shared<const PermutationTransformer>(*this, p, "Inverse");
  } else if (name == "cyclic") {
    vector<size_t> p(length);
    for (size_t i = 0; i < length; ++i) {
      p[i] = (i + 1) % length;
    }
    t = make_shared<const PermutationTransformer>(*this, p, "Cyclic");
  } else if (name == "lyndon") {
    t = make_shared<const LyndonTransformer>(*this);
  } else if (name == "zigzag") {
    vector<size_t> p(length);
    for (size_t i = 0; i < length; ++i) {
      p[i] = ((i & 1) ? (length - i - (length & 1)) : i);
    }

    t = make_shared<const PermutationTransformer>(*this, p, "ZigZag");
  } else {
    cerr << "Error: Unsupported transformation method '" << name << "'." << endl;
    exit(1);
  }
  return t;
}

void Settings::setMethod(const string &method) {
  _transformer = _string2transformer(method);
  _method = method;
}

shared_ptr<const Transformer> Settings::transformer() const {
  if (!_transformer) {
    shared_ptr<const Transformer> _t = const_pointer_cast<const Transformer>(_transformer);
    _t = _string2transformer(_method);
   }
  return _transformer;
}

ostream &operator<<(ostream &os, const Settings &s) {
  os << "- length: " << s.length << " nucleotides" << endl
     << "- prefix_length: " << s.prefix_length << " nucleotides" << endl
     << "- method: " << s.getMethod() << " => " << s.transformer()->description << endl
     << "- nb_bins: " << s.nb_bins << " bins" << endl
     << "- queue_size: " << s.queue_size << " k-mers" << endl
     << "- tag: " << s.tag << endl
     << "- verbosity: " << (s.verbose ? "verbose" : "quiet") << endl;
  return os;
}

