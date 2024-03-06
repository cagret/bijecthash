#include "settings.hpp"

using namespace std;

ostream &operator<<(ostream &os, const Settings &s) {
  os << "- length: " << s.length << endl
     << "- prefix_length: " << s.prefix_length << endl
     << "- method: " << s.method << endl
     << "- nb_bins: " << s.nb_bins << endl
     << "- tag: " << s.tag << endl
     << "- verbose: " << s.verbose << endl;
  return os;
}

