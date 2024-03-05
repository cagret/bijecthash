#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include <vector>
#include <string>
#include <iostream>

struct Settings {
  static const Settings default_settings;
  static const std::vector<std::string> available_methods;
  std::string program_name;
  std::string filename;

  /**
   * The k-mer length.
   */
  size_t length;

  /**
   * The k-mer prefix length.
   */
  size_t prefix_length;

  std::string method;
  size_t nb_bins;
  bool verbose;

  void usage() const;
  Settings(int argc, char **argv);

  Settings();

};

std::ostream &operator<<(std::ostream &os, const Settings &s);

#endif
