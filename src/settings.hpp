#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include <iostream>

/**
 * A simple structure to handle all necessary informations settings.
 */
struct Settings {

  /**
   * The k-mer length.
   */
  size_t length;

  /**
   * The k-mer prefix length.
   */
  size_t prefix_length;

  /**
   * The transformer method to use.
   */
  std::string method;

  /**
   * The tag of the experiment.
   */
  std::string tag;

  /**
   * The number of bins to use for statistics computations.
   */
  size_t nb_bins;

  /**
   * Verbosity of the program.
   */
  bool verbose;

};

/**
 * Print settings.
 *
 * \param os The output stream on which to output the given settings.
 *
 * \param s The settings to output.
 *
 * \return Returns the modified output stream.
 */
std::ostream &operator<<(std::ostream &os, const Settings &s);

#endif
