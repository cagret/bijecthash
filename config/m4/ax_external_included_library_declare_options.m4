# ===========================================================================
#  https://https://gite.lirmm.fr/doccy-dev-tools/autoconf
# ===========================================================================
#
# Serial 5
#
# SYNOPSIS
#
#   This macro defines options for using either system available or included
#   external libraries.
#
#   AX_EXTERNAL_INCLUDED_LIBRARY_DECLARE_OPTIONS(NAME, [ONLY_HEADER])
#
# DESCRIPTION
#
#   According to the value of the optional ONLY_HEADER parameter, this macro
#   will declare options for a full library detection or a some header file
#   detection.
#
#   This macro defines the following options:
#     - '--with-included-NAME[=yes|no|check]' (default to is 'check')
#     - '--without-included-NAME' (alias for '--without-included-NAME=no').
#     - '--with-NAME-path=DIR' if ONLY_HEADER is set to 'true'
#     - '--with-NAME-prefix=DIR' if ONLY_HEADER is not set to 'true'
#
#   This does nothing more and is mainly devoted to the following macros:
#     - AX_EXTERNAL_INCLUDED_C_LIBRARY_DETECTION()
#     - AX_EXTERNAL_INCLUDED_CXX_LIBRARY_DETECTION()
#     - AX_EXTERNAL_INCLUDED_CPP_HEADER_DETECTION()
#
#   ANY CHANGE IN THIS FILE MUST ENSURE NOT BREAKING THE ABOVE MACROS!
#
# LICENSE
#
#   Copyright © 2015-2025 -- LIRMM / CNRS / UM
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

AC_DEFUN([AX_EXTERNAL_INCLUDED_LIBRARY_DECLARE_OPTIONS],
[dnl

dnl Add option --without-included-NAME (default --with-included-NAME=check)
AC_ARG_WITH([included-$1],
            [dnl
AS_HELP_STRING([--with-included-$1[[=yes*|no|check]]],
               [When argument is set to 'yes', then forces to use the $1 library included here.

                When argument is set to 'no' then forces to use an already installed $1 library.

                When not given or set to 'check', then uses the included $1 library if it is not found on the host system.])
AS_HELP_STRING([--without-included-$1],
               [Don't use the $1 library included here (shortcut for --without-included-$1=no)])],
            [],
            [with_included_$1="check"])
ifelse([$2], [true],
      [dnl Add option --with-NAME-path=DIR
       AC_ARG_WITH([$1-path],
                   [AS_HELP_STRING([--with-$1-path=DIR],
                                   [search for $1 headers in DIR])])],
      [dnl Add option --with-NAME-prefix=PATH
       AC_ARG_WITH([$1-prefix],
                   [AS_HELP_STRING([--with-$1-prefix=DIR],
                                   [search for $1 headers in DIR/include and the library in DIR/lib])])])
])dnl AX_EXTERNAL_INCLUDED_LIBRARY_DECLARE_OPTIONS
dnl Local variables:
dnl mode: autoconf
dnl fill-column: 77
dnl End:
