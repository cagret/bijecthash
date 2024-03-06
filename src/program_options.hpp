#ifndef __PROGRAM_OPTIONS_HPP__
#define __PROGRAM_OPTIONS_HPP__

#include <vector>
#include <string>

#include <settings.hpp>

/**
 * Program option programr.
 */
class ProgramOptions {

private:
  /**
   * The program name.
   */
  const std::string _program_name;

  /**
   * The program settings.
   */
  Settings _settings;

  /**
   * List of file o process.
   */
  std::vector<std::string> _filenames;

public:

  /**
   * The default program settings
   */
  static const Settings default_settings;

  /**
   * The program available transformers.
   */
  static const std::vector<std::string> available_methods;

  /**
   * Print usage of the program then exit.
   */
  void usage() const;

  /**
   * Program program command line.
   *
   * \param argc The number of arguments in the command line
   *
   * \param argv The array of command line arguments (argv[0] must be the invoked program name).
   */
  ProgramOptions(int argc, char **argv);

  /**
   * Get the program name.
   *
   * \return Return the program name.
   */
  inline const std::string &programName() const {
    return _program_name;
  }

  /**
   * Get the program settings.
   *
   * \return Return the program settings.
   */
  inline const Settings &settings() const {
    return _settings;
  }

  /**
   * Get the filenames to process.
   *
   * \return Return the filenames to process.
   */
  inline const std::vector<std::string> &filenames() const {
    return _filenames;
  }

};

#endif
