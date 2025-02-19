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

#ifndef __BH_EXCEPTION_HPP__
#define __BH_EXCEPTION_HPP__

#include <exception>
#include <iostream>
#include <string>
#include <sstream>

namespace bijecthash {

  /**
   * Generic exception with an associated message that can be given
   * extra informations as a string stream.
   */
  class Exception: public std::exception {

  protected:

    /**
     * The exception message.
     */
    std::string _msg;

  public:

    /**
     * Create an exception with some initial message.
     *
     * \param msg The initial message string.
     */
    inline Exception(const std::string &msg = ""): std::exception(), _msg(msg) {}

    /**
     * Get the message associated with this exception.
     *
     * \return Returns the C string message associated to this exception.
     */
    inline virtual const char *what() const noexcept {
      return _msg.c_str();
    }

    /**
     * Template operator << to inject any type having the capacity to
     * be injected into an output stream.
     *
     * \param t The value to append to this exception message.
     *
     * \return Returns this exception.
     */
    template <typename T>
    Exception &operator<<(const T &t) {
      std::ostringstream s;
      s << t;
      _msg += s.str();
      return *this;
    }

    /**
     * Injects modifier function (like std::endl) into the message.
     *
     * \param pf The stream modifier function to apply to this
     * exception message.
     *
     * \return Returns this exception.
     */
    Exception &operator<<(std::ostream &(*pf)(std::ostream &)) {
      std::ostringstream s;
      s << pf;
      _msg += s.str();
      return *this;
    }

  };

}

#endif
