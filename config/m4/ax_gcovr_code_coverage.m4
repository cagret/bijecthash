# ===========================================================================
#  https://https://gite.lirmm.fr/doccy-dev-tools/autoconf
# ===========================================================================
#
# Serial 5
#
# SYNOPSIS
#
#   `AX_GCOVR_CODE_COVERAGE()`
#
#   `GCOVR_SUMMARY_TARGET(FILE)`      (default target is `summary.txt`)
#   `GCOVR_TEXT_TARGET(FILE)`         (default target is `report.txt`)
#   `GCOVR_XML_TARGET(FILE)`          (default target is `report.xml`)
#   `GCOVR_HTML_TARGET(FILE)`         (default target is `html/index.html`)
#   `GCOVR_JSON_TARGET(FILE)`         (default target is `report.json`)
#   `GCOVR_SONARQUBE_TARGET(FILE)`    (default target is `sonarqube-report.xml`)
#   `GCOVR_COVERALLS_TARGET(FILE)`    (default target is `coveralls-report.json`)
#   `GCOVR_CSV_TARGET(FILE)`          (default target is `report.csv`)
#   `GCOVR_COBERTURA_TARGET(FILE)`    (default target is `cobertura-report.xml`)
#   `GCOVR_JACOCO_TARGET(FILE)`       (default target is `jacoco-report.xml`)
#   `GCOVR_CLOVER_TARGET(FILE)`       (default target is `clover-report.xml`)
#   `GCOVR_LEGACY_HTML_TARGET(FILE)`  (default target is `$(CODE_COVERAGE_OUTPUT_FILE)`)
#
#   `GCOVR_AC_VERBOSITY(INFO|WARN|OFF) (default is `WARN`)
#   `GCOVR_GCOV_FILE_SED_COMMAND_ON_CUSTOM_BUILDDIR(CMD)` (default is empty)
#
# DESCRIPTION
#
#   The `AX_GCOVR_CODE_COVERAGE()` macro enables and automates code coverage
#   reports generation using `gcovr` and extends the `AX_CODE_COVERAGE()`
#   macro.
#
#   TL;DR: jump to the two examples below.
#
#   The `AX_GCOVR_CODE_COVERAGE()` macro defines the
#   `@GCOVR_CODE_COVERAGE_RULES@` autoconf substitution variable that defines
#   all the rules needed to generates the code coverage reports (you
#   essentially should be interested in the `coverage`, ~`coverage-all`~,
#   `coverage-<format>`, `coverage-clean-<format>`, `coverage-clean` and
#   `coverage-distclean` targets, which are quite explicit).
#
#   In order to enable automatic generation on `make check` (for example) and
#   cleaning on `make clean` and `make disctlean`, one can add the following
#   line in the `Makefile.am` file:
#
#       check-local: coverage
#       clean-local: coverage-clean
#       distclean-local: coverage-distclean
#
#   For each supported `<FORMAT>`, a `GCOVR_<FORMAT>_TARGET()` allows to set
#   a custom file. If the file has a directory component in its name (like
#   for `HTML` format), then it should be a relative subdirectory. This is
#   highly recommended for `HTML` since lots of files are produced. Setting
#   an empty target for some `<FORMAT>` simply disable the format by default
#   in the configure script. Anyway, users can still enable the disabled
#   `<format>`s using `--enable-code-coverage-<format>` configure script
#   option or disable the enabled ones using the
#   `--disable-code-coverage-<format>` configure script option. Not all
#   formats are necessarily available, according to the version of the
#   `gcovr` installed program:
#     - Formats available from any version of `gcovr`:
#       - `SUMMARY`
#       - `TEXT`
#       - `XML`
#     - Format available from version 3.0 of `gcovr`:
#       - `HTML` (buggy until version 6.0)
#     - Formats available from version 4.2 of `gcovr`:
#       - `JSON`
#       - `SONARQUBE`
#     - Formats available from version 5.0 of `gcovr`:
#       - `COVERALLS`
#       - `CSV`
#     - Format available from version 5.1 of `gcovr`:
#       - `COBERTURA` (notice that `XML` format IS `COBERTURA`, but the
#         `COBERTURA` explicit format was introduced in v4.2)
#     - Format available from version 7.0 of `gcovr`:
#       - `JACOCO`
#     - Format available from version 7.1 of `gcovr`:
#       - `CLOVER`
#
#   Concerning the `LEGACY_HTML` format, things are a little bit different
#   since its only a wrapper around the report generation of
#   `AX_CODE_COVERAGE()`. So the `LEGACY_HTML` format can be disabled by
#   setting an empty target, but setting a non empty target just sets the
#   `CODE_COVERAGE_OUTPUT_FILE` and `CODE_COVERAGE_OUTPUT_DIRECTORY` makefile
#   variables (notice that if those variables are directly set, this should
#   work as expected).
#
#   The `GCOVR_AC_VERBOSITY(INFO|WARN|OFF)` set the level of the code
#   coverage configuration.
#
#   The code coverage reports consider symlink files are not the same as the
#   linked ones. This is *a priori* not really a problem except if you use a
#   symlink to some directory for accessing your public headers in your test
#   programs AND if you are building your program in a subdirectory (which is
#   the case for the `make distcheck` target). In order to solve this
#   problem, The `GCOVR_GCOV_FILE_SED_COMMAND_ON_CUSTOM_BUILDDIR(CMD)` allows
#   to apply a regex replacement in `*.gcov` files. This is ugly (this is why
#   the macro name too is), but it does the job.
#
#   Some Makefile variables can be defined in order to customize code
#   coverage reports:
#
#      - The legacy HTML report generation (if enabled) takes care of the
#        following Makefile variables inherited and used only by the
#        `AX_CODE_COVERAGE()` macro (please, refer to this macro
#        documentation for more details):
#        - Unchanged from the `AX_CODE_COVERAGE()` macro:
#          - `CODE_COVERAGE_DIRECTORY`
#            Default: `$(top_builddir)`
#          - `CODE_COVERAGE_LCOV_SHOPTS_DEFAULT`
#            Default: based on `$(CODE_COVERAGE_BRANCH_COVERAGE)`
#          - `CODE_COVERAGE_LCOV_SHOPTS`
#            Default: `$(CODE_COVERAGE_LCOV_SHOPTS_DEFAULT)`
#          - `CODE_COVERAGE_LCOV_OPTIONS_GCOVPATH`
#            Default: `--gcov-tool path-to-gcov`
#          - `CODE_COVERAGE_LCOV_OPTIONS_DEFAULT`
#            Default: `$(CODE_COVERAGE_LCOV_OPTIONS_GCOVPATH)`
#          - `CODE_COVERAGE_LCOV_OPTIONS`
#            Default: `$(CODE_COVERAGE_LCOV_OPTIONS_DEFAULT)`
#          - `CODE_COVERAGE_LCOV_RMOPTS`
#            Default: `$(CODE_COVERAGE_LCOV_RMOPTS_DEFAULT)`
#          - `CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT`
#            Default: based on `$(CODE_COVERAGE_BRANCH_COVERAGE)`
#          - `CODE_COVERAGE_GENHTML_OPTIONS`
#            Default: `$(CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT)`
#          - `CODE_COVERAGE_IGNORE_PATTERN`
#            Default: empty
#        - Modified compared to the `AX_CODE_COVERAGE()` macro:
#          - `CODE_COVERAGE_OUTPUT_FILE`
#            New default: `$(GCOVR_CODE_COVERAGE_TOP_DIRECTORY)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage-legacy-html.info`
#          - `CODE_COVERAGE_OUTPUT_DIRECTORY`
#            New default: `$(GCOVR_CODE_COVERAGE_TOP_DIRECTORY)/legacy-html`
#          - `CODE_COVERAGE_BRANCH_COVERAGE`
#            New default: `1`
#          - `CODE_COVERAGE_LCOV_RMOPTS_DEFAULT`
#            New default: `"/usr/" "$(GCOVR_TESTS_DIRECTORY)"`
#      - The `gcovr` based report generation (if enabled) takes care of the
#        following Makefile variables (see 'Usage advanced example'):
#        - `CODE_COVERAGE_TOP_OUTPUT_DIRECTORY:`
#          This variable defines the top directory of all code coverage
#          reports.
#          New default: `$(builddir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage`
#        - `GCOVR_EXTRA_DEFAULT_TARGETS`
#          Extra targets to build when `make coverage` or `make coverage-all`
#          are invoked (if code coverage is enabled).
#          Default: empty
#        - `GCOVR_EXTRA_AVAILABLE_TARGETS`
#          Extra targets to build when `make coverage-all` is invoked (if
#          code coverage is enabled)
#          Default: empty
#        - `GCOVR_TESTS_DIRECTORY`
#          The build directory of tests programs.
#          Default: `$(top_builddir)/tests`
#        - `GCOVR_CHECK_TARGET_READY`
#          The target file to build in order to assume that test programs
#          were run.
#          Default: `$(GCOVR_TESTS_DIRECTORY)/test-suite.log`
#        - `GCOVR_CORE_OPTS`
#          The options to apply to the `gcovr` program whatever the output
#          format is. Default is to exclude the directory containing test
#          programs and to assume that source files are located in the `src`
#          directory (notice that unreachable branches are excluded from the
#          coverage computation).
#          Default: `--root "$(top_builddir)" --exclude-unreachable-branches --exclude '^$(GCOVR_TESTS_DIRECTORY)/' --filter '^(.*/)?src/'`
#        - `GCOVR_<FORMAT>_EXTRA_OPTIONS`
#          For all formats (except legacy HTML), it is possible to pass some
#          extra options in the `gvovr` command line by defining the
#          associated Makefile variable.
#
#
#   Usage (very) basic example:
#
#   - In your `configure.ac` file:
#
#       dnl ============================== Code Coverage ==============================
#       
#       AX_GCOVR_CODE_COVERAGE()
#       AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CFLAGS}], [CFLAGS])
#       AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CPPFLAGS}], [CPPFLAGS])
#       AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CXXFLAGS}], [CXXFLAGS])
#       AX_APPEND_LINK_FLAGS([${CODE_COVERAGE_LIBS}], [LIBS])
#       
#       dnl ========================== End of Code Coverage ===========================
#
#   - In your `Makefile.am` file:
#
#       #################
#       # CODE COVERAGE #
#       #################
#       
#       @GCOVR_CODE_COVERAGE_RULES@
#       
#       include $(top_srcdir)/aminclude_static.am
#       
#       clean-local: coverage-clean
#       
#       distclean-local: coverage-distclean
#       
#       ########################
#       # END OF CODE COVERAGE #
#       ########################
#
#
#   Usage advanced (and mostly complete) example:
#
#   - In your `configure.ac` file:
#
#       dnl ============================== Code Coverage ==============================
#       
#       dnl All formats of code coverage reports supported by the gcovr
#       dnl (until version 7.2) are enabled by default.
#       
#       dnl Set the verbosity level of AX_GCOVR_CODE_COVERAGE() and
#       dnl associated macros to INFO (default is WARN).
#       GCOVR_AC_VERBOSITY(INFO)
#       
#       dnl Apply a regex replacement in `*.gcov` files to merge files having
#       dnl different paths due to symlinks when built in some subdirectory.
#       dnl This is ugly, but it does the job...
#       GCOVR_GCOV_FILE_SED_COMMAND_ON_CUSTOM_BUILDDIR([s,0:Source:../\$(LIBNAME)/,0:Source:\$(top_builddir)/../src/,])
#       
#       dnl Change the default target for JSON report (`report.json`) to
#       dnl `code-coverage.json`
#       GCOVR_JSON_TARGET([code-coverage.json])
#       
#       dnl Disable TEXT/XML/SONARQUBE/COVERALLS code coverage report by
#       dnl default (by setting an empty target file). This will add
#       dnl `--enable-code-coverage-*format*[=target]` options in your
#       dnl configure script, for *format* in text, xml, sonarqube and
#       dnl coveralls.
#       GCOVR_TEXT_TARGET()
#       GCOVR_XML_TARGET()
#       GCOVR_SONARQUBE_TARGET()
#       GCOVR_COVERALLS_TARGET()
#       
#       dnl For the other *format*s the cnfigure script will be added the
#       dnl `--disable-code-coverage-*format*` option.
#       
#       dnl Don't need to call the AX_CODE_COVERAGE() since it will be done
#       dnl by the AX_GCOVR_CODE_COVERAGE() macro
#       AX_GCOVR_CODE_COVERAGE()
#       
#       dnl The macro AX_GCOVR_CODE_COVERAGE() will add the
#       dnl `--enable-code-coverage` and the `--with-gcov=GCOV` options (see
#       dnl AX_CODE_COVERAGE() macro documentation) and the new
#       dnl `--with-gcovr=GCOVR` option in configure script to provide a
#       dnl custom GCOVR program. The GCOVR environment variable can also be
#       dnl used to define the GCOVR program if not explicitly set.
#       
#       dnl Finally, append the computed compiler/linker flags to enable the
#       dnl code coverage computation when code coverage is enabled.
#       AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CFLAGS}], [CFLAGS])
#       AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CPPFLAGS}], [CPPFLAGS])
#       AX_APPEND_COMPILE_FLAGS([${CODE_COVERAGE_CXXFLAGS}], [CXXFLAGS])
#       AX_APPEND_LINK_FLAGS([${CODE_COVERAGE_LIBS}], [LIBS])
#       
#       dnl ========================== End of Code Coverage ===========================
#
#   - In your `Makefile.am` file:
#
#       #################
#       # CODE COVERAGE #
#       #################
#       
#       # Extra targets to build when `make coverage` or `make coverage-all`
#       # are invoked (if code coverage is enabled).
#       GCOVR_EXTRA_DEFAULT_TARGETS = Hello
#       
#       # Extra targets to build when `make coverage-all` is invoked (if code
#       # coverage is enabled)
#       GCOVR_EXTRA_AVAILABLE_TARGETS = World
#       
#       # The build directory of tests programs.
#       GCOVR_TESTS_DIRECTORY = $(top_builddir)/my-tests
#       
#       # The target file to build in order to assume that test programs were
#       # run.
#       GCOVR_CHECK_TARGET_READY = $(GCOVR_TESTS_DIRECTORY)/my-test-suite.log
#       
#       # The options to apply to the GCOVR program whatever the output
#       # format is. Here, we want to exclude the directory containing test
#       # programs and we assume that source files are located in the `src`
#       # directory as in this parameter default value. We add the assumption
#       # that every external library/headers used in the project are located
#       # into some `external` subdirectory, then they must be excluded from
#       # the code coverage reports.
#       GCOVR_CORE_OPTS = --root "$(top_builddir)" --exclude-unreachable-branches --exclude '^$(GCOVR_TESTS_DIRECTORY)/' --exclude '^(.*/)?external/' --filter '^(.*/)?src/'
#       
#       # Change the color theme of the HTML code coverage report
#       GCOVR_HTML_EXTRA_OPTIONS = --html-theme blue
#       
#       # The 'Hello' and the 'World' target (respectively invoked with the
#       # 'coverage' and 'coverage-all' targets) just display a very helpful
#       # message.
#       Hello World:
#       	@echo "Everything begins with $@"
#       
#       @GCOVR_CODE_COVERAGE_RULES@
#       
#       # Ignore (and suppress) `unused` and `deprecated` warnings emitted by
#       # the `lcov` program.
#       CODE_COVERAGE_LCOV_SHOPTS = $(CODE_COVERAGE_LCOV_SHOPTS_DEFAULT) --ignore-errors unused,unused,deprecated,deprecated
#       # Ignore (and suppress) `deprecated` warnings emitted by the `genhtml`
#       # program.
#       CODE_COVERAGE_GENHTML_OPTIONS = $(CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT) --ignore-errors deprecated,deprecated
#       
#       include $(top_srcdir)/aminclude_static.am
#       
#       clean-local: coverage-clean
#       
#       distclean-local: coverage-distclean
#       
#       ########################
#       # END OF CODE COVERAGE #
#       ########################
#
#
# LICENSE
#
#   Copyright © 2023-2025 -- LIRMM / CNRS / UM
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


dnl Coding conventions: m4 macros are prefixed by '_gcovr_' and are
dnl lower-snakecased. Internal shell variables are prefixed by 'gcovr_' and
dnl are lower-snakecased too. Shell variable that are substituted or used in
dnl Makefiles are prefixed by 'GCOVR_' and are upper-snakecased.

dnl _gcovr_do_notify(MSG)
m4_define([_gcovr_do_notify], [dnl
  AS_IF([test "${gcovr_enabled}" = "true"], [AC_MSG_NOTICE([[[GCOVR]] $1])])
])dnl _gcovr_notification


dnl _gcovr_do_warn(MSG)
m4_define([_gcovr_do_warn], [dnl
  AS_IF([test "${gcovr_enabled}" = "true"], [AC_MSG_WARN([[[GCOVR]] $1])])
])dnl _gcovr_notification


dnl _gcovr_ignore(MSG)
m4_define([_gcovr_ignore], [:m4_ignore($@)])dnl _gcovr_ignore


# GCOVR_AC_VERBOSITY(INFO|WARN|OFF)
AC_DEFUN([GCOVR_AC_VERBOSITY], [dnl
m4_case([$1],
  [INFO], [dnl
    m4_copy_force([_gcovr_do_warn], [_gcovr_warn])
    m4_copy_force([_gcovr_do_notify], [_gcovr_notification])
  ],
  [WARN], [dnl
    m4_copy_force([_gcovr_do_warn], [_gcovr_warn])
    m4_copy_force([_gcovr_ignore], [_gcovr_notification])
  ],
  [OFF], [dnl
    m4_copy_force([_gcovr_ignore], [_gcovr_warn])
    m4_copy_force([_gcovr_ignore], [_gcovr_notification])
  ],
  [dnl
    m4_fatal([Unknown value '$1' for macro 'GCOVR_AC_VERBOSITY()' (authorized values are: INFO, WARN, OFF)])
  ])dnl
])dnl GCOVR_AC_VERBOSITY
GCOVR_AC_VERBOSITY([WARN])dnl


# GCOV_FILE_SED_COMMAND_ON_CUSTOM_BUILDDIR(CMD)
AC_DEFUN([GCOVR_GCOV_FILE_SED_COMMAND_ON_CUSTOM_BUILDDIR], [dnl
m4_define([_gcov_file_sed_command_on_custom_builddir], [$1])dnl
])dnl GCOVR_GCOV_FILE_SED_COMMAND_ON_CUSTOM_BUILDDIR
GCOVR_GCOV_FILE_SED_COMMAND_ON_CUSTOM_BUILDDIR([])dnl


dnl _gcovr_tolower(FMT)
m4_define([_gcovr_tolower], [m4_translit([$1], [_A-Z], [-a-z])])dnl _gcovr_tolower_

dnl Clear any previously stored (un)available formats
m4_set_delete([_gcovr_available_formats])dnl


dnl _gcovr_format_init(FMT, FILE)
m4_define([_gcovr_format_init], [
m4_define([_gcovr_target_$1], [$2])dnl
m4_ifnblank([$2], [dnl
# Declaration of gcovr_target_$1
gcovr_target_$1="$2"
# Enable $1 format
gcovr_enabled_$1="yes"], [dnl
# Disable $1 format
gcovr_enabled_$1="no"])])dnl _gcovr_format_init


dnl _gcovr_specific_options([FMT], [OPTIONS])
dnl Macro to add the given GCOVR options to generate report in the given format.
m4_define([_gcovr_specific_options], [dnl
  m4_define([_gcovr_specific_opts_$1], [$2]) dnl
])dnl _gcovr_specific_options


dnl _gcovr_clean_specific_options([FMT], [OPTIONS])
dnl Macro to add the given GCOVR options to clean report in the given format.
m4_define([_gcovr_clean_specific_options], [dnl
  m4_define([_gcovr_clean_specific_opts_$1], [$2]) dnl
])dnl _gcovr_clean_specific_options


dnl _gcovr_add_format_option(FMT)
m4_define([_gcovr_add_format_option], [dnl
  m4_define([_lc_arg], code-coverage-_gcovr_tolower([$1]))dnl
  m4_define([_msg], [generation of _gcovr_pretty_name_$1 code coverage report[]_gcovr_extra_description_$1.])dnl
  AC_ARG_ENABLE(_lc_arg,
                [AS_HELP_STRING(m4_ifblank(_gcovr_target_$1,
                                           [--enable-[]_lc_arg],
                                           [--disable-[]_lc_arg]),
                                m4_ifblank(_gcovr_target_$1,
                                           [Enable _msg],
                                           [Disable _msg]))],
                [m4_define([_gcovr_macro_name], [GCOVR_[]$1[]_TARGET])
                 AS_CASE(["${enableval}"],
                         [yes], _gcovr_macro_name[](_gcovr_default_target_$1),
                         [no], _gcovr_macro_name[](),
                         _gcovr_macro_name[]([${enableval}]))])dnl
])dnl _gcovr_add_format_option

dnl _gcovr_add_format(FMT, TARGET, [MIN-VERSION], [MIN-RECOMMENDED-VERSION])
dnl Macro to add the given format.
m4_define([_gcovr_add_format], [dnl
  dnl Define the default value of the target file
  m4_define([_gcovr_default_target_$1], [$2])dnl
  dnl Define the initial value of the target file
  m4_define([_gcovr_target_$1], [$2])dnl
  dnl Initialize the corresponding shell variable(s) with the default value(s)
  m4_divert_text([DEFAULTS], [_gcovr_format_init([$1], [$2])])dnl
  dnl Define the format minimal version of gcovr.
  m4_define([_gcovr_min_version_$1], [m4_default([$3], [0])])dnl
  dnl Define the format minimal recommender version of gcovr.
  m4_define([_gcovr_min_recommended_version_$1], [m4_default([$4], [_gcovr_min_version_$1])])
  dnl Append the format to the set of available formats.
  m4_set_add([_gcovr_available_formats], [$1])dnl
  dnl Empty specific target options is an error
  _gcovr_specific_options([$1], [dnl
    m4_fatal([There is a bug in the current package for format '$1'. Please contact the author(s) of this file to describe that bug by copying this message.])])dnl
  dnl Empty specific target options is the default
  _gcovr_clean_specific_options([$1], [])dnl
  dnl Define a default pretty name which is the given format.
  m4_define([_gcovr_pretty_name_$1], [$1])dnl
  dnl Define an extra description which is empty.
  m4_define([_gcovr_extra_description_$1], [])dnl
])dnl

dnl +--------------------------------------------+
dnl | Initialization of GCOVR supported formats. |
dnl +--------------------------------------------+


### Formats supported by any GCOVR version.

# GCOVR_SUMMARY_TARGET(FILE)
AC_DEFUN([GCOVR_SUMMARY_TARGET], [_gcovr_format_init([SUMMARY], [$1])])dnl
_gcovr_add_format([SUMMARY], [summary.txt])dnl
_gcovr_specific_options([SUMMARY], [dnl
--print-summary --output \"\$(@).tmp\" > \"\$(@)\"
${TAB}\$(AM_V${EMPTY}_at)rm -f  \"\$(@).tmp\"])dnl
m4_define([_gcovr_pretty_name_SUMMARY], [Summary (text)])

# GCOVR_TEXT_TARGET(FILE)
AC_DEFUN([GCOVR_TEXT_TARGET], [_gcovr_format_init([TEXT], [$1])])dnl
_gcovr_add_format([TEXT], [report.txt])dnl
_gcovr_specific_options([TEXT], [--output \"\$(@)\"])dnl
m4_define([_gcovr_pretty_name_TEXT], [Text])

# GCOVR_XML_TARGET(FILE)
AC_DEFUN([GCOVR_XML_TARGET], [_gcovr_format_init([XML], [$1])])dnl
_gcovr_add_format([XML], [report.xml])dnl
_gcovr_specific_options([XML], [--xml --xml-pretty --output \"\$(@)\"])dnl


### Formats supported since version 3.0

# GCOVR_HTML_TARGET(FILE)
AC_DEFUN([GCOVR_HTML_TARGET], [_gcovr_format_init([HTML], [$1])])dnl
_gcovr_add_format([HTML], [html/index.html], [3.0], [6.0])dnl
_gcovr_specific_options([HTML], [dnl
--html --html-details --html-title \"GCOVR - Code Coverage Report\" dnl
--merge-mode-functions strict dnl
--output \"\$(@)\"])dnl
_gcovr_clean_specific_options([HTML], [dnl
${TAB}  rm -f \"\$(CODE_COVERAGE_HTML_OUTPUT_TARGET:.html=.*.html)\"; \\
${TAB}  rm -f \"\$(CODE_COVERAGE_HTML_OUTPUT_TARGET:.html=.css)\"; \\
])


### Format supported since version 4.2

# GCOVR_JSON_TARGET(FILE)
AC_DEFUN([GCOVR_JSON_TARGET], [_gcovr_format_init([JSON], [$1])])dnl
_gcovr_add_format([JSON], [report.json], [4.2])dnl
_gcovr_specific_options([JSON], [--json --json-pretty --output \"\$(@)\"])dnl

# GCOVR_SONARQUBE_TARGET(FILE)
AC_DEFUN([GCOVR_SONARQUBE_TARGET], [_gcovr_format_init([SONARQUBE], [$1])])dnl
_gcovr_add_format([SONARQUBE], [sonarqube-report.xml], [4.2])dnl
_gcovr_specific_options([SONARQUBE], [--sonarqube --output \"\$(@)\"])
m4_define([_gcovr_pretty_name_SONARQUBE], [SonarQube (XML)])


### Format supported since version 5.0

# GCOVR_COVERALLS_TARGET(FILE)
AC_DEFUN([GCOVR_COVERALLS_TARGET], [_gcovr_format_init([COVERALLS], [$1])])dnl
_gcovr_add_format([COVERALLS], [coveralls-report.json], [5.0])dnl
_gcovr_specific_options([COVERALLS], [--coveralls --coveralls-pretty --output \"\$(@)\"])dnl
m4_define([_gcovr_pretty_name_COVERALLS], [Coveralls (JSON)])

# GCOVR_CSV_TARGET(FILE)
AC_DEFUN([GCOVR_CSV_TARGET], [_gcovr_format_init([CSV], [$1])])dnl
_gcovr_add_format([CSV], [report.csv], [5.0])dnl
_gcovr_specific_options([CSV], [--csv --output \"\$(@)\"])dnl


### Format supported since version 5.1

# GCOVR_COBERTURA_TARGET(FILE)
AC_DEFUN([GCOVR_COBERTURA_TARGET], [_gcovr_format_init([COBERTURA], [$1])])dnl
_gcovr_add_format([COBERTURA], [cobertura-report.xml], [5.1])dnl
_gcovr_specific_options([COBERTURA], [--cobertura --cobertura-pretty --output \"\$(@)\"])dnl
m4_define([_gcovr_pretty_name_COBERTURA], [Cobertura (XML)])
m4_define([_gcovr_extra_description_COBERTURA], [ (this is the same format as the default XML)])


### Format supported since version 7.0

# GCOVR_JACOCO_TARGET(FILE)
AC_DEFUN([GCOVR_JACOCO_TARGET], [_gcovr_format_init([JACOCO], [$1])])dnl
_gcovr_add_format([JACOCO], [jacoco-report.xml], [7.0])dnl
_gcovr_specific_options([JACOCO], [--jacoco --jacoco-pretty --output \"\$(@)\"])dnl
m4_define([_gcovr_pretty_name_JACOCO], [JaCoCo (XML)])


### Format supported since version 7.1

# GCOVR_CLOVER_TARGET(FILE)
AC_DEFUN([GCOVR_CLOVER_TARGET], [_gcovr_format_init([CLOVER], [$1])])dnl
_gcovr_add_format([CLOVER], [clover-report.xml], [7.1])dnl
_gcovr_specific_options([CLOVER], [--clover --clover-pretty --output \"\$(@)\"])dnl
m4_define([_gcovr_pretty_name_CLOVER], [Clover (XML)])


### The LEGACY_HTML format is not handled by the GCOVR program but uses lcov
### and genhtml.

# GCOVR_LEGACY_HTML_TARGET(FILE)
AC_DEFUN([GCOVR_LEGACY_HTML_TARGET], [dnl
m4_ifblank([$1],
           [_gcovr_format_init([LEGACY_HTML], [])],
           [_gcovr_format_init([LEGACY_HTML], [\$(CODE_COVERAGE_OUTPUT_FILE)])
            m4_if($1, [\$(CODE_COVERAGE_OUTPUT_FILE)], [], [
CODE_COVERAGE_OUTPUT_FILE=$(AS_BASENAME([$1]))

CODE_COVERAGE_OUTPUT_DIRECTORY=$(AS_DIRNAME([$1]))
])])])dnl GCOVR_LEGACY_HTML_TARGET
_gcovr_add_format([LEGACY_HTML], [\$(CODE_COVERAGE_OUTPUT_FILE)])dnl
m4_define([_gcovr_pretty_name_LEGACY_HTML], [Legacy HTML])
m4_define([_gcovr_extra_description_LEGACY_HTML], [ (generated without GCOVR)])


dnl +---------------------------------------------------+
dnl | End of initialization of GCOVR supported formats. |
dnl +---------------------------------------------------+


dnl _gcovr_check_handled_format(FMT) Check if the given format is handled by
dnl the current gcovr program version (which must be available in the stored
dnl _gcovr_version variable).
m4_define([_gcovr_check_handled_format], [dnl
  gcovr_available_$1="yes"
  AX_COMPARE_VERSION([${gcovr_version}], [ge], [_gcovr_min_version_$1],
    [dnl Check if the gcovr program version meets the recommended minimal version.
     AX_COMPARE_VERSION([${gcovr_version}], [lt], [_gcovr_min_recommended_version_$1],
       [AS_IF([test "${gcovr_enabled_$1}" = "yes"],
         [dnl Emit a warning because the current version doesn't meet the
          dnl recommended version.
          _gcovr_warn([Format $1 badly supported
    Your version of the '${GCOVR}' program (v. ${gcovr_version}) is not up-to-date
    and badly support the $1 format for code coverage reports.
    This isn't critical, but it means that your report generation may crash or be far from reality.])])])],
    [AS_IF([test "${gcovr_enabled_$1}" = "yes"],
       [dnl Emit a warning because the current version doesn't meet the minimal
        dnl version.
        _gcovr_warn([Format $1 not supported
    Your version of the '${GCOVR}' program (v. ${gcovr_version}) is not up-to-date
    and doesn't support the $1 format for code coverage reports.
    This isn't critical, but it means that you won't be able to create proper
    code coverage reports.])])
     dnl Disable format in code report generation.
     gcovr_available_$1="no"
     GCOVR_$1_TARGET()])dnl
])dnl _gcovr_check_handled_format


dnl _gcovr_update_enabled_formats() Update enabled formats
dnl (gcovr_enabled_formats shell variable).  and really available formats
dnl according to gcovr program version (gcovr_available_formats shell
dnl variable).
m4_define([_gcovr_update_enabled_formats], [dnl
  gcovr_enabled_formats=
  gcovr_available_formats=
  AS_IF([test "${gcovr_enabled}" = "true"],
    [m4_set_foreach([_gcovr_available_formats], [_fmt], [
       _gcovr_check_handled_format(_fmt)
       AS_IF([test "${gcovr_enabled_[]_fmt}" = "yes"],
         [_gcovr_notification([Format _fmt is enabled])dnl
          gcovr_enabled_formats+=" _fmt"
          gcovr_available_formats+=" _fmt"],
         [AS_IF([test "${gcovr_available_[]_fmt}" = "yes"],
            [_gcovr_notification([Format _fmt is disabled])dnl
             gcovr_available_formats+=" _fmt"],
            [_gcovr_notification([Format _fmt is not available])])])])])dnl
])dnl _gcovr_update_enabled_formats


dnl Global Makefile rules for code coverage reports (preamble)
m4_define([_gcovr_rule_preamble],
          [dnl
           gcovr_rules="
###############################################################################
#                                Code Coverage                                #
###############################################################################

.PHONY: \\
m4_set_foreach([_gcovr_available_formats], [_fmt], [dnl
  coverage-_gcovr_tolower(_fmt) coverage-clean-_gcovr_tolower(_fmt) \\
])dnl
  coverage-dir coverage-clean-dir \\
  coverage coverage-all \\
  coverage-clean coverage-distclean \\
  coverage-always-build coverage-gcov-files
"
           AS_IF([test "${gcovr_enabled}" = "true"],
                 [dnl GCOVR code reports are enabled
                  gcovr_rules+="
GCOVR_CODE_COVERAGE_TOP_DIRECTORY ?= \$(builddir)/\$(PACKAGE_NAME)-\$(PACKAGE_VERSION)-coverage
"
                  AS_IF([test "${gcovr_available_LEGACY_HTML}" = "yes"],
                        [dnl Legacy HTML report is enabled (handling this format separately)
                         gcovr_rules+="
########################
# Legacy Code Coverage #
########################

# These settings can be overriden by specifying explicit values in your
# Makefile.am or by providing them as environment variables.
CODE_COVERAGE_OUTPUT_FILE          ?= \$(GCOVR_CODE_COVERAGE_TOP_DIRECTORY)/legacy-html.info
CODE_COVERAGE_OUTPUT_DIRECTORY     ?= \$(GCOVR_CODE_COVERAGE_TOP_DIRECTORY)/legacy-html
CODE_COVERAGE_BRANCH_COVERAGE      ?= 1
CODE_COVERAGE_LCOV_RMOPTS_DEFAULT  ?= \"/usr/*\" \$(GCOVR_TESTS_DIRECTORY)

code-coverage-capture-hook: coverage-dir

coverage-legacy-html:
${TAB}\$(AM_V${EMPTY}_at)\$(MAKE) \$(AM_MAKEFLAGS) code-coverage-capture abs_builddir=\"\$(abs_top_builddir)\"

coverage-clean-legacy-html:
${TAB}@if test -f \"\$(CODE_COVERAGE_OUTPUT_FILE)\"; then \\
${TAB}  \$(AM_V${EMPTY}_P) && echo \"Cleaning code coverage _gcovr_pretty_name_LEGACY_HTML report\" || true; \\
${TAB}  rm -f \"\$(CODE_COVERAGE_OUTPUT_FILE)\"; \\
${TAB}fi
${TAB}@if test \"\$(CODE_COVERAGE_OUTPUT_DIRECTORY)\" != \"\$(GCOVR_CODE_COVERAGE_TOP_DIRECTORY)\" -a -d \"\$(CODE_COVERAGE_OUTPUT_DIRECTORY)\"; then \\
${TAB}  rm -rf \"\$(CODE_COVERAGE_OUTPUT_DIRECTORY)\"; \\
${TAB}fi
"
])
                  gcovr_rules+="

#######################
# GCOVR code coverage #
#######################

GCOVR_EXTRA_DEFAULT_TARGETS   ?=
GCOVR_EXTRA_AVAILABLE_TARGETS ?=
GCOVR_TESTS_DIRECTORY         ?= \$(abs_top_builddir)/tests
GCOVR_CHECK_TARGET_READY      ?= \$(GCOVR_TESTS_DIRECTORY)/test-suite.log

coverage: \\
  coverage-dir \\
$(for fmt in ${gcovr_enabled_formats,,}; do echo "  coverage-${fmt//_/-} \\"; done)
  \$(GCOVR_EXTRA_DEFAULT_TARGETS)

coverage-all: \\
  coverage-dir \\
$(for fmt in ${gcovr_available_formats,,}; do echo "  coverage-${fmt//_/-} \\"; done)
  \$(GCOVR_EXTRA_DEFAULT_TARGETS) \\
  \$(GCOVR_EXTRA_AVAILABLE_TARGETS)

coverage-distclean: code-coverage-dist-clean

.NOTPARALLEL:

gcovr_code_coverage = \$(gcovr_code_coverage_${AM_V})
gcovr_code_coverage_ = \$(gcovr_code_coverage_${AM_DEFAULT_VERBOSITY})
gcovr_code_coverage_0 = @echo \"  GCOVR   \" \$(@);
gcovr_code_coverage_1 =

GCOVR_CORE_OPTS ?=${TAB}${TAB}${TAB}\\
  --root \"\$(top_builddir)\"${TAB}${TAB}\\
  --exclude-unreachable-branches${TAB}\\
  --exclude '^\$(GCOVR_TESTS_DIRECTORY)/'${TAB}${TAB}\\
  --filter '^(.*/)?src/'

GCOVR_OPTS =${TAB}${TAB}${TAB}\\
  --gcov-executable \"\$(GCOV)\"${TAB}\\
  \$(GCOVR_CORE_OPTS)${TAB}${TAB}\\
  --use-gcov-files --keep

### Ensure tests were run

# Fake target to force some rule to be applied
coverage-always-build:

# rule
\$(GCOVR_CHECK_TARGET_READY): coverage-always-build
${TAB}\$(AM_V${EMPTY}_at)\$(MAKE) \$(AM_MAKEFLAGS) \$(@F) -C \$(@D)
dnl Need to add the ${EMPTY} unset variable in order automake to split the
dnl AM_V_at string which provokes automake failure.

coverage-gcov-files:
${TAB}\$(gcovr_code_coverage)\$(GCOVR) \$(GCOVR_CORE_OPTS) --keep
m4_ifnblank(m4_defn([_gcov_file_sed_command_on_custom_builddir]), [dnl
${TAB}\$(AM_V${EMPTY}_at)test \"\$(abs_top_builddir)\" = \"\$(abs_top_srcdir)\" \\
${TAB}|| find \$(top_builddir) \\
${TAB}  -name \"*.gcov\" \\
${TAB}  -exec \$(SED) -i '_gcov_file_sed_command_on_custom_builddir' \"{}\" \\;
])dnl

### Create coverage report directory

# named target and rule

coverage-dir: \$(GCOVR_CHECK_TARGET_READY)
${TAB}\$(AM_V${EMPTY}_at)\$(MAKE) \$(AM_MAKEFLAGS) coverage-gcov-files
${TAB}\$(AM_V${EMPTY}_at)\$(MKDIR_P) \"\$(GCOVR_CODE_COVERAGE_TOP_DIRECTORY)\"
dnl Need to add the ${EMPTY} unset variable in order automake to split the
dnl AM_V_at string which provokes automake failure.

coverage-clean-dir:
${TAB}\$(A${EMPTY}M_V_at)if test -d \"\$(GCOVR_CODE_COVERAGE_TOP_DIRECTORY)\"; then \\
${TAB}  echo \"Removing any existing coverage directory\"; \\
${TAB}  rmdir \"\$(GCOVR_CODE_COVERAGE_TOP_DIRECTORY)\"; \\
${TAB}fi

coverage-clean: \\
m4_set_foreach([_gcovr_available_formats], [_fmt], [dnl
  coverage-clean-_gcovr_tolower(_fmt) \\
])dnl
  code-coverage-clean
${TAB}\$(AM_V${EMPTY}_at)find \$(top_builddir) -name \"*.gcov\" -delete
${TAB}\$(AM_V${EMPTY}_at)\$(MAKE) \$(AM_MAKEFLAGS) coverage-clean-dir
dnl Need to add the ${EMPTY} unset variable in order automake to split the
dnl AM_V_at string which provokes automake failure.

### Generate all coverage reports
"
],               [dnl GCOVR code reports are disabled
                  gcovr_rules+="
m4_set_foreach([_gcovr_available_formats], [_fmt], [dnl
coverage-_gcovr_tolower(_fmt) coverage-clean-_gcovr_tolower(_fmt) \\
])dnl
coverage-dir coverage-clean-dir \\
coverage coverage-all \\
code-coverage-clean code-coverage-dist-clean \\
coverage-always-build coverage-gcov-files:
${TAB}@echo
${TAB}@echo \"=====================================================================\"
${TAB}@echo
${TAB}@echo \"GCOVR code coverage reports are disabled (so is target '\$(@)').\"
${TAB}@echo \"Consider re-run the configure script using the\"
${TAB}@echo \"'--enabled-code-coverage' (and eventually '--with-gcovr=<path>'\"
${TAB}@echo \"option(s) in order to make enable gcovr reports).\"
${TAB}@echo
${TAB}@echo \"=====================================================================\"
${TAB}@echo
${TAB}@false

coverage-clean coverage-distclean:
"
])])dnl _gcovr_rule_preamble


dnl _gcovr_rule_add_format(FMT)
dnl Add makefile rule according to the format status except if it is the
dnl LEGACY_HTML format (which is already handled)
m4_define([_gcovr_rule_add_format],
  [m4_if([$1], [LEGACY_HTML],
         [],
         [dnl The format is not LEGACY_HTML, thus adding Makefile rules
          AS_IF([test "${gcovr_enabled}" = "true"],
                [dnl Code coverage is enabled
                 gcovr_rules+="

#### $1 code coverage report

# named target for $1
"
                 m4_define([_output_target], [CODE_COVERAGE_[]$1[]_OUTPUT_TARGET])dnl
                 m4_define([_rule_target], [coverage-_gcovr_tolower([$1])])dnl
                 m4_define([_clean_rule_target], [coverage-clean-_gcovr_tolower([$1])])
                 AS_IF([test "${gcovr_available_$1}" = "yes"],
                       [dnl If format is available
                        gcovr_rules+="
_output_target ?= \$(GCOVR_CODE_COVERAGE_TOP_DIRECTORY)/${gcovr_target_$1}

GCOVR_$1_EXTRA_OPTIONS ?=

_rule_target: \$(_output_target)

_clean_rule_target:
${TAB}@if test -f \"\$(_output_target)\"; then \\
${TAB}  \$(AM_V${EMPTY}_P) && echo \"Cleaning code coverage _gcovr_pretty_name_$1 report\" || true; \\
${TAB}  rm -f \"\$(_output_target)\"; \\
_gcovr_clean_specific_opts_$1()dnl
${TAB}fi
${TAB}@d=\$\$(dirname \"\$(_output_target)\"); \\
${TAB}if test \"\$\${d}\" != \"\$(GCOVR_CODE_COVERAGE_TOP_DIRECTORY)\" -a -d \"\$\${d}\"; then \\
${TAB}  rm -rf \"\$\${d}\"; \\
${TAB}fi

# rule for $1 final target
\$(_output_target): \$(GCOVR_CHECK_TARGET_READY) | coverage-dir
${TAB}@\$(AM_V${EMPTY}_P) && echo \"Generating code coverage $1 report \$(_output_target)\" || true
${TAB}@d=\$\$(dirname \"\$(_output_target)\"); \\
${TAB}if test \"\$\${d}\" != \".\" -a ! -d \"\$\${d}\"; then \\
${TAB}  echo \"\$\${d}\"; \$(MKDIR_P) \"\$\${d}\"; \\
${TAB}fi
${TAB}\$(gcovr_code_coverage)\$(GCOVR) \$(GCOVR_OPTS) \$(GCOVR_$1_EXTRA_OPTIONS) _gcovr_specific_opts_$1
${TAB}@\$(AM_V${EMPTY}_P) && echo \"=> Code coverage $1 report is available at '\$(@)'.\" || true
dnl Need to add the ${EMPTY} unset variable in order automake to split the
dnl AM_V_at and AM_V_P strings which provoke automake failure.
"],
                       [dnl If format is not available
                        gcovr_rules+="
_rule_target:
${TAB}@echo
${TAB}@echo \"=====================================================================\"
${TAB}@echo
${TAB}@echo \"Code coverage _gcovr_pretty_name_$1 report is not available.\"
${TAB}@echo \"You are currently using '${GCOVR} v${gcovr_version}'.\"
${TAB}@echo \"To be able to generate _gcovr_pretty_name_$1 report, please upgrade your gcovr\"
${TAB}@echo \"program to version _gcovr_min_version_$1 (or greater) and re-run the configure script.\"
${TAB}@echo
${TAB}@echo \"=====================================================================\"
${TAB}@echo

_clean_rule_target:
"])])dnl

])])dnl _gcovr_rule_add_format

dnl Global Makefile rules for code coverage reports (postamble)
m4_define([_gcovr_rule_postamble], [dnl
gcovr_rules+="
###############################################################################
#                             End of Code Coverage                            #
###############################################################################
"
])dnl _gcovr_rule_postamble


# AX_GCOVR_CODE_COVERAGE()
AC_DEFUN([AX_GCOVR_CODE_COVERAGE],
[dnl
AX_REQUIRE_DEFINED([AC_ARG_ENABLE])dnl
AX_REQUIRE_DEFINED([AC_ARG_VAR])dnl
AX_REQUIRE_DEFINED([AC_MSG_NOTICE])dnl
AX_REQUIRE_DEFINED([AC_MSG_WARN])dnl
AX_REQUIRE_DEFINED([AC_PATH_PROG])dnl
AX_REQUIRE_DEFINED([AC_PROG_SED])dnl
AX_REQUIRE_DEFINED([AC_SUBST])dnl
AX_REQUIRE_DEFINED([AM_SUBST_NOTMAKE])dnl
AX_REQUIRE_DEFINED([AM_CONDITIONAL])dnl
AX_REQUIRE_DEFINED([AS_HELP_STRING])dnl
AX_REQUIRE_DEFINED([AS_IF])dnl
AX_REQUIRE_DEFINED([AX_CODE_COVERAGE])dnl
AX_REQUIRE_DEFINED([AX_COMPARE_VERSION])dnl
AC_REQUIRE([AC_PROG_SED])dnl

# Disable gcovr code report generation by default
gcovr_enabled=false

dnl Then use the legacy m4 macros to check for available programs and define
dnl the compiler/linker flags. Just definitely disable the rule generation since there a.
dnl m4_define([_AX_CODE_COVERAGE_RULES], [])dnl
AX_CODE_COVERAGE()dnl

m4_set_map([_gcovr_available_formats], [_gcovr_add_format_option])

dnl Declare the GCOVR environment variable.
AC_ARG_VAR([GCOVR], [
Wrapper around gcov program to generate simple reports (default: gcovr).
This variable is used only in conjunction with '--enable-code-coverage' option.
])dnl

AS_IF([test "x${enable_code_coverage}" = "xyes"],
      [gcovr_enabled=true])dnl

dnl If code coverage is enabled, check what format are supported according to
dnl the 'gcovr' program.
AS_IF([test "${gcovr_enabled}" = "true"],
      [dnl Check for some available gcovr program.
       AC_PATH_PROG([GCOVR], [${GCOVR:-gcovr}])
       AS_IF([test -z "${GCOVR}"],
             [_gcovr_warn([Missing GCOVR program
    The 'gcovr' program could not be found.
    This isn't critical, but it means that you won't be able to create all code
    coverage reports.])],
             [dnl The gcovr program exists.
              dnl Compute the gcovr program version
              gcovr_version=$(${GCOVR} --version | head -n 1 | cut -d' ' -f2)
              dnl Update the set of enabled formats
              _gcovr_update_enabled_formats()])])

AS_IF([test "${gcovr_enabled}" = "true" -a -z "${gcovr_available_formats}"],
      [dnl
       _gcovr_warn([You have enabled code coverage but all formats are disabled.
    Please check your configure script options.])
       gcovr_enabled="false"])dnl

dnl Computes makefile rules and export them in GCOVR_CODE_COVERAGE_RULES
dnl autoconf substituted variable.
TAB=$'\t'
_gcovr_rule_preamble
m4_set_map([_gcovr_available_formats], [_gcovr_rule_add_format])
_gcovr_rule_postamble
AC_SUBST([GCOVR_CODE_COVERAGE_RULES], [${gcovr_rules}])dnl
AM_SUBST_NOTMAKE([GCOVR_CODE_COVERAGE_RULES])dnl

dnl Define an Automake conditional variable
AM_CONDITIONAL([GCOVR_CODE_COVERAGE_ENABLED], [test "${gcovr_enabled}" = "true"])dnl

])dnl AX_GCOVR_CODE_COVERAGE
dnl Local variables:
dnl mode: autoconf
dnl fill-column: 77
dnl End:
