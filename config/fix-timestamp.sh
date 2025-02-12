#!/bin/sh
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
#  Programmeurs/Programmers:                                                  #
#                   Alban MANCHERON  <alban.mancheron@lirmm.fr>               #
#                                                                             #
#  -------------------------------------------------------------------------  #
#                                                                             #
#  Ce logiciel est régi par la licence CeCILL-C soumise au droit français et  #
#  respectant les principes  de diffusion des logiciels libres.  Vous pouvez  #
#  utiliser, modifier et/ou redistribuer ce programme sous les conditions de  #
#  la licence CeCILL-C telle que diffusée par le CEA, le CNRS et l'INRIA sur  #
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
#  pris connaissance de la licence CeCILL-C, et que vous en avez accepté les  #
#  termes.                                                                    #
#                                                                             #
#  -------------------------------------------------------------------------  #
#                                                                             #
#  This software is governed by the CeCILL-C license under French law and     #
#  abiding by the rules of distribution of free software. You can use,        #
#  modify and/ or redistribute the software under the terms of the CeCILL-C   #
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
#  knowledge of the CeCILL-C license and that you accept its terms.           #
#                                                                             #
###############################################################################

set -eup

PROG_NAME=$(basename $0)
PROG_VERSION=1.0.0

CURRENT_DIR=$(dirname $0)
TOP_SRCDIR=$(dirname ${CURRENT_DIR})

cd "${TOP_SRCDIR}"

# fix-timestamp.sh: prevents useless rebuilds after "cvs update", "git
# clone", ...
#
# The script is merely inspired by
# https://www.gnu.org/software/automake/manual/html_node/CVS.html
#
# It has been adapted to accept other autoconf managed projects
# included in subdirectories (like libraries).


# The timestamps must be set according the following dependy graph:
# [source: https://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.71/html_node/Making-configure-Scripts.html]
#
#   configure.ac --.
#                  +--> automake* --> Makefile.in
#   Makefile.am ---'
#
#
#   [acinclude.m4] -------.
#                         |
#   [local macros] -------+--> aclocal* --> aclocal.m4
#                         |
#   configure.{ac,in] ----'
#
#
#   configure.{ac,in] --.
#                       |   .------> autoconf* -----> configure
#   [aclocal.m4] -------+---+
#                       |   `-----> [autoheader*] --> [config.h.in]
#   [acsite.m4] --------'
#

# Everything starts with the configure.{ac,in} script(s), the
# (auto)makefile template(s) and the acinclude.m4 and local macros
# (expected to be stored in a m4/ directory)
find . \( \
     -iname "configure.ac" \
     -o -name "configure.in" \
     -o -iname "makefile.am" \
     -o -iname "acinclude.m4" \
     -o -ipath "*/m4/*.m4" \
     \) \
     -exec touch "{}" \;

sleep 1 # To be sure that the next touched files have a posterior
        # timestamp.

# aclocal-generated aclocal.m4 depends on locally-installed '.m4'
# macro files, the acinclude.m4 file as well as on 'configure.{ac,in}'
find . -iname "aclocal.m4" -exec touch "{}" \;

sleep 1 # To be sure that the next touched files have a posterior
        # timestamp.

# autoconf-generated configure depends on aclocal.m4 and on
# configure.{ac,in}
find . -iname "configure" -exec touch "{}" \;

# so does autoheader-generated config.h.in
find . -iname "config.h.in" -exec touch "{}" \;

# and all the automake-generated Makefile.in files
find . -iname "makefile.in" -exec touch "{}" \;

# Finally, the makeinfo-generated '.info' files depend on the
# corresponding '.texi' files (if any).
find . -ipath "*/doc/*.info" -exec touch "{}" \;

