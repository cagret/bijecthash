###############################################################################
#                                                                             #
#  Copyright © 2016-2025 -- LIRMM/CNRS/UM                                     #
#                           (Laboratoire d'Informatique, de Robotique et de   #
#                           Microélectronique de Montpellier /                #
#                           Centre National de la Recherche Scientifique /    #
#                           Université de Montpellier)                        #
#                                                                             #
#  Auteurs/Authors: Alban MANCHERON  <alban.mancheron@lirmm.fr>               #
#                                                                             #
#  -------------------------------------------------------------------------  #
#                                                                             #
#  Ce logiciel permet  de gérer le marquage  des versions logicielles et des  #
#  éventuelles librairies versionnées dans un dépôt git.                      #
#                                                                             #
#  Ce logiciel est régi  par la licence CeCILL  soumise au droit français et  #
#  respectant les principes  de diffusion des logiciels libres.  Vous pouvez  #
#  utiliser, modifier et/ou redistribuer ce programme sous les conditions de  #
#  la licence CeCILL  telle que diffusée par le CEA,  le CNRS et l'INRIA sur  #
#  le site "http://www.cecill.info".                                          #
#                                                                             #
#  En contrepartie de l'accessibilité au code source et des droits de copie,  #
#  de modification et de redistribution accordés par cette licence, il n'est  #
#  offert aux utilisateurs qu'une garantie limitée.  Pour les mêmes raisons,  #
#  seule une responsabilité  restreinte pèse  sur l'auteur du programme,  le  #
#  titulaire des droits patrimoniaux et les concédants successifs.            #
#                                                                             #
#  À  cet égard  l'attention de  l'utilisateur est  attirée sur  les risques  #
#  associés  au chargement,  à  l'utilisation,  à  la modification  et/ou au  #
#  développement  et à la reproduction du  logiciel par  l'utilisateur étant  #
#  donné  sa spécificité  de logiciel libre,  qui peut le rendre  complexe à  #
#  manipuler et qui le réserve donc à des développeurs et des professionnels  #
#  avertis  possédant  des  connaissances  informatiques  approfondies.  Les  #
#  utilisateurs  sont donc  invités  à  charger  et  tester  l'adéquation du  #
#  logiciel  à leurs besoins  dans des conditions  permettant  d'assurer  la  #
#  sécurité de leurs systêmes et ou de leurs données et,  plus généralement,  #
#  à l'utiliser et l'exploiter dans les mêmes conditions de sécurité.         #
#                                                                             #
#  Le fait  que vous puissiez accéder  à cet en-tête signifie  que vous avez  #
#  pris connaissance  de la licence CeCILL,  et que vous en avez accepté les  #
#  termes.                                                                    #
#                                                                             #
#  -------------------------------------------------------------------------  #
#                                                                             #
#  This software allow to manage tags for distribute software and library     #
#  releases inside a git repository.                                          #
#                                                                             #
#  This software is governed by the CeCILL license under French law and       #
#  abiding by the rules of distribution of free software. You can use,        #
#  modify and/ or redistribute the software under the terms of the CeCILL     #
#  license as circulated by CEA, CNRS and INRIA at the following URL          #
#  "http://www.cecill.info".                                                  #
#                                                                             #
#  As a counterpart to the access to the source code and rights to copy,      #
#  modify and redistribute granted by the license, users are provided only    #
#  with a limited warranty and the software's author, the holder of the       #
#  economic rights, and the successive licensors have only limited            #
#  liability.                                                                 #
#                                                                             #
#  In this respect, the user's attention is drawn to the risks associated     #
#  with loading, using, modifying and/or developing or reproducing the        #
#  software by the user in light of its specific status of free software,     #
#  that may mean that it is complicated to manipulate, and that also          #
#  therefore means that it is reserved for developers and experienced         #
#  professionals having in-depth computer knowledge. Users are therefore      #
#  encouraged to load and test the software's suitability as regards their    #
#  requirements in conditions enabling the security of their systems and/or   #
#  data to be ensured and, more generally, to use and operate it in the same  #
#  conditions as regards security.                                            #
#                                                                             #
#  The fact that you are presently reading this means that you have had       #
#  knowledge of the CeCILL license and that you accept its terms.             #
#                                                                             #
###############################################################################

#
# This file defines two macros: AX_GIT_VERSIONING_PROG_PATH and AX_GIT_VERSIONING.
#
# The AX_GIT_VERSIONING_PROG_PATH macro intends to set the correct path 


m4_define([gv_prog_basename], [git-version-gen])dnl
m4_define([chk_gv_prog],
           [m4_define([gv_prog_cur], [$1/gv_prog_basename])
            m4_define([gv_prog], m4_esyscmd_s(test -f "gv_prog_cur" && echo "gv_prog_cur" || echo "gv_prog"))])dnl

# AX_GIT_VERSIONING_PROG_PATH([PATH_ENV = $PATH])
# --------------------------------------
# This macro defines the macro gv_prog to the correct path for the
# git-version-gen program (see macro gv_prog_basename defined above).
#
# When called for the first time, the git-version-gen program is
# searched within the PATH_ENV variable (if not given, the PATh
# environment variable is used).
#
# Any subsequent call have no effects while the gv_prog is defined.
#
# This macro is automatically called without argument by
# AX_GIT_VERSIONING, thus it is not required to call it. Although, if
# the git-version-gen program is not available from the PATH
# environment variable, it is required to explicitally call this macro
# with a correct argument BEFORE any subsequent call to
# AX_GIT_VERSIONING.
# --------------------------------------
AC_DEFUN([AX_GIT_VERSIONING_PROG_PATH], [
  m4_divert_text([BODY], [AS_ECHO_N(["Checking for gv_prog_basename... "])])
  m4_ifdef([gv_prog],[
    m4_divert_text([BODY], [AS_ECHO(["Already set to 'gv_prog_path/gv_prog_basename'."])])
  ],[
    m4_define([gv_prog_path], m4_esyscmd_s([echo "m4_default([$1], [$PATH])"]))
    m4_define([paths], m4_split(gv_prog_path, [[:]]))
    m4_define([rpaths], [m4_reverse(paths)])
    m4_define([gv_prog], [])
    m4_map([chk_gv_prog], [m4_reverse(paths)])
    m4_ifset([gv_prog], 
             [m4_divert_text([BODY], [AS_ECHO(["gv_prog"])])],
             [m4_divert_text([BODY], [AS_ERROR(["Program gv_prog_basename not found in gv_prog_path"])])])
  ])
])

# AX_GIT_VERSIONING([STORE_PATH = .])
# --------------------------------------
#
# This macro computes the major, minor (and micro if available)
# information numbers as well as extra pending version informations
# from the main version file. It also computes (when available) the
# library version number using both colon-separated or dot-separated
# values. Finally, it also computes the configure.ac/configure.in git
# version tag.
#
# This macro first defines the corresponding macros (GV_MAJOR,
# GV_MINOR, GV_MICRO, GV_EXTRA, GV_LIB_VERSION, GV_LIB_VERSION_EXT,
# GV_REVISION) to their correct values, then delays the definition of
# the eponymous shell variables at the beginning of the generated
# configure script body.
#
# Calling this macro before AC_INIT makes available the use of the
# GV_* macros to set the version number.
#
# A common way to use these macros in configure.ac/configure.in file
# is:
#   dnl AX_GIT_VERSIONING_PROG_PATH([/the/paths/to:/look/for/if/necessary])
#   AX_GIT_VERSIONING([config])
#   AC_INIT([MyProg], [GV_MAJOR.GV_MINOR.GV_MICRO], ...)
#   AC_REVISION([GV_REVISION])
# --------------------------------------
AC_DEFUN([AX_GIT_VERSIONING], [
  AC_BEFORE([$0], [PACKAGE_VERSION])dnl
  m4_ifdef([GV_MAJOR],[
  ],[
    AC_PREREQ([2.64])
    m4_divert_text([BODY], [m4_text_box([start of git-versioning])])
    m4_define([gv_store_path], [m4_default([$1], [.])])
    AX_GIT_VERSIONING_PROG_PATH()

    m4_divert_text([BODY], [AS_ECHO_N(["Collecting version informations... "])])
    m4_define([gv_version], m4_esyscmd_s(gv_prog --store gv_store_path))

    m4_define([GV_MAJOR], [m4_bpatsubst(m4_quote(gv_version),
                                        [^[^0-9]*\([0-9]+\)\.\([0-9]+\)\(\.\([0-9]+\)\)?\(-\(.*\)\)?$],
                                        [\1])])
    m4_define([GV_MINOR], [m4_bpatsubst(m4_quote(gv_version),
                                        [^[^0-9]*\([0-9]+\)\.\([0-9]+\)\(\.\([0-9]+\)\)?\(-\(.*\)\)?$],
                                        [\2])])
    m4_define([GV_MICRO], [m4_bpatsubst(m4_quote(gv_version),
                                        [^[^0-9]*\([0-9]+\)\.\([0-9]+\)\(\.\([0-9]+\)\)?\(-\(.*\)\)?$],
                                        [\4])])
    m4_define([GV_EXTRA], [m4_bpatsubst(m4_quote(gv_version),
                                        [^[^0-9]*\([0-9]+\)\.\([0-9]+\)\(\.\([0-9]+\)\)?\(-\(.*\)\)?$],
                                        [\6])])
    m4_define([GV_REVISION], [m4_esyscmd_s(gv_prog --timestamp --force-update --force-creation --store gv_store_path __file__)])
    m4_define([gv_versionfile], m4_quote(m4_sinclude(gv_store_path/version)))
    m4_define([GV_LIB_VERSION], [m4_chomp(m4_bregexp(gv_versionfile, [^[ ]*LIB_VERSION[ ]*:[ ]*\([^ ]*\)$], [\1]))])
    m4_define([GV_LIB_VERSION_EXT], [m4_translit(GV_LIB_VERSION, [:], [.])])
    m4_divert_text([BODY], [dnl
[GV_MAJOR]="GV_MAJOR"
[GV_MINOR]="GV_MINOR"
[GV_MICRO]="GV_MICRO"
[GV_EXTRA]="GV_EXTRA"
[GV_REVISION]="GV_REVISION"
[GV_LIB_VERSION]="GV_LIB_VERSION"
[GV_LIB_VERSION_EXT]="GV_LIB_VERSION_EXT"
AS_ECHO(["Ok
Git-versioning settings:
  - Informations stored in path 'gv_store_path'
  - [GV_MAJOR]=$[GV_MAJOR]
  - [GV_MINOR]=$[GV_MINOR]
  - [GV_MICRO]=$[GV_MICRO]
  - [GV_EXTRA]=$[GV_EXTRA]
  - [GV_REVISION] (file __file__)=$[GV_REVISION]
  - [GV_LIB_VERSION]=$[GV_LIB_VERSION]
  - [GV_LIB_VERSION_EXT]=$[GV_LIB_VERSION_EXT]dnl
"])
m4_text_box([end of git-versioning])
    ])
  ])
])
