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


######################
# pkgconfig handling #
######################

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libkmer-reader-@VERSION@.pc libkmer-transformers-@VERSION@.pc

lib%-@VERSION@.pc: lib%.pc
	cp "$^" "$@"

install-data-hook:
	@for pc_file in $(pkgconfig_DATA); do \
	  $(LN_S) -f $${pc_file/.pc/-@VERSION@.pc} $(DESTDIR)@libdir@/pkgconfig/$${pc_file}; \
	done

uninstall-hook:
	@for pc_file in $(pkgconfig_DATA); do \
	  test "`readlink $(DESTDIR)@libdir@/pkgconfig/$${pc_file}`" != "$${pc_file/.pc/-@VERSION@.pc}" \
          || $(RM) $(DESTDIR)@libdir@/pkgconfig/$${pc_file}; \
	done


####################
# Cleaning targets #
####################

MOSTLYCLEANFILES=*~
CLEANFILES=*~
DISTCLEANFILES=*~ control libkmer-reader-@VERSION@.pc libkmer-transformers-@VERSION@.pc
MAINTAINERCLEANFILES=*~
