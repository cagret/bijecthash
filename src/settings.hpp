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

#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include <iostream>
#include <memory>

namespace bijecthash {

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
     * \param method The transformation method name to use.
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

}

#endif
