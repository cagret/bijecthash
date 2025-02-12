# ===========================================================================
#  https://https://gite.lirmm.fr/doccy-dev-tools/autoconf
# ===========================================================================
#
# Serial 2
#
# SYNOPSIS
#
#   AX_DEBIAN_PACKAGE(CONTROL, [METADATA:FILE], ...)
#
# DESCRIPTION
#
#
#   The `AX_DEBIAN_PACKAGE()` macro enables and automates debian package to
#   be built in an old fashion way by adding a `deb` target and a `deb-clean`
#   for `make`.
#
#   In order to build the Debian package, the only mandatory metadata file is
#   the CONTROL file, which must exist or have a template having the `.in`
#   extension. If a template is found, then it is automatically added to the
#   `AC_CONFIG_FILES()`). It is possible to add other metadata files using
#   the additional macro parameters as `METADATA:FILE` where `METADATA` must
#   be a valid metadata keyword (available metadata keywords are: `PREINST`,
#   `PRERM`, `POSTINST`, POSTRM`, `CONFFILES` and `SHLIBS`) and `FILE` must
#   exist or have a template (behaves exactly as for the `CONTROL`
#   parameter). Notice that the `Installed-Size` field of the `CONTROL` file
#   is automatically updated to thecorrect value
#
#   Since macro 'AX_DEBIAN_PACKAGE()` requires the `AC_CANONICAL_HOST()` and
#   `AC_CANONICAL_TARGET()`, any macro that invoke the `AC_ARG_PROGRAM()`
#   (*e.g.,* `AM_INIT_AUTOMAKE()`) must be called after this one.
#
#   The installation prefix (in the Debian package) defaults to the current
#   configured ${prefix}. Nonetheless, it is possible to use either the
#   `DEBIAN_PACKAGE_PREFIX` environment variable or the homonym Makefile
#   variable to specify another installation prefix (can be used as output
#   variable for pre/post inst/rm scripts).
#
#   To build build the Debian package, only the `dpkg` program is
#   required. You can a custom version of this program by setting the `DPKG`
#   environment variable. If not valid `dpkg` program is found, the the `deb`
#   and `deb-clean` targets will provoke an error with an explicit message.
#
#   The Makefile rules are defined in the `DEBIAN_PACKAGE_RULES` configure
#   substituted variable, which should be used in one `Makefile.am` (either
#   in the top directory or in some custom subdirectory).
#
#   The `@DEBIAN_PACKAGE_ARCH@` output variable is defined to the correct
#   target value (and should be used in the `control.in` template file -- if
#   any -- for the `Architecture` field).
#
#   Building the Debian package involves running the `configure` script. If
#   it is needed to supply additional flags to it, it is possible to define
#   them using the `DISTCHECK_CONFIGURE_FLAGS`, on the command line when
#   invoking `make` (the variable name is the one used in `make distcheck`
#   since it behaves exactly the same, see
#   https://www.gnu.org/software/automake/manual/html_node/Checking-the-Distribution.html). Notice
#   that `make deb` (like `make distcheck`) needs complete control over the
#   `configure` options `--srcdir` and `--prefix`, thus these two option
#   can't be set by using the `DISTCHECK_CONFIGURE_FLAGS` variable.
#
#   Finally, the `DEBIAN_PACKAGE_ENABLED` automake conditional is defined to
#   true if the `dpkg` program is available (and thus the makefile rules to
#   produce a Debian package) as well as the metadata (or the metadata
#   template) files.
#
#   Usage example:
#
#   - In your `configure.ac` file:
#
#       dnl Let's suppose that the `control.in` template file is located in
#       dnl the `packaging` subdirectory. Let's suppose that the package must
#       dnl provides a `prerm` and a `postinst` script that are located in
#       dnl this subdirectory too. The following rule will pass the
#       dnl `packaging/control` file to the `AC_CONFIG_FILES()` and the
#       dnl Debian package rules will use these three metadata files.
#       AX_DEBIAN_PACKAGE([packaging/control],
#                         [PRERM:packaging/prerm],
#                         [POSTINST:packaging/postinst])
#
#   - In your `Makefile.am` file:
#
#       ##################################
#       # Debian Package related targets #
#       ##################################
#       
#       @DEBIAN_PACKAGE_RULES@
#       
#       clean-local: deb-clean
#       
#       #########################################
#       # End of Debian Package relater targets #
#       #########################################
#       
#       # If control is generated in this directory using the control.in
#       # template (and only in this case!!!), uncomment the following.
#       # DISTCLEANFILES = control
#
#
# LICENSE
#
#   Copyright © 2004-2025 -- LIRMM / CNRS / UM
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


dnl _debian_package_enabled is set to true unless some bad configuration
dnl occurs.
m4_define([_debian_package_enabled], [true])dnl

dnl _debian_package_executable_metadata is the set of metadata files that
dnl must have execution rights.
m4_set_delete([_debian_package_executable_metadata])dnl
m4_set_add_all([_debian_package_executable_metadata],
               [PREINST], [PRERM], [POSTINST], [POSTRM])dnl

dnl _debian_package_valid_metadata is the set of accepted metadata files.
m4_set_delete([_debian_package_valid_metadata])dnl
m4_set_add_all([_debian_package_valid_metadata],
               [CONFFILES], [SHLIBS])dnl
m4_set_foreach([_debian_package_executable_metadata], [_metadata],
               [m4_set_add([_debian_package_valid_metadata], _metadata)])dnl

dnl _debian_package_check_file_exist(FILE,
dnl                                  [ACTION-IF-FOUND],
dnl                                  [ACTION-IF-NOT-FOUND],
dnl                                  [TEST_FLAG])
dnl Check for file existence on host file system using the given TEST_FLAG
dnl (or '-r', which means that file is readable, if not given). If file
dnl exists, then run ACTION-IF-FOUND else run ACTION-IF-NOT-FOUND.
m4_define([_debian_package_check_file_exist], [dnl
m4_syscmd(test m4_default([$4], [-r]) "$1")dnl
m4_if(m4_sysval(), [0], [$2], [$3])dnl
])dnl _debian_package_check_file_exist


dnl _debian_package_set_distfile(NAME, PATH)
dnl Set the `debian_package_NAME` and `debian_package_base_NAME` shell
dnl variables respectively to PATH and to the PATH base name.
m4_define([_debian_package_set_distfile], [dnl
m4_ifnblank([$2], [dnl
m4_if(_debian_package_enabled, [true],
  [_debian_package_check_file_exist(
    [$2.in],
    [AC_CONFIG_FILES([$2])m4_define([_built], [yes])],
    [_debian_package_check_file_exist(
       [$2], [m4_define([_built], [no])],
       [AC_MSG_WARN([File $2 or $2.in not found.
                     Debian package creation is disabled.])
        m4_define([_debian_package_enabled], [false])])])],
  [_debian_package_check_file_exist(
    [$2.in],
    [AC_CONFIG_FILES([$2])])])dnl
m4_if(_debian_package_enabled, [true], [dnl
debian_package_$1='$2'
debian_package_$1_built='_built'
debian_package_base_$1=$(AS_BASENAME([$2]))
m4_undefine([_built])dnl
])])])dnl _debian_package_set_distfile


dnl _debian_package_rules()
m4_define([_debian_package_rules], [dnl

debian_package_rules="
########################
# Build Debian package #
########################

# The following variable are defined by automake only for the top build
# directory Makefile.
GZIP_ENV ?= --best
distdir ?= \$(PACKAGE)-\$(VERSION)
am__remove_distdir ?= \\
  if test -d \"\$(distdir)\"; then \\
    find \"\$(distdir)\" -type d ! -perm -200 -exec chmod u+w {} ';' \\
      && rm -rf \"\$(distdir)\" \\
      || { sleep 5 && rm -rf \"\$(distdir)\"; }; \\
  else :; fi
# Now they also are defined for the current directory

DEBIAN_PACKAGE_PREFIX ?= ${DEBIAN_PACKAGE_PREFIX}

.PHONY: deb deb-always-build deb-clean

# Fake target to force rules to be applied
deb-always-build:
"

AS_IF([test "${debian_package_enabled}" = "true"],
      [dnl
m4_define([_deb_target], [\$(PACKAGE_NAME)_\$(PACKAGE_VERSION)_\$(DEBIAN_PACKAGE_ARCH).deb])
debian_package_rules+="

# Named targets

deb: _deb_target

deb-clean:
${TAB}-test -z \"_deb_target\" || rm -f \"_deb_target\"


# rule for the Debian package
_deb_target: deb-always-build
${TAB}@if test -z \"\$(DPKG)\"; then \\
${TAB}  echo \"You need to install a Debian package builder and to reconfigure this package.\";\\
${TAB}  exit 1;\\
${TAB}fi
${TAB}\$(am__remove_distdir)
${TAB}\$(AM_V${EMPTY}_at)\$(MAKE) -C \"\$(abs_top_builddir)\" \$(AM_MAKEFLAGS) dist-gzip
${TAB}GZIP= gunzip \$(GZIP_ENV) -c \"\$(abs_top_builddir)/\$(distdir).tar.gz\" | \$(am__untar)
${TAB}\$(AM_V${EMPTY}_at)\$(MKDIR_P) \$(distdir)/_build/_deb/
${TAB}\$(AM_V${EMPTY}_at)\$(MKDIR_P) \$(distdir)/_inst/_deb/
${TAB}\$(AM_V${EMPTY}_at)chmod -R a-w \$(distdir)
${TAB}\$(AM_V${EMPTY}_at)dc_install_base=\$\$(\$(am__cd) \$(distdir)/_inst/_deb/ && pwd | \$(SED) -e 's,^[[^:\\\\/]]:[[\\\\/]],/,') \\
${TAB}  && \$(am__cd) \"\$(distdir)\" \\
${TAB}  && dc_srcdir=\$\$(pwd) \\
${TAB}  && \$(am__cd) _build/_deb \\
${TAB}  && dc_builddir=\$\$(pwd) \\
${TAB}  && chmod u+w \"\$\${dc_builddir}\" \\
${TAB}  && DEBIAN_PACKAGE_PREFIX=\$(DEBIAN_PACKAGE_PREFIX) ../../configure \\
${TAB}       \$(DISTCHECK_CONFIGURE_FLAGS) \\
${TAB}       --srcdir=\"\$\${dc_srcdir}\" \\
${TAB}       --prefix=\"\$(DEBIAN_PACKAGE_PREFIX)\" \\
${TAB}  && \$(MAKE) \$(AM_MAKEFLAGS) localedir=\$\${localedir} \\
${TAB}  && \$(MAKE) \$(AM_MAKEFLAGS) check \\
${TAB}  && chmod u+w \"\$\${dc_install_base}\" \\
${TAB}  && \$(MAKE) \$(AM_MAKEFLAGS) DESTDIR=\"\$\${dc_install_base}\" install \\
${TAB}  && \$(MAKE) \$(AM_MAKEFLAGS) DESTDIR=\"\$\${dc_install_base}\" installcheck \\
${TAB}  && \$(am__cd) \"\$\${dc_install_base}\" \\
${TAB}  && find . -type f -exec md5sum {} \; | \$(SED) \"s,\.\/,,g\" > \"\$\${dc_builddir}/md5sums\" \\
${TAB}  && \$(MKDIR_P) DEBIAN \\
${TAB}  && mv \"\$\${dc_builddir}/md5sums\" DEBIAN/ \\
${TAB}  && chmod u-w \"\$\${dc_builddir}\" \\"

m4_set_foreach([_debian_package_valid_metadata], [file],
               [AS_IF([test -n "${debian_package_[]file}"],
                      [AS_IF([test "${debian_package_[]file[]_built}" = "yes"],
                             [metadata_file_location="\$\${dc_builddir}"],
                             [metadata_file_location="\$\${dc_srcdir}"])
                       debian_package_rules+="
${TAB}  && cp \"${metadata_file_location}/${debian_package_[]file}\" DEBIAN/m4_tolower(file) \\"])
])dnl

m4_set_foreach([_debian_package_executable_metadata], [file],
               [AS_IF([test -n "${debian_package_[]file}"],
                      [debian_package_rules+="
${TAB}  && chmod 755 DEBIAN/m4_tolower(file) \\"])
])dnl

AS_IF([test "${debian_package_CONTROL_built}" = "yes"],
      [metadata_file_location="\$\${dc_builddir}"],
      [metadata_file_location="\$\${dc_srcdir}"])
debian_package_rules+="
${TAB}  && \$(SED) \"s/^Installed-Size:.*\\\$\$/Installed-Size: \$\$(du -ck . | tail -1 | cut -f1)/\" \\
${TAB}      \"${metadata_file_location}/${debian_package_CONTROL}\" > DEBIAN/control \\
${TAB}  && chmod u+w \"\$\${dc_builddir}\" \\
${TAB}  && \$(am__cd) \"\$\${dc_builddir}\" \\
${TAB}  && \$(DPKG) -b \"\$\${dc_install_base}\" \"\$[]@\" \\
${TAB}  && mv \"\$[]@\" \"\$(abs_builddir)\" \\"

m4_set_foreach([_debian_package_executable_metadata],
               [file],
               [AS_IF([test -n "${debian_package_[]file}"],
                      [debian_package_rules+="
${TAB}  && rm -f \"\$\${dc_install_base}/DEBIAN/m4_tolower(file)\" \\"])
])dnl

debian_package_rules+="
${TAB}  && rm -f \"\$\${dc_install_base}/DEBIAN/control\" \\
${TAB}  && rm -f \"\$\${dc_install_base}/DEBIAN/md5sums\" \\
${TAB}  && chmod u-w \"\$\${dc_builddir}\" \\
${TAB}  && \$(MAKE) \$(AM_MAKEFLAGS) DESTDIR=\"\$\${dc_install_base}\" uninstall \\
${TAB}  && \$(MAKE) \$(AM_MAKEFLAGS) distuninstallcheck_dir=\"\$\${dc_install_base}\" \\
${TAB}        distuninstallcheck \\
${TAB}  && rm -rf \"\$\${dc_destdir}\" \\
${TAB}  && chmod -R a+w \"\$\${dc_builddir}\" \\
${TAB}  && \$(MAKE) \$(AM_MAKEFLAGS) distcleancheck
${TAB}\$(am__remove_distdir)
${TAB}@echo \"\$[]@ is ready for distribution\" | \\
${TAB}  \$(SED) 'h;s/./=/g;p;x;p;x'

"], [dnl
debian_package_rules+="
deb-clean:
deb:
${TAB}@echo
${TAB}@echo \"=====================================================================\"
${TAB}@echo
${TAB}@echo \"Release construction is disabled.\"
${TAB}@echo \"Please check your 'config.log' file to see what the problem is\"
${TAB}@echo \"(the 'dpkg' or some debian specific file is probably missing).\"
${TAB}@echo
${TAB}@echo \"=====================================================================\"
${TAB}@echo
${TAB}@false
"
])])dnl _debian_package_rules


dnl _debian_package_is_valid_key([_debian_package_key_name]),
m4_define([_debian_package_is_valid_key], [
m4_set_contains([_debian_package_valid_metadata], $1, [],
                [m4_fatal("Metadata parameter '$1' is unknown; Valid metadata are: m4_set_contents([_debian_package_valid_metadata], [, ]))])dnl 
])dnl


dnl _debian_package_set_key_val(KEY, VALUE)
m4_define([_debian_package_set_key_val], [dnl
m4_if([$#], [2], [],
      [m4_fatal([Bad format for '$@', must be a KEY:VALUE string])])dnl
m4_define([_debian_package_key_name], m4_translit([$1], [-+.a-z], [___A-Z]))dnl
_debian_package_is_valid_key([_debian_package_key_name])
_debian_package_set_distfile(_debian_package_key_name, [$2])dnl
])dnl


dnl _debian_package_process_metadata_key_val(KEY:VALUE]
m4_define([_debian_package_process_metadata_key_val], [dnl
_debian_package_set_key_val(m4_unquote(m4_split($1, [:])))dnl
])dnl


# AX_DEBIAN_PACKAGE(CONTROL, [METADATA:FILE], ...)
AC_DEFUN([AX_DEBIAN_PACKAGE],
[dnl
AX_REQUIRE_DEFINED([AC_ARG_VAR])dnl
AX_REQUIRE_DEFINED([AC_CANONICAL_HOST])dnl
AX_REQUIRE_DEFINED([AC_CANONICAL_TARGET])dnl
AX_REQUIRE_DEFINED([AC_CONFIG_FILES])dnl
AX_REQUIRE_DEFINED([AC_MSG_ERROR])dnl
AX_REQUIRE_DEFINED([AC_MSG_WARN])dnl
AX_REQUIRE_DEFINED([AC_PATH_PROG])dnl
AX_REQUIRE_DEFINED([AC_PROG_SED])dnl
AX_REQUIRE_DEFINED([AC_REQUIRE])dnl
AX_REQUIRE_DEFINED([AC_SUBST])dnl
AX_REQUIRE_DEFINED([AS_BASENAME])dnl
AX_REQUIRE_DEFINED([AS_CASE])dnl
AX_REQUIRE_DEFINED([AS_IF])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_TARGET])dnl
AC_REQUIRE([AC_PROG_SED])dnl

dnl Check target CPU architecture
AS_IF([test -n "${target_cpu}"],
      [DEBIAN_PACKAGE_ARCH="${target_cpu,,}"],
      [test -n "${host_cpu}"],
      [DEBIAN_PACKAGE_ARCH="${host_cpu,,}"])

#Fix the debian architecture name
#See https://wiki.debian.org/ArchitectureSpecificsMemo#Architecture_baselines
AS_CASE([${DEBIAN_PACKAGE_ARCH}],
        [x86_64],          [DEBIAN_PACKAGE_ARCH="amd64"],
        [aarch64 | armv8], [DEBIAN_PACKAGE_ARCH="arm64"],
        [armv[45]te],      [DEBIAN_PACKAGE_ARCH="armel"],
        [armv7],           [DEBIAN_PACKAGE_ARCH="armhf"],
        [i[3456]86],       [DEBIAN_PACKAGE_ARCH="i386"],
        [mips32r2],        [DEBIAN_PACKAGE_ARCH="mips"],
        [power8],          [DEBIAN_PACKAGE_ARCH="ppc64el"])dnl

# Make the DEBIAN_PACKAGE_ARCH an output variable
AC_SUBST([DEBIAN_PACKAGE_ARCH])

dnl Ensure at least the control file is specified.
m4_ifblank([$1],
  [AC_MSG_ERROR([bad usage: '$0(CONTROL, [METADATA:FILE], ...)'.
  The <CONTROL> parameter is mandatory and must exists (or being generated from <CONTROL>.in template).])])dnl

dnl Set the control metadata file
_debian_package_set_distfile([CONTROL],  [$1])dnl
dnl Get list of extra metadata files
m4_define([_metadata], [m4_shift($@)])dnl
dnl For each metadata file parameter...
m4_foreach([_m], [_metadata], [dnl
_debian_package_process_metadata_key_val([_m])dnl
])dnl

m4_if(_debian_package_enabled, [true], [dnl
  AC_PATH_PROG([DPKG], [${DPKG-dpkg}])dnl
  AC_ARG_VAR([DPKG], [Path to the dpkg program (default: dpkg)])dnl
  AC_ARG_VAR([DEBIAN_PACKAGE_PREFIX],
             [Install prefix path for debian package (default: ${prefix})])dnl
  AS_IF([test -z "${DEBIAN_PACKAGE_PREFIX}"],
        [DEBIAN_PACKAGE_PREFIX="${prefix}"])
  AS_IF([test -z "${DEBIAN_PACKAGE_PREFIX}" -o "${DEBIAN_PACKAGE_PREFIX}" = "NONE"],
        [DEBIAN_PACKAGE_PREFIX="${ac_default_prefix}"])
  AS_IF([test -z "${DEBIAN_PACKAGE_PREFIX}"],
        [DEBIAN_PACKAGE_PREFIX="/usr"])
])dnl

debian_package_enabled=_debian_package_enabled
AS_IF([test -z "${DPKG}"],
      [debian_package_enabled=false
       AC_MSG_WARN([
   The 'dpkg' program cound not be found.
   This isn't critical, but it means that you won't be able to create Debian package.
])])

dnl Computes makefile rules and export them in DEBIAN_PACKAGE_RULES autoconf
dnl substituted variable.
TAB=$'\t'
_debian_package_rules
AC_SUBST([DEBIAN_PACKAGE_RULES], [${debian_package_rules}])dnl
AM_SUBST_NOTMAKE([DEBIAN_PACKAGE_RULES])dnl

dnl Define an Automake conditional variable
AM_CONDITIONAL([DEBIAN_PACKAGE_ENABLED], [test "${debian_package_enabled}" = "true"])dnl

])dnl AX_DEBIAN_PACKAGE
dnl Local variables:
dnl mode: autoconf
dnl fill-column: 77
dnl End:
