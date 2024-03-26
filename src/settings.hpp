#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include <iostream>
#include <memory>

class Transformer;

/**
 * A simple structure to handle all necessary informations settings.
 */
struct Settings {

private:

  /**
   * The transformer to use.
   */
  std::shared_ptr<const Transformer> _transformer;

  /**
   * The transformer method to use.
   */
  std::string _method;

  /**
   * This method compute the Transformer corresponding to the given
   * string description.
   *
   * \param name The transformer to create.
   *
   * \return Returns the created transformer as a smart pointer.
   */
  std::shared_ptr<const Transformer> _string2transformer(const std::string &name) const;

public:

  /**
   * The k-mer length.
   */
  size_t length;

  /**
   * The k-mer prefix length.
   */
  size_t prefix_length;

  /**
   * The tag of the experiment.
   */
  std::string tag;

  /**
   * The number of bins to use for statistics computations.
   */
  size_t nb_bins;

  /**
   * The circular queue size to share data between the k-mer collectors and processors.
   */
  size_t queue_size;

  /**
   * Verbosity of the program.
   */
  bool verbose;

  /**
   * Settings constructor
   *
   * \param length The length of the k-mers.
   *
   * \param prefix_length The length of the k-mer prefix.
   *
   * \param method The transformation method to use.
   *
   * \param tag The tag of the experiment.
   *
   * \param nb_bins The number of bins to use for statistics computations.
   *
   * \param queue_size The circular queue size to share data between
   * the k-mer collectors and processors.
   *
   * \param verbose Verbosity of the program.
   */
  Settings(size_t length, size_t prefix_length, const std::string &method,
           const std::string &tag = "",
           size_t nb_bins = 1024, size_t queue_size = 1024,
           bool verbose = true);

  /**
   * Get the transformer method to use.
   *
   * \return Returns the transformer method to use.
   */
  inline std::string getMethod() const {
    return _method;
  }

  /**
   * Get the transformer method to use.
   *
   * \return Returns the transformer method to use.
   */
  void setMethod(const std::string &method);

  /**
   * Provide the transformer corresponding to this settings.
   *
   * \return Returns the transformer corresponding to the _method
   * attribute.
   */
  std::shared_ptr<const Transformer> transformer() const;

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
