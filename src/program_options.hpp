/******************************************************************************
*                                                                             *
*  Copyright © 2024-2025 -- LIRMM/CNRS/UM                                     *
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

#ifndef __PROGRAM_OPTIONS_HPP__
#define __PROGRAM_OPTIONS_HPP__

#include <string>
#include <vector>

#include <settings.hpp>

namespace bijecthash {

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

}

#endif
