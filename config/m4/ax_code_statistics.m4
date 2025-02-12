# ===========================================================================
#  https://https://gite.lirmm.fr/doccy-dev-tools/autoconf
# ===========================================================================
#
# Serial 3
#
# SYNOPSIS
#
#   `AX_CODE_STATISTICS()`
#
# DESCRIPTION
#
#
#   The `AX_CODE_STATISTICS()` macro enables and automates computation of
#   code statistics using `cloc` and 'ohcount' programs.
#
#   The `AX_CODE_STATISTICS()` macro adds the `--enable-code-statistics`
#   option to the configure script. This option checks for the `cloc` and
#   `ohcount` availability on the system (using first the content of the CLOC
#   and OHCOUNT environment variables if set or by searching the PATH
#   environment variable for the tools).
#
#   This autoconf macro also defines the `@CODE_STATISTICS_RULES@` autoconf
#   substitution variable that defines all the rules needed to generates the
#   code statistics (you essentially should be interested in the `stats`,
#   `clean-stats`, `cloc` and `ohcount` targets, which are quite explicit).
#
#   If code statistics are enabled (default) and tools are found, then a
#   complete report can be generated. If some tool is missing, then the
#   generated report is a message that indicates the tool was missing. If the
#   code statistics are disabled, then all named target leads to displayng an
#   error message and causes `make` to fail.
#
#   In order to enable automatic generation on `make` (for example) and
#   cleaning on `make clean`, one can add the following line in the
#   `Makefile.am` file:
#
#       all-local: stats
#       clean-local: clean-stats
#
#   If you wan't to distribute satistics with your package, you can add the
#   following line in your `Makefile.am`:
#
#       dist_pkgdata_DATA = $(STATS_FILE)
#
#   If you generate your code statistics in some subdirectory (*i.e.*, the
#   `resources` subdirectory), then it is not enough to distribute them. In
#   such case, you have to add in your top directory `Makefile.am` the
#   following lines:
#
#       STATS_FILE = resources/$(PACKAGE_NAME)-$(PACKAGE_VERSION).stats
#       BUILT_SOURCES = $(STATS_FILE)
#
#       $(STATS_FILE):
#       	$(AM__at)$(MAKE) $(AM_MAKEFLAGS) $(@F) -C $(@D)
#
#   You also can set custom values for the following Makefile variables :
#
#     - `STATS_FILE_BASENAME`
#        Default: `$(PACKAGE_NAME)-$(PACKAGE_VERSION)`
#     - `CLOC_FILE`
#       Default: `$(STATS_FILE_BASENAME).cloc`
#     - `OHCOUNT_FILE`
#       Default: `$(STATS_FILE_BASENAME).ohcount`
#     - LIST_FILE`
#       Default: `$(STATS_FILE_BASENAME).filelist`
#     - `STATS_FILE`
#       Default: $(STATS_FILE_BASENAME).stats`
#
#   Usage example:
#
#   - In your `configure.ac` file:
#
#       dnl ============================= Code Statistics =============================
#       
#       AX_CODE_STATISTICS()
#       
#       dnl ========================= End of code statistics ==========================
#
#
#   - In your `Makefile.am` file:
#
#       ###################
#       # CODE STATISTICS #
#       ###################
#
#       dist_pkgdata_DATA = $(STATS_FILE)
#       
#       @CODE_STATISTICS_RULES@
#       
#       all-local: stats
#       clean-local: clean-stats
#
#       ##########################
#       # END OF CODE STATISTICS #
#       ##########################
#
#
# LICENSE
#
#   Copyright © 2017-2025 -- LIRMM / CNRS / UM
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


dnl _code_statistics_check_tool(tool, [var])
dnl If var is not given, then uses an uppercase sanitized version of the
dnl given tool (e.g., _code_statistics_check_tool([my-tool]) sets the shell
dnl variable MY_TOOL to the absolute path of my-tool if found (empty
dnl otherwise).
m4_define([_code_statistics_check_tool], [dnl
  m4_define([_var_name], m4_default([$2], [m4_translit([$1], [-+.a-z], [___A-Z])]))dnl
  dnl check for <tool> program
  AC_PATH_PROG(_var_name, [$1])
  dnl Declare the <var> environment variable.
  AC_ARG_VAR(_var_name, [Path to the '$1' program.])dnl
  AS_IF([test -z "${_var_name}"],
        [AC_MSG_WARN([Missing $1 program
    The '$1' program could not be found.
    This isn't critical, but it means that you won't be able to create all code statistics.dnl
])])])dnl


dnl Global Makefile rules for code statistics
m4_define([_code_statistics_rules],
          [dnl
code_statistics_rules="
####################
# Default settings #
####################

STATS_FILE_BASENAME ?= \$(PACKAGE_NAME)-\$(PACKAGE_VERSION)
CLOC_FILE ?= \$(STATS_FILE_BASENAME).cloc
OHCOUNT_FILE ?= \$(STATS_FILE_BASENAME).ohcount
LIST_FILE ?= \$(STATS_FILE_BASENAME).filelist
STATS_FILE ?= \$(STATS_FILE_BASENAME).stats

"

AS_IF([test "${enable_code_statistics}" = "yes"], [dnl
code_statistics_rules+="
############################################
# List of files used to compute statistics #
############################################

# named target
.PHONY: filelist
filelist: \$(LIST_FILE)

git_ls_tree = \$(git_ls_tree_${AM_V})
git_ls_tree_ = \$(git_ls_tree_${AM_DEFAULT_VERBOSITY})
git_ls_tree_0 = @echo \"  GIT-LS-TREE \" \$(@);
git_ls_tree_1 =

# rule
\$(LIST_FILE):
${TAB}@\$(AM_V${EMPTY}_P) && echo \"Generate package file list\" || true
${TAB}\$(git_ls_tree)(cd  \"\$(top_srcdir)\" \\
${TAB}    && git ls-tree --format='\$(top_srcdir)/%(path)' -r HEAD) \\
${TAB}  > \"\$(@)\" || true


#####################################
# CLOC [Count Lines of Code] report #
#####################################

# named target
.PHONY: cloc
cloc: \$(CLOC_FILE)

cloc = \$(cloc_${AM_V})
cloc_ = \$(cloc_${AM_DEFAULT_VERBOSITY})
cloc_0 = @echo \"  CLOC    \" \$(@);
cloc_1 =

# rule
"

AS_IF([test -n "${CLOC}"],
      [code_statistics_rules+="
\$(CLOC_FILE): \$(LIST_FILE)
${TAB}@\$(AM_V${EMPTY}_P) && echo \"Generate cloc summary\" || true
${TAB}\$(AM_V${EMPTY}_at)echo \"CLoC summary\" | \$(SED) 'h;s/./=/g;p;x;p;x' > \"\$(@)\"
${TAB}\$(AM_V${EMPTY}_at)echo >> \"\$(@)\"
${TAB}\$(cloc)\$(CLOC) --list-file=\"\$<\" --quiet | tail -n +2 >> \"\$(@)\"
"],   [code_statistics_rules+="
\$(CLOC_FILE):
${TAB}@echo \"You should install cloc (source code line counter) and reconfigure this package.\"
${TAB}\$(AM_V${EMPTY}_at)echo \"CLoC isn't installed. No report created\" > \"\$(@)\"
"])dnl

code_statistics_rules+="
###########################################
# Ohloh's source code line counter report #
###########################################

# named target
.PHONY: ohcount
ohcount: \$(OHCOUNT_FILE)

ohcount = \$(ohcount_${AM_V})
ohcount_ = \$(ohcount_${AM_DEFAULT_VERBOSITY})
ohcount_0 = @echo \"  OHCOUNT \" \$(@);
ohcount_1 =

# rule
"

AS_IF([test -n "${OHCOUNT}"],
      [code_statistics_rules+="
\$(OHCOUNT_FILE): \$(LIST_FILE)
${TAB}@\$(AM_V${EMPTY}_P) && echo \"Generate OhCount summary\" || true
${TAB}\$(AM_V${EMPTY}_at)echo \"OhCount summary\" | \$(SED) 'h;s/./=/g;p;x;p;x' > \"\$(@)\"
${TAB}\$(ohcount)\$(OHCOUNT) \$\$(cat \"\$<\") | tail -n +2 >> \"\$(@)\"
"],   [code_statistics_rules+="
\$(OHCOUNT_FILE): \$(LIST_FILE)
${TAB}@echo \"You should install ohcount (source code line counter) and reconfigure this package.\"
${TAB}\$(AM_V${EMPTY}_at)echo \"OhCount isn't installed. No report created\" > \"\$(@)\"
"])dnl

code_statistics_rules+="

#################
# Global report #
#################

# named target
.PHONY: stats update-stats clean-stats

stats: \$(STATS_FILE)

update-stats:
${TAB}\$(AM_V${EMPTY}_at)\$(MAKE) \$(AM_MAKEFLAGS) clean
${TAB}\$(AM_V${EMPTY}_at)\$(MAKE) \$(AM_MAKEFLAGS) stats

stats = \$(stats_${AM_V})
stats_ = \$(stats_${AM_DEFAULT_VERBOSITY})
stats_0 = @echo \"  STATS   \" \$(@);
stats_1 =

# rule
\$(STATS_FILE): \$(CLOC_FILE) \$(OHCOUNT_FILE) \$(LIST_FILE)
${TAB}@\$(AM_V${EMPTY}_P) && echo \"Backup existing code statistics (if any)\" || true
${TAB}\$(AM_V${EMPTY}_at)test ! -f \"\$(abs_srcdir)/\$(STATS_FILE)\" || cat \"\$(abs_srcdir)/\$(STATS_FILE)\" > \"\$(STATS_FILE).tmp\"
${TAB}\$(stats)echo \"Generate code statistics\"
${TAB}\$(AM_V${EMPTY}_at)echo \"# \$(@) #\" | \$(SED) 'h;s/./#/g;p;x;p;x' > \"\$(@)\"
${TAB}\$(AM_V${EMPTY}_at)echo >> \"\$(@)\"
${TAB}\$(AM_V${EMPTY}_at)cat \$(CLOC_FILE) >> \"\$(@)\"
${TAB}\$(AM_V${EMPTY}_at)echo >> \"\$(@)\"
${TAB}\$(AM_V${EMPTY}_at)cat \$(OHCOUNT_FILE) >> \"\$(@)\"
${TAB}\$(AM_V${EMPTY}_at)echo >> \"\$(@)\"
${TAB}\$(AM_V${EMPTY}_at)printf \"%.78d\n\" | tr 0 - >> \"\$(@)\"
${TAB}\$(AM_V${EMPTY}_at)echo >> \"\$(@)\"
${TAB}\$(AM_V${EMPTY}_at)if test ! -s \"\$(LIST_FILE)\" -a -f \"\$(STATS_FILE).tmp\"; then \\
${TAB}  echo \"Using existing code statistics backup file since generation failed\"; \\
${TAB}  mv \"\$(STATS_FILE).tmp\" \"\$(STATS_FILE)\"; \\
${TAB}fi
${TAB}@echo \"Removing temporary files and backup (if any)\"
${TAB}\$(AM_V${EMPTY}_at)\$(RM) \"\$(STATS_FILE).tmp\"
${TAB}\$(AM_V${EMPTY}_at)cat \"\$(STATS_FILE)\"

clean-stats:
${TAB}test -z \"\$(LIST_FILE)\" || rm -f \"\$(LIST_FILE)\"
${TAB}test -z \"\$(CLOC_FILE)\" || rm -f \"\$(CLOC_FILE)\"
${TAB}test -z \"\$(OHCOUNT_FILE)\" || rm -f \"\$(OHCOUNT_FILE)\"
${TAB}test -z \"\$(STATS_FILE)\" || rm -f \"\$(STATS_FILE)\"
"
], [dnl
code_statistics_rules+="
.PHONY: filelist cloc ohcount stats update-stats clean-stats
filelist cloc ohcount stats update-stats clean-stats:
${TAB}@echo "Code statistics was disabled during configuration."
${TAB}@false
"
])])dnl _code_statistics_rules


# AX_CODE_STATISTICS()
AC_DEFUN([AX_CODE_STATISTICS],
[dnl
AX_REQUIRE_DEFINED([AC_ARG_ENABLE])dnl
AX_REQUIRE_DEFINED([AC_ARG_VAR])dnl
AX_REQUIRE_DEFINED([AC_MSG_WARN])dnl
AX_REQUIRE_DEFINED([AC_PATH_PROG])dnl
AX_REQUIRE_DEFINED([AC_PROG_SED])dnl
AX_REQUIRE_DEFINED([AC_SUBST])dnl
AX_REQUIRE_DEFINED([AM_SUBST_NOTMAKE])dnl
AX_REQUIRE_DEFINED([AS_IF])dnl
AC_REQUIRE([AC_PROG_SED])dnl

AC_ARG_ENABLE([code-statistics],
              [AS_HELP_STRING([--disable-code-statistics],
                              [Disable computation of code statistics.])])dnl

AS_IF([test "${enable_code_statistics}" != "no"],
      [enable_code_statistics="yes"])dnl
      
dnl If code statistics is enabled...
AS_IF([test "${enable_code_statistics}" = "yes"],
      [dnl ...check for available program to compute code statistics.
       _code_statistics_check_tool([cloc])
       _code_statistics_check_tool([ohcount])
       dnl Check if at least one program is available
       AS_IF([test -z "${CLOC}" -a -z "${OHCOUNT}"],
             [enable_code_statistics=no
              AC_MSG_WARN([No program found to compute statistics. Please update your settings.])])])dnl

dnl Compute makefile rules and export them in CODE_STATISTICS_RULES autoconf
dnl substituted variable.
TAB=$'\t'
_code_statistics_rules
AC_SUBST([CODE_STATISTICS_RULES], [${code_statistics_rules}])dnl
AM_SUBST_NOTMAKE([CODE_STATISTICS_RULES])dnl

dnl Define an Automake conditional variable
AM_CONDITIONAL([CODE_STATISTICS_ENABLED], [test "${enable_code_statistics}" = "yes"])dnl

])dnl AX_CODE_STATISTICS
dnl Local variables:
dnl mode: autoconf
dnl fill-column: 77
dnl End:
