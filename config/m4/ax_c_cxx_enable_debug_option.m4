# ===========================================================================
#  https://https://gite.lirmm.fr/doccy-dev-tools/autoconf
# ===========================================================================
#
# Serial 2
#
# SYNOPSIS
#
#   AX_C_CXX_ENABLE_DEBUG_OPTION([EXTRA_HELP_MESSAGE],
#                                [FORCE-ENABLE-TEST],
#                                [FORCE-DISABLE-TEST])
#
#
# DESCRIPTION
#
#   The `AX_ENABLE_DEBUG_OPTION()` macro adds the `--enable-debug[=mode]`
#   (and thus `--disable-debug`) options to the configure script, which
#   adapts the C/C++ (pre)compiler flags according to the given
#   mode. Providing '--disable-debug` option to the configure script is
#   equivalent to `--enable-debug=no`.
#
#   If the mode is anything but "no", then the `-g -O0 -fno-inline` flags are
#   added to the C/C++ compiler flags and the preprocessor symbol `NDEBUG` is
#   undefined (thus assertions are enabled, see `assert` manpage).
#
#   If the mode is a non empty string except "no", then additionally to the
#   previous behavior, the preprocessor symbol `DEBUG` is defined in the
#   configuration header generated by the `AC_CONFIG_HEADERS()` macro.
#
#   Finally, if option is not given or is given mode "no", then the `-O3`
#   flag is added to the C/C++ compiler flags and the preprocessor symbol
#   `NDEBUG` is defined (disabling assertions, see `assert` manpage).
#
#   The `EXTRA_HELP_MESSAGE` macro argument is appended to the help string of
#   the `--enable-debug` option.
#
#   If some `FORCE-ENABLE-TEST` is given and evaluates to true, then the
#   debug mode is enabled. If it was explicitly enabled in command line, the
#   mode is unchanged, otherwise it is set to "yes". If the
#   `--enable-debug=no` option was provided, then an error message is
#   displayed during configuration.
#
#   If some `FORCE-DISABLE-TEST` is given and evaluates to true, then the
#   debug mode is disabled. If it was explicitly enabled in command line,
#   then an error message is displayed during configuration.
#
#   The `enable_debug` shell variable is set to the mode and can be used in
#   the `configure.ac`.
#
#
# LICENSE
#
#   Copyright © 2010-2025 -- LIRMM / CNRS / UM
#                            (Laboratoire d'Informatique, de Robotique et de
#                            Microélectronique de Montpellier /
#                            Centre National de la Recherche Scientifique /
#                            Université de Montpellier)
#
#
#   Auteur/Author:
#     - Alban MANCHERON  <alban.mancheron@lirmm.fr>
#
#   Programmeurs/Programmers:
#     - Alban MANCHERON  <alban.mancheron@lirmm.fr>
#
#   -------------------------------------------------------------------------
#
#   Ce logiciel est régi par la  licence CeCILL  soumise au droit français et
#   respectant les principes  de diffusion des logiciels libres.  Vous pouvez
#   utiliser, modifier et/ou redistribuer ce programme sous les conditions de
#   la licence CeCILL telle que diffusée par  le CEA,  le CNRS et l'INRIA sur
#   le site "http://www.cecill.info".
#
#   En contrepartie de l'accessibilité au code source et des droits de copie,
#   de modification et de redistribution accordés par cette licence, il n'est
#   offert aux utilisateurs qu'une garantie limitée.  Pour les mêmes raisons,
#   seule une responsabilité  restreinte pèse  sur l'auteur du programme,  le
#   titulaire des droits patrimoniaux et les concédants successifs.
#
#   À  cet égard  l'attention de  l'utilisateur est  attirée sur  les risques
#   associés  au chargement,  à  l'utilisation,  à  la modification  et/ou au
#   développement  et à la reproduction du  logiciel par  l'utilisateur étant
#   donné  sa spécificité  de logiciel libre,  qui peut le rendre  complexe à
#   manipuler et qui le réserve donc à des développeurs et des professionnels
#   avertis  possédant  des  connaissances  informatiques  approfondies.  Les
#   utilisateurs  sont donc  invités  à  charger  et  tester  l'adéquation du
#   logiciel  à leurs besoins  dans des conditions  permettant  d'assurer  la
#   sécurité de leurs systêmes et ou de leurs données et,  plus généralement,
#   à l'utiliser et l'exploiter dans les mêmes conditions de sécurité.
#
#   Le fait que  vous puissiez accéder  à cet en-tête signifie  que vous avez
#   pris connaissance  de la licence CeCILL,  et que vous en avez accepté les
#   termes.
#
#   -------------------------------------------------------------------------
#
#   This software is governed by the CeCILL license under French law and
#   abiding by the rules of distribution of free software. You can use,
#   modify and/ or redistribute the software under the terms of the CeCILL
#   license as circulated by CEA, CNRS and INRIA at the following URL
#   "http://www.cecill.info".
#
#   As a counterpart to the access to the source code and rights to copy,
#   modify and redistribute granted by the license, users are provided only
#   with a limited warranty and the software's author, the holder of the
#   economic rights, and the successive licensors have only limited
#   liability.
#
#   In this respect, the user's attention is drawn to the risks associated
#   with loading, using, modifying and/or developing or reproducing the
#   software by the user in light of its specific status of free software,
#   that may mean that it is complicated to manipulate, and that also
#   therefore means that it is reserved for developers and experienced
#   professionals having in-depth computer knowledge. Users are therefore
#   encouraged to load and test the software's suitability as regards their
#   requirements in conditions enabling the security of their systems and/or
#   data to be ensured and, more generally, to use and operate it in the same
#   conditions as regards security.
#
#   The fact that you are presently reading this means that you have had
#   knowledge of the CeCILL license and that you accept its terms.


# AX_C_CXX_ENABLE_DEBUG_OPTION([EXTRA_HELP_MESSAGE], [FORCE-ENABLE-TEST], [FORCE-DISABLE-TEST])
AC_DEFUN([AX_C_CXX_ENABLE_DEBUG_OPTION],
[dnl
AX_REQUIRE_DEFINED([AC_ARG_ENABLE])dnl
AX_REQUIRE_DEFINED([AC_DEFINE])dnl
AX_REQUIRE_DEFINED([AC_PROG_SED])dnl
AX_REQUIRE_DEFINED([AC_MSG_ERROR])dnl
AX_REQUIRE_DEFINED([AS_ECHO])dnl
AX_REQUIRE_DEFINED([AS_HELP_STRING])dnl
AX_REQUIRE_DEFINED([AS_IF])dnl
AX_REQUIRE_DEFINED([AX_APPEND_COMPILE_FLAGS])dnl
AC_REQUIRE([AC_PROG_SED])dnl

m4_define([_enable_debug_option_error_str],
          [Please correct your settings and re-run the 'configure' script.])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug[[=mode]]],
                  [Compiles without code optimization and with debug mode
                   (and enables assertion too). If 'mode' is set to "no",
                   then it is equivalent to providing '--disable-debug'
                   option, which is the default behavior (any other value
                   means to enable debug).
                   $1])])dnl

AS_IF([m4_default([$2], [false]) && m4_default([$3], [false])],
      [dnl Bad configuration since both FORCE-ENABLE-TEST and
       dnl FORCE-DISABLE-TEST are true.
       AC_MSG_ERROR([
    Your configuration implies both debug compile flags to be enabled and
    disabled.
    _enable_debug_option_error_str])],
      [dnl FORCE-ENABLE-TEST is true. Change the `enable_debug` variable to
       dnl "yes" if not set or set to "no" (since any other value means "yes").
       m4_default([$2], [false])],
      [AS_IF([test -z "${enable_debug}"],
             [enable_debug="yes"],
             [test "${enable_debug}" = "no"],
             [AC_MSG_ERROR([
    You tried to disable debug mode, but it is required by some other
    configuration.
    _enable_debug_option_error_str])])],
      [dnl FORCE-DISABLE-TEST is true. Change the `enable_debug` variable to
       dnl "no".
       m4_default([$2], [false])],
      [AS_IF([test -z "${enable_debug}"],
             [enable_debug="no"],
             [test "${enable_debug}" = "no"],
             [],
             [AC_MSG_ERROR([
    You tried to enable debug mode, but it is someother configuration
    requires this mode to be disabled.
    _enable_debug_option_error_str])])])dnl


# Remove '-UNDEBUG' or '-DNDEBUG' options from CPPFLAGS, CFLAGS and CXXFLAGS (if any)
CPPFLAGS=$(AS_ECHO(["${CPPFLAGS}"]) | ${SED} 's,-@<:@DU@:>@NDEBUG\b,,g')
CFLAGS=$(AS_ECHO(["${CFLAGS}"]) | ${SED} 's,-@<:@DU@:>@NDEBUG\b,,g')
CXXFLAGS=$(AS_ECHO(["${CXXFLAGS}"]) | ${SED} 's,-@<:@DU@:>@NDEBUG\b,,g')

# Remove '-g' option from CPPFLAGS, CFLAGS and CXXFLAGS (if any)
CPPFLAGS=$(AS_ECHO(["${CPPFLAGS}"]) | ${SED} 's,-g\b,,g')
CFLAGS=$(AS_ECHO(["${CFLAGS}"]) | ${SED} 's,-g\b,,g')
CXXFLAGS=$(AS_ECHO(["${CXXFLAGS}"]) | ${SED} 's,-g\b,,g')

# Remove '-O.' option from CPPFLAGS, CFLAGS and CXXFLAGS (if any)
CPPFLAGS=$(AS_ECHO(["${CPPFLAGS}"]) | ${SED} 's,-O@<:@^ @:>@*\b,,g')
CFLAGS=$(AS_ECHO(["${CFLAGS}"]) | ${SED} 's,-O@<:@^ @:>@*\b,,g')
CXXFLAGS=$(AS_ECHO(["${CXXFLAGS}"]) | ${SED} 's,-O@<:@^ @:>@*\b,,g')

# Remove spurious spaces from CPPFLAGS, CFLAGS and CXXFLAGS (if any)
CPPFLAGS=$(AS_ECHO(["${CPPFLAGS}"]) | ${SED} 's,\(^\|@<:@ @:>@\)@<:@ @:>@*,\1,g')
CFLAGS=$(AS_ECHO(["${CFLAGS}"]) | ${SED} 's,\(^\|@<:@ @:>@\)@<:@ @:>@*,\1,g')
CXXFLAGS=$(AS_ECHO(["${CXXFLAGS}"]) | ${SED} 's,\(^\|@<:@ @:>@\)@<:@ @:>@*,\1,g')

dnl Update the (pre)compiler flags.
AC_CONFIG_COMMANDS_PRE([
  AS_IF([test -n "${enable_debug}" -a "${enable_debug}" != "no"],
        [AX_APPEND_COMPILE_FLAGS([-UNDEBUG], [CPPFLAGS])
         AX_APPEND_COMPILE_FLAGS([-g -O0 -fno-inline], [CFLAGS])
         AX_APPEND_COMPILE_FLAGS([-g -O0 -fno-inline], [CXXFLAGS])
         AS_IF([test "${enable_debug}" != "yes"],
               [AC_DEFINE([DEBUG],[],[Enables debug])])],
        [AX_APPEND_COMPILE_FLAGS([-DNDEBUG], [CPPFLAGS])
         AX_APPEND_COMPILE_FLAGS([-O3], [CFLAGS])
         AX_APPEND_COMPILE_FLAGS([-O3], [CXXFLAGS])])
])

])dnl AX_C_CXX_ENABLE_DEBUG_OPTION
dnl Local variables:
dnl mode: autoconf
dnl fill-column: 77
dnl End:
