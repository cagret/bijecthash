dnl Process this file with autoconf to produce a configure script.

dnl Copyright © 2024-2025 -- LIRMM/CNRS/UM
dnl                          (Laboratoire d'Informatique, de Robotique et de
dnl                          Microélectronique de Montpellier /
dnl                          Centre National de la Recherche Scientifique /
dnl                          Université de Montpellier)
dnl                          CRIStAL/CNRS/UL
dnl                          (Centre de Recherche en Informatique, Signal et
dnl                          Automatique de Lille /
dnl                          Centre National de la Recherche Scientifique /
dnl                          Université de Lille)
dnl
dnl Auteurs/Authors:
dnl                  Clément AGRET      <cagret@mailo.com>
dnl                  Annie   CHATEAU    <annie.chateau@lirmm.fr>
dnl                  Antoine LIMASSET   <antoine.limasset@univ-lille.fr>
dnl                  Alban   MANCHERON  <alban.mancheron@lirmm.fr>
dnl                  Camille MARCHET    <camille.marchet@univ-lille.fr>
dnl
dnl Programmeurs/Programmers:
dnl                  Clément AGRET      <cagret@mailo.com>
dnl                  Alban   MANCHERON  <alban.mancheron@lirmm.fr>
dnl
dnl -------------------------------------------------------------------------
dnl
dnl This file is part of BijectHash.
dnl
dnl BijectHash is free software: you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by the
dnl Free Software Foundation, either version 3 of the License, or (at your
dnl option) any later version.
dnl
dnl BijectHash is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
dnl FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
dnl more details.
dnl
dnl You should have received a copy of the GNU General Public License along
dnl with BijectHash. If not, see <https://www.gnu.org/licenses/>.

AC_PREREQ([2.69])


dnl ###########################################################################
dnl #                           Project Informations                          #
dnl ###########################################################################


dnl ============================ Project Settings =============================

AX_GIT_VERSIONING_PROG_PATH([config])
AX_GIT_VERSIONING([config])

AC_INIT([BijectHash],
        [GV_MAJOR.GV_MINOR.GV_MICRO],
        [bijecthash@lirmm.fr],
        [bijecthash],
        [https://github.com/cagret/bijecthash/])

AX_PACKAGE_INFOS(
  [PRETTY_NAME:BijectHash.],
  [SHORT_DESCRIPTION:BijectHash is a tool designed for generating permutations of k-mers from DNA sequences and encoding them into unique integers using bijective hash functions.],
  [FULL_DESCRIPTION:TODO],
  [AUTHORS:Clément AGRET <cagret@mailo.com> and Alban MANCHERON <alban.mancheron@lirmm.fr>],
  [LICENSE:GNU GPL v3],
  [COPYRIGHT_DATE:2024-2025])dnl

AC_REVISION([GV_REVISION])

AC_DEFINE_UNQUOTED([BEGIN_BIJECTHASH_NAMESPACE], [namespace bijecthash {], [Starts the BijectHash namespace.])
AC_DEFINE_UNQUOTED([END_BIJECTHASH_NAMESPACE], [}], [Ends the BijectHash namespace.])

AC_COPYRIGHT([
###############################################################################
#                                                                             #
#  Copyright © 2024-2025 -- LIRMM/CNRS/UM                                     #
#                           (Laboratoire d'Informatique, de Robotique et de   #
#                           Microélectronique de Montpellier /                #
#                           Centre National de la Recherche Scientifique /    #
#                           Université de Montpellier)                        #
#                           CRIStAL/CNRS/UL                                   #
#                           (Centre de Recherche en Informatique, Signal et   #
#                           Automatique de Lille /                            #
#                           Centre National de la Recherche Scientifique /    #
#                           Université de Lille)                              #
#                                                                             #
#  Auteurs/Authors:                                                           #
#                   Clément AGRET      <cagret@mailo.com>                     #
#                   Annie   CHATEAU    <annie.chateau@lirmm.fr>               #
#                   Antoine LIMASSET   <antoine.limasset@univ-lille.fr>       #
#                   Alban   MANCHERON  <alban.mancheron@lirmm.fr>             #
#                   Camille MARCHET    <camille.marchet@univ-lille.fr>        #
#                                                                             #
#  Programmeurs/Programmers:                                                  #
#                   Clément AGRET      <cagret@mailo.com>                     #
#                   Alban   MANCHERON  <alban.mancheron@lirmm.fr>             #
#                                                                             #
#  -------------------------------------------------------------------------  #
#                                                                             #
#  This file is part of BijectHash.                                           #
#                                                                             #
#  BijectHash is free software: you can redistribute it and/or modify it      #
#  under the terms of the GNU General Public License as published by the      #
#  Free Software Foundation, either version 3 of the License, or (at your     #
#  option) any later version.                                                 #
#                                                                             #
#  BijectHash is distributed in the hope that it will be useful, but WITHOUT  #
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      #
#  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   #
#  more details.                                                              #
#                                                                             #
#  You should have received a copy of the GNU General Public License along    #
#  with BijectHash. If not, see <https://www.gnu.org/licenses/>.              #
#                                                                             #
###############################################################################
])

AC_CONFIG_SRCDIR([src/biject_hash.cpp])

AC_CONFIG_AUX_DIR([config])

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AC_CONFIG_HEADERS([config/config.h])
AH_TOP([
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

#ifndef __BIJECTHASH_CONFIG_H__
#define __BIJECTHASH_CONFIG_H__
])
AH_BOTTOM([
#endif
])

AC_CONFIG_MACRO_DIR([config/m4])

dnl ========================= End of Project Settings =========================


dnl ###########################################################################
dnl #                Checking System and Configuration Options                #
dnl ###########################################################################


dnl ============================== System check ===============================

AC_CANONICAL_HOST

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CPP
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_AWK
AC_PROG_LN_S
AC_PROG_EGREP
AC_PROG_SED
PKG_PROG_PKG_CONFIG([0.25])

AC_SUBST([DEBIAN_PACKAGE_DESCRIPTION], [" ${PACKAGE_FULL_DESCRIPTION//$'\n'/\\n }"])
AX_DEBIAN_PACKAGE([packaging/control],
                  [PRERM:packaging/prerm],
                  [POSTINST:packaging/postinst])

AC_LANG([C++])


dnl Ensure compiler is C++17 compatible
AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])

dnl Force some extra flags if not already present
AX_APPEND_COMPILE_FLAGS([-Wall -Wextra -pedantic -funroll-loops], [CXXFLAGS])

dnl Using libtool
LT_INIT
LT_LANG([C++])
LT_PATH_LD
AX_COMPILER_FLAGS_LDFLAGS([EXTRA_LDFLAGS])
AX_APPEND_LINK_FLAGS([${EXTRA_LDFLAGS}], [LDFLAGS])

dnl Using POSIX threads
AX_PTHREAD([AX_APPEND_COMPILE_FLAGS([${PTHREAD_CFLAGS}], [CXXFLAGS])
            AX_APPEND_LINK_FLAGS([${PTHREAD_LIBS}], [LDFLAGS])
            CXX="${PTHREAD_CXX}"],
            [AC_MSG_ERROR([This program requires POSIX threads but your ${CXX} compiler seems not to support them.])])

dnl Automake/Makefile settings
AM_INIT_AUTOMAKE([foreign no-texinfo.tex])
AM_SILENT_RULES([yes])

dnl =========================== End of System check ===========================


dnl ============================= Library Settings ============================

LIB_VERSION=GV_LIB_VERSION
LIB_VERSION_EXT=GV_LIB_VERSION_EXT

dnl LIB_VERSION is not the release number.
dnl Please read libtool documentation about versionning

LIBNAME=${PACKAGE_NAME#lib}
AC_SUBST([LIBNAME])
AC_SUBST([VERSION])
AC_SUBST([LIB_VERSION])
AC_SUBST([LIB_VERSION_EXT])
AC_DEFINE_UNQUOTED([LIB_VERSION], ["GV_LIB_VERSION_EXT"], [Libtool version string])

dnl ========================= End of Library Settings =========================


dnl ============================= Code Statistics =============================

AX_CODE_STATISTICS()

dnl ========================= End of code statistics ==========================


dnl ================================= Doxygen =================================

AC_ARG_VAR([DX_DOXYGEN],
           [Documentation generator command (default: doxygen).])dnl

DX_MIN_VERSION=1.9.6

DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(ON)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([${PACKAGE_PRETTY_NAME}], [doxygen.cfg], [docs])

AS_IF([test -z "${DX_DOXYGEN}"],
      [BUILD_DOC=false
       AC_MSG_WARN([
   The 'doxygen' program could not be found or documentation generation has
   been disable by providing the '--disable-doxygen-doc' option to the
   configure script.
   This isn't critical, but it means that you won't be able to create the
   documentation.
])],
      [AS_IF([test "${DX_FLAG_doc}" -ne 1],
             [BUILD_DOC=false
              AC_MSG_WARN([
   The documentation generation is disabled.
])],
             [BUILD_DOC=true
              DX_DOXYGEN_VERSION=`${DX_DOXYGEN} --version | cut -d' ' -f1`
              AX_COMPARE_VERSION([${DX_DOXYGEN_VERSION}],[lt],[${DX_MIN_VERSION}],
                                 [AC_MSG_WARN([
   Your version of the 'doxygen' program is quite old (v. ${DX_DOXYGEN_VERSION}).
   The generation of the documentation may fail or the result may look bad.
   This isn't critical, but it means that you won't be able to create a proper
   API documentation and thus to make correct releases.
])])])])

AM_CONDITIONAL([BUILD_DOC], [${BUILD_DOC}])
dnl Whatever the value of BUILD_DOC, the doc subdirectory need to be
dnl added to the configuration files see
dnl https://www.gnu.org/software/automake/manual/html_node/Unconfigured-Subdirectories.html
dnl for more details.
dnl The `doc/doxygen.cfg` and `doc/Makefile` targets are added to the `AC_CONFIG_FILES()` macro arguments at the end of this file
AM_CONDITIONAL([BUILD_DOC_MAN], [test ${DX_FLAG_man} = 1])

dnl ============================= End of Doxygen ==============================




dnl ============================== Code Coverage ==============================

dnl All formats of code coverage reports supported by the gcovr (until
dnl version 7.2) are enabled by default.

dnl Set the verbosity level of AX_GCOVR_CODE_COVERAGE() and associated
dnl macros to INFO (default is WARN).
GCOVR_AC_VERBOSITY(INFO)

dnl Apply a regex replacement in `*.gcov` files to merge files having
dnl different paths due to symlinks when built in some subdirectory.
dnl This is ugly, but it does the job...
GCOVR_GCOV_FILE_SED_COMMAND_ON_CUSTOM_BUILDDIR([s,0:Source:../\$(LIBNAME)/,0:Source:\$(top_srcdir)/../src/,])

dnl Disable TEXT/XML/JSON/SONARQUBE/COVERALLS/CSV/LEGACY code coverage report
dnl by default (by setting an empty target file). This will add
dnl `--enable-code-coverage-*format*[=target]` options in your
dnl configure script, for *format* in text, xml, sonarqube and
dnl coveralls.

dnl GCOVR_SUMMARY_TARGET()
GCOVR_TEXT_TARGET()
GCOVR_XML_TARGET()
dnl GCOVR_HTML_TARGET()
GCOVR_JSON_TARGET()
GCOVR_SONARQUBE_TARGET()
GCOVR_COVERALLS_TARGET()
GCOVR_CSV_TARGET()
dnl GCOVR_COBERTURA_TARGET()
GCOVR_JACOCO_TARGET()
GCOVR_CLOVER_TARGET()
GCOVR_LEGACY_HTML_TARGET()

dnl For the other *format*s the cnfigure script will be added the
dnl `--disable-code-coverage-*format*` option.

dnl Don't need to call the AX_CODE_COVERAGE() since it will be done by
dnl the AX_GCOVR_CODE_COVERAGE() macro
AX_GCOVR_CODE_COVERAGE()

dnl The macro AX_GCOVR_CODE_COVERAGE() will add the
dnl `--enable-code-coverage` and the `--with-gcov=GCOV` options (see
dnl AX_CODE_COVERAGE() macro documentation) and the new
dnl `--with-gcovr=GCOVR` option in configure script to provide a
dnl custom GCOVR program. The GCOVR environment variable can also be
dnl used to define the GCOVR program if not explicitly set.

dnl Finally, append the computed compiler/linker flags to enable the
dnl code coverage computation when code coverage is enabled.
AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CFLAGS}], [CFLAGS])
AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CPPFLAGS}], [CPPFLAGS])
AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CXXFLAGS}], [CXXFLAGS])
AX_APPEND_LINK_FLAGS([${CODE_COVERAGE_LIBS}], [LIBS])

dnl ========================== End of Code Coverage ===========================


dnl ================================== Debug ==================================

AX_C_CXX_ENABLE_DEBUG_OPTION([Passing argument 'print' does the same thing than 'yes' and
                              enables printing debugging informations on output too.
                              This option is implied by '--enable-code-coverage'],
                             [test "x${enable_code_coverage}" = "xyes"])

AS_CASE(["${enable_debug}"],
        [""], [enable_debug=no],
        [yes], [],
        [no],  [],
        [print], [],
        [AC_MSG_WARN([Value '${enable_debug}' is not valid for the '--enable-debug' option. Assuming 'yes'])
         enable_debug=yes])

dnl ============================== End of Debug ===============================


dnl ###########################################################################
dnl #                          Checking for libraries                         #
dnl ###########################################################################


dnl ================================ Sphinx++ =================================

AX_EXTERNAL_INCLUDED_CXX_LIBRARY_DETECTION([sphinxpp],
                                           [[external/Sphinxpp-library],
                                            [-I\$(top_builddir)/external/Sphinxpp-library],
                                            [-L\$(top_builddir)/external/Sphinxpp-library/src -lsphinx++]],
                                           [libsphinx++])

dnl Don't use neither AX_APPEND_COMPILE_FLAGS() nor
dnl AX_APPEND_LINK_FLAGS() since:
dnl
dnl   1. The AX_EXTERNAL_INCLUDED_CPP_HEADER_DETECTION(), the
dnl      AX_EXTERNAL_INCLUDED_C_LIBRARY_DETECTION() and the
dnl      AX_EXTERNAL_INCLUDED_CXX_LIBRARY_DETECTION() macros already
dnl      check for validity when using already installed
dnl      library/headers.
dnl
dnl   2. If using the included library/headers, the
dnl      (pre)compiler/linker flags may not be correctly appended
dnl      since the library/header is most probably not already
dnl      available
dnl
dnl An acceptable option is to update the (pre)compiler/linker by
dnl appending the computed flags if no more test involving compilation
dnl is executed hereafter.
dnl
dnl A more secure (and recommended) strategy is to defer the
dnl compiler/linker flag updates at the end of the configure script,
dnl before generating the Makefiles using AC_CONFIG_COMMANDS_PRE()

dnl Append only the Sphinx++ (pre)ompiler flags to the default ones.
AC_CONFIG_COMMANDS_PRE([CXXFLAGS="${CXXFLAGS} ${sphinxpp_CXXFLAGS}"])

dnl The linker flages are not automatically added since only the
dnl libkmer-transformers need to be linked with the Sphinx++
dnl library. So the sphinxpp_LDFLAGS variable is exported in order to
dnl to be used in Makefiles (typically for the libkmer-transformers.la
dnl target).
AC_SUBST([sphinxpp_LDFLAGS])

dnl Disable releases if using the included Sphinx++ library.
AS_IF([test "x${with_included_sphinxpp}" = "xyes"],
      [MAKE_RELEASE=false
       AC_MSG_WARN([
   You are using the included Sphinx++ library.
   This is fine for us, but it means that you won't be able to build correct releases.
])])

dnl ============================= End of Sphinx++ =============================

dnl ###########################################################################
dnl #                         Checking for header files                       #
dnl ###########################################################################

AC_CHECK_INCLUDES_DEFAULT

AC_CHECK_HEADERS([\
    stdint.h \
    sys/ioctl.h \
    unistd.h\
  ], [], [AC_MSG_ERROR([Header file '${ac_header}' not found.])])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

dnl Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([\
  memset pow \
  strerror strtoul \
])


dnl ###########################################################################
dnl #               Producing files according to configuration                #
dnl ###########################################################################


AC_CONFIG_FILES([
  Makefile
  coverage/Makefile
  doc/doxygen.cfg doc/mainpage.dox doc/Makefile
  extra/Makefile
  packaging/libkmer-reader.pc packaging/libkmer-transformers.pc
  packaging/Makefile
  resources/Makefile
  src/Makefile
  src/transformers/Makefile
  src/transformers/basic/Makefile
  src/transformers/extra/Makefile
  tests/Makefile
])
AC_OUTPUT
