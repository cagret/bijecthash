# ===========================================================================
#  https://https://gite.lirmm.fr/doccy-dev-tools/autoconf
# ===========================================================================
#
# Serial 5
#
# SYNOPSIS
#
#   This macro defines options for using either system available or included
#   external C++ library and computes the required C++ compiler and linker
#   flags accordingly.
#
#   AX_EXTERNAL_INCLUDED_CXX_LIBRARY_DETECTION(NAME,
#                                              INCLUDED_LIB_CONFIG,
#                                              [LIBRARY],
#                                              [VERSION],
#                                              [FUNCTION],
#                                              [OTHER_LIBRARIES],
#                                              [MESSAGE-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   This macro is a simple wrapper around the
#   AX_EXTERNAL_INCLUDED_GENERIC_LIBRARY_CHECK() macro. Please, refer to the
#   documentation of this later macro for full details.
#
#   This macro defines the '--with-NAME-prefix' and '--with-included-NAME'
#   options and sets the NAME_CXXFLAGS compiler flags and NAME_LDFLAGS linker
#   flags variables as well as the INCLUDED_NAME automake conditional.
#
#   Note: This macro relies on the AX_REQUIRE_DEFINED() external macro, which
#   is available from the autoconf archive
#   (https://www.gnu.org/software/autoconf-archive/ax_require_defined.html).
#   It also relies to the AX_EXTERNAL_INCLUDED_GENERIC_LIBRARY_CHECK() macro
#   coming with this one.
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

AC_DEFUN([AX_EXTERNAL_INCLUDED_CXX_LIBRARY_DETECTION],
[dnl
AC_REQUIRE([AC_PROG_CXX])dnl
AX_REQUIRE_DEFINED([AX_EXTERNAL_INCLUDED_GENERIC_LIBRARY_CHECK])dnl
AX_EXTERNAL_INCLUDED_GENERIC_LIBRARY_CHECK([CXXFLAGS], [LDFLAGS], [$1], [$2], [$3], [$4], [$5], [$6], [$7])dnl
])dnl AX_EXTERNAL_INCLUDED_CXX_LIBRARY_DETECTION
dnl Local variables:
dnl mode: autoconf
dnl fill-column: 77
dnl End:
