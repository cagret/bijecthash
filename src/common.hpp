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

#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

// Make debug easy with macros:
// - MSG_DBG_HEADER
// - DBG(instructions)
// - DEBUG_MSG(msg)
// Set SUFFIX and DESC_COMPLEMENT C strings for k-mer transformer
// plugins
#ifdef DEBUG

#  include <cerrno>
#  include <iostream>
#  include <thread>

#  include <locker.hpp>

// Tags for (debug) messages
#  define MSG_DBG_HEADER                                          \
  "[DEBUG] "                                                      \
  << "[Thread " << this_thread::get_id()  << "] "                 \
  << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "

#  define DBG(instructions) do { instructions } while 0

#  define DEBUG_MSG(msg)                                \
  bijecthash::io_mutex.lock();                          \
  std::cerr << MSG_DBG_HEADER << msg << std::endl;      \
  bijecthash::io_mutex.unlock()

#  define SUFFIX "-dbg"
#  define DESC_COMPLEMENT                                               \
  "\nThis is the debug version of these transformers (compiled with all debugging informations)."
#  ifdef NDEBUG
#    undef NDEBUG
#  endif

#else

#  define DBG(instructions)           (void)0
#  define DEBUG_MSG(msg)              (void)0

#  ifdef NDEBUG
#    define SUFFIX
#    define DESC_COMPLEMENT
#  else
#    define SUFFIX "-check"
#    define DESC_COMPLEMENT                                             \
  "\nThis is the development version of these transformers which are compiled with assertion checkings."
#  endif

#endif

// Handle unused parameters
#ifndef __UNUSED__
#  define __UNUSED__(x)
#endif
#ifndef __USED_IN_ASSERT_ONLY__
#  ifdef NDEBUG
#    define __USED_IN_ASSERT_ONLY__(x) __UNUSED__(x)
#  else
#    define __USED_IN_ASSERT_ONLY__(x) x
#  endif
#endif
#ifndef __USED_IN_DEBUG_ONLY__
#  ifdef DEBUG
#    define __USED_IN_DEBUG_ONLY__(x) x
#  else
#    define __USED_IN_DEBUG_ONLY__(x) __UNUSED__(x)
#  endif
#endif

// Prepare assert() macro (or not if NDEBUG is defined, thus if DEBUG is not defined)
#include <cassert>

#endif
