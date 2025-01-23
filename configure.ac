dnl Process this file with autoconf to produce a configure script.

dnl Copyright © 2024      -- LIRMM/CNRS/UM
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

m4_define([VERSION_MAJOR], [m4_esyscmd_s([./config/git-version-gen | sed 's,^[^0-9]*\([0-9]*\)\.\([0-9]*\)\.\(.*\)$,\1,'])])
m4_define([VERSION_MINOR], [m4_esyscmd_s([./config/git-version-gen | sed 's,^[^0-9]*\([0-9]*\)\.\([0-9]*\)\.\(.*\)$,\2,'])])
m4_define([VERSION_MICRO], [m4_esyscmd_s([./config/git-version-gen | sed 's,^[^0-9]*\([0-9]*\)\.\([0-9]*\)\.\(.*\)$,\3,'])])

AC_INIT([BijectHash],
        [VERSION_MAJOR.VERSION_MINOR.VERSION_MICRO],
        [bijecthash@lirmm.fr],
        [bijecthash],
        [https://github.com/cagret/bijecthash/])

PACKAGE_PRETTY_NAME="BijectHash."
AC_SUBST([PACKAGE_PRETTY_NAME])
AC_DEFINE_UNQUOTED([PACKAGE_PRETTY_NAME], ["${PACKAGE_PRETTY_NAME}"], [Define to this package "pretty" name.])

PACKAGE_DESCRIPTION="BijectHash is a tool designed for generating permutations of k-mers from DNA sequences and encoding them into unique integers using bijective hash functions."
AC_SUBST([PACKAGE_DESCRIPTION])
AC_DEFINE_UNQUOTED([PACKAGE_DESCRIPTION], ["${PACKAGE_DESCRIPTION}"], [Define to this package description.])

PACKAGE_AUTHORS="Clément AGRET <cagret@mailo.com> and Alban MANCHERON <alban.mancheron@lirmm.fr>"
AC_SUBST([PACKAGE_AUTHORS])
AC_DEFINE_UNQUOTED([PACKAGE_AUTHORS], ["${PACKAGE_AUTHORS}"], [Define to this package author(s).])

PACKAGE_LICENCE="GNU GPL v3"
AC_SUBST([PACKAGE_LICENCE])
AC_DEFINE_UNQUOTED([PACKAGE_LICENCE], ["${PACKAGE_LICENCE}"], [Define to this package licence.])

PACKAGE_COPYRIGHT_DATE="2024"
AC_SUBST([PACKAGE_COPYRIGHT_DATE])
AC_DEFINE_UNQUOTED([PACKAGE_COPYRIGHT_DATE], ["${PACKAGE_COPYRIGHT_DATE}"], [Define to this package copyright date.])

PACKAGE_LOGO=""
AC_SUBST([PACKAGE_LOGO])
AC_DEFINE_UNQUOTED([PACKAGE_LOGO], ["${PACKAGE_LOGO}"], [Define to this package logo filename.])

AC_DEFINE_UNQUOTED([BEGIN_BIJECTHASH_NAMESPACE], [namespace bijecthash {], [Starts the BijectHash namespace.])
AC_DEFINE_UNQUOTED([END_BIJECTHASH_NAMESPACE], [}], [Ends the BijectHash namespace.])

AC_COPYRIGHT([
###############################################################################
#                                                                             #
#  Copyright © 2024      -- LIRMM/CNRS/UM                                     #
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
*  Copyright © 2024      -- LIRMM/CNRS/UM                                     *
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

dnl Every other copy of the package version number gets its value from here
AM_INIT_AUTOMAKE([foreign no-texinfo.tex])
AM_SILENT_RULES([yes])

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

AC_REVISION([m4_esyscmd_s([./config/git-version-gen --timestamp --force-update configure.ac])])

ARCH=$(uname -m | sed s/i.86/i386/ | sed s/x86_64/amd64/)
AC_SUBST([ARCH])

AC_PATH_PROG([CLOC], [cloc])
AM_CONDITIONAL([CLOC], [test -n "${CLOC}"])
AC_PATH_PROG([OHCOUNT], [ohcount])
AM_CONDITIONAL([OHCOUNT], [test -n "${OHCOUNT}"])
AC_PATH_PROG([DEB_BUILDER], [dpkg])

ISODATE=$(date +%Y-%m-%d)
AC_SUBST([ISODATE])

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


dnl =========================== End of System check ===========================




dnl ============================= Library Settings ============================

m4_define([lib_version], [m4_esyscmd_s([cat ./config/version | grep "^[ ]*LIB_VERSION[ ]*:" | sed 's,^[^:]*:[ ]*\(.*\)$,\1,'])])

LIB_VERSION=lib_version

m4_define([lib_version_ext], [m4_esyscmd_s([cat ./config/version | grep "^[ ]*LIB_VERSION[ ]*:" | sed 's,^[^:]*:[ ]*\(.*\)$,\1,' | tr ':' '.'])])

LIB_VERSION_EXT=lib_version_ext

dnl LIB_VERSION is not the release number.
dnl Please read libtool documentation about versionning

LIBNAME=${PACKAGE_NAME#lib}
AC_SUBST([LIBNAME])
AC_SUBST([VERSION])
AC_SUBST([LIB_VERSION])
AC_SUBST([LIB_VERSION_EXT])
AC_DEFINE_UNQUOTED([LIB_VERSION],["${LIB_VERSION_EXT}"],[Libtool version string])

dnl ========================= End of Library Settings =========================




dnl ========================= Release enabled checking ========================

MAKE_RELEASE=true

AS_IF([test -z "${DEB_BUILDER}"],
      [MAKE_RELEASE=false
       AC_MSG_WARN([
   The 'dpkg' program could not be found.
   This isn't critical, but it means that you won't be able to build releases.
])])

AM_CONDITIONAL([MAKE_RELEASE], [${MAKE_RELEASE}])

hacked_prefix="${HACKED_PREFIX:-${prefix}}"
AS_IF([test "x${hacked_prefix}" = "xNONE"],
      [hacked_prefix=\"${ac_default_prefix}\"])
AC_SUBST([hacked_prefix])

dnl ===================== End of Release enabled checking =====================




dnl ================================= Doxygen =================================

AC_ARG_VAR([DX_DOXYGEN], [Documenation generator command (default: doxygen).])
AC_PATH_PROG([DX_DOXYGEN], [${DX_DOXYGEN:-doxygen}])

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
   The 'doxygen' program cound not be found.
   This isn't critical, but it means that you won't be able to create the
   documentation and thus to make releases.
])],
      [AS_IF([test "${DX_FLAG_doc}" -ne 1],
             [BUILD_DOC=false
              AC_MSG_WARN([
   The documentation generation is disabled, so is the ability to make releases.
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
dnl added to the configuraton files see
dnl https://www.gnu.org/software/automake/manual/html_node/Unconfigured-Subdirectories.html
dnl for more details
AM_CONDITIONAL([BUILD_DOC_MAN], [test ${DX_FLAG_man} = 1])

dnl ============================= End of Doxygen ==============================




dnl ============================== Code Coverage ==============================

dnl Add option --enable-code-coverage (default to --disable-code-coverage)
AX_CODE_COVERAGE()
AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CFLAGS}], [CFLAGS])
AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CPPFLAGS}], [CPPFLAGS])
AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CXXFLAGS}], [CXXFLAGS])
AX_APPEND_LINK_FLAGS([${CODE_COVERAGE_LIBS}], [LIBS])
AS_IF([test ${MAKE_RELEASE} = "true" -a "x${enable_code_coverage}" = "xyes"],
      [MAKE_RELEASE=false
       AC_MSG_WARN([
    The '--enable-code-coverage' option has been provided.
    If you want to produce some release, re-run the configure script without
    this option.
])])

AC_ARG_VAR([GCOVR],
           [Wrapper around gcov program to generate simple reports (default: gcovr).
            This option is used only in conjunction with '--enable-code-coverage' option.])

AC_PATH_PROG([GCOVR], [${GCOVR:-gcovr}])

GCOVR_ENABLED=false
GCOVR_TXT_ENABLED=false        # Available from any version of gcovr
GCOVR_XML_ENABLED=false        # Available from any version of gcovr
GCOVR_HTML_ENABLED=false       # Available from gcovr v3.0 but is buggy until version 6.0
GCOVR_JSON_ENABLED=false       # Available from gcovr v4.2
GCOVR_SONARQUBE_ENABLED=false  # Available from gcovr v4.2
GCOVR_COBERTURA_ENABLED=false  # Available from gcovr v4.2 (XML was already Cobertura format, v4.2 introduce explicit Cobertura format)
GCOVR_COVERALLS_ENABLED=false  # Available from gcovr v5.0
GCOVR_CSV_ENABLED=false        # Available from gcovr v5.0

AS_IF([test "x${enable_code_coverage}" = "xyes"],
      [AS_IF([test -z "${GCOVR}"],
             [AC_MSG_WARN([
    The 'gcovr' program could not be found.
    This isn't critical, but it means that you won't be able to create all code
    coverage reports.
])],
             [GCOVR_ENABLED=true
              GCOVR_TXT_ENABLED=true
              GCOVR_XML_ENABLED=true
              GCOVR_VERSION=$(${GCOVR} --version | head -n 1 | cut -d' ' -f2)
              AX_COMPARE_VERSION([${GCOVR_VERSION}], [ge], [4.2],
                                 [GCOVR_JSON_ENABLED=true
                                  GCOVR_SONARQUBE_ENABLED=true])
              AX_COMPARE_VERSION([${GCOVR_VERSION}], [ge], [5.0],
                                 [GCOVR_COVERALLS_ENABLED=true
                                  GCOVR_CSV_ENABLED=true])
              AX_COMPARE_VERSION([${GCOVR_VERSION}], [ge], [5.1],
                                 [GCOVR_COBERTURA_ENABLED=true])
              AX_COMPARE_VERSION([${GCOVR_VERSION}], [ge], [6.0],
                                 [GCOVR_HTML_ENABLED=true],
                                 [AC_MSG_WARN([
    Your version of the 'gcovr' program (v. ${GCOVR_VERSION}) is not up-to-date
    and the generation of the code coverage reports may fail or the result may
    look bad.
    This isn't critical, but it means that you won't be able to create proper
    code coverage reports.
])])])])

AM_CONDITIONAL([GCOVR_ENABLED], [${GCOVR_ENABLED}])
AM_CONDITIONAL([GCOVR_TXT_ENABLED], [${GCOVR_TXT_ENABLED}])
AM_CONDITIONAL([GCOVR_XML_ENABLED], [${GCOVR_XML_ENABLED}])
AM_CONDITIONAL([GCOVR_HTML_ENABLED], [${GCOVR_HTML_ENABLED}])
AM_CONDITIONAL([GCOVR_JSON_ENABLED], [${GCOVR_JSON_ENABLED}])
AM_CONDITIONAL([GCOVR_SONARQUBE_ENABLED], [${GCOVR_SONARQUBE_ENABLED}])
AM_CONDITIONAL([GCOVR_COBERTURA_ENABLED], [${GCOVR_COBERTURA_ENABLED}])
AM_CONDITIONAL([GCOVR_COVERALLS_ENABLED], [${GCOVR_COVERALLS_ENABLED}])
AM_CONDITIONAL([GCOVR_CSV_ENABLED], [${GCOVR_CSV_ENABLED}])

dnl ========================== End of Code Coverage ===========================




dnl ================================== Debug ==================================

dnl Add option --enable-debug (default to --disable-debug)
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug@<:@=ARG@:>@],
                              [Without argument or passing argument 'yes', compiles without
                               code optimization and with debug flag (and enables assertion too).
                               Passing argument 'print' (or 'full') does the same thing than
                               'yes' and enables printing debugging informations on output too.
                               This option is implied by '--enable-code-coverage'])])

dnl remove '-UNDEBUG' or '-DNDEBUG' option from CFLAGS, CPPFLAGS and CXXFLAGS (if any)
CFLAGS=$(AS_ECHO(["${CFLAGS}"]) | sed 's,-@<:@DU@:>@NDEBUG\b,,g')
CPPFLAGS=$(AS_ECHO(["${CPPFLAGS}"]) | sed 's,-@<:@DU@:>@NDEBUG\b,,g')
CXXFLAGS=$(AS_ECHO(["${CXXFLAGS}"]) | sed 's,-@<:@DU@:>@NDEBUG\b,,g')

dnl remove '-g' option from CFLAGS, CPPFLAGS and CXXFLAGS (if any)
CFLAGS=$(AS_ECHO(["${CFLAGS}"]) | sed 's,-g\b,,g')
CPPFLAGS=$(AS_ECHO(["${CPPFLAGS}"]) | sed 's,-g\b,,g')
CXXFLAGS=$(AS_ECHO(["${CXXFLAGS}"]) | sed 's,-g\b,,g')

dnl remove and '-O.' option from CFLAGS, CPPFLAGS and CXXFLAGS (if any)
CFLAGS=$(AS_ECHO(["${CFLAGS}"]) | sed 's,-O@<:@^ @:>@*\b,,g')
CPPFLAGS=$(AS_ECHO(["${CPPFLAGS}"]) | sed 's,-O@<:@^ @:>@*\b,,g')
CXXFLAGS=$(AS_ECHO(["${CXXFLAGS}"]) | sed 's,-O@<:@^ @:>@*\b,,g')

dnl remove spurious spaces from CFLAGS, CPPFLAGS and CXXFLAGS (if any)
CFLAGS=$(AS_ECHO(["${CFLAGS}"]) | sed 's,\(^\|@<:@ @:>@\)@<:@ @:>@*,\1,g')
CPPFLAGS=$(AS_ECHO(["${CPPFLAGS}"]) | sed 's,\(^\|@<:@ @:>@\)@<:@ @:>@*,\1,g')
CXXFLAGS=$(AS_ECHO(["${CXXFLAGS}"]) | sed 's,\(^\|@<:@ @:>@\)@<:@ @:>@*,\1,g')

AS_IF([test "x${enable_code_coverage}" = "xyes"],
      [AS_IF([test -z "${enable_debug}" -o "x${enable_debug}" = "xno"],
             [enable_debug="yes"])])

AS_IF([test -n "${enable_debug}" -a "x${enable_debug}" != "xno"],
      [AX_APPEND_COMPILE_FLAGS([-UNDEBUG], [CPPFLAGS])
       AX_APPEND_COMPILE_FLAGS([-g -O0 -fno-inline], [CXXFLAGS])
       AS_IF([test "x${enable_debug}" == "xprint" -o  "x${enable_debug}" == "xfull"],
             [AC_DEFINE([DEBUG],[],[Output (lots of) debugging informations])])],
      [AX_APPEND_COMPILE_FLAGS([-DNDEBUG], [CPPFLAGS])
       AX_APPEND_COMPILE_FLAGS([-O3], [CXXFLAGS])])

dnl ============================== End of Debug ===============================


dnl ###########################################################################
dnl #                          Checking for libraries                         #
dnl ###########################################################################


dnl ###########################################################################
dnl #                         Checking for header files                       #
dnl ###########################################################################


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
  doc/doxygen.cfg doc/mainpage.dox doc/Makefile
  extra/Makefile
  packaging/control
  packaging/libkmer-reader.pc packaging/libkmer-transformers.pc
  packaging/postinst packaging/prerm
  packaging/Makefile
  resources/Makefile
  src/Makefile
  tests/Makefile
])
AC_OUTPUT
