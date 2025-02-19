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


########################
# Global configuration #
########################

SUBDIRS = basic extra

relative_libdir = transformers
install_dirname = $(PACKAGE_NAME)-$(PACKAGE_VERSION)/$(relative_libdir)
pkgincludedir   = $(includedir)/$(install_dirname)

# Bug fix for path with spaces
bindir = "@bindir@"
libdir = "@libdir@/$(install_dirname)"
# End of bug fix

BUILT_SOURCES      =
lib_LTLIBRARIES    =
noinst_LTLIBRARIES =
check_LTLIBRARIES  =
bin_PROGRAMS       =
pkginclude_HEADERS =


#######################################
# Official k-mer transformers plugins #
#######################################

transformers_main_header = $(srcdir)/kmer_transformers.hpp

pkginclude_HEADERS += $(transformers_main_header)

BUILT_SOURCES += $(transformers_main_header)

$(transformers_main_header):
	@$(AM_V_P) || echo "  HEADER   " '$(@)'
	$(AM_V_at)($(SED) -n '/^\/\*\+$$/{p;:a;n;/^\*\+\/$$/{p;q};p;ba}' $(CONFIG_HEADER) \
	&& echo \
	&& echo '#ifndef __KMER_TRANSFORMERS_HPP__' \
	&& echo '#define __KMER_TRANSFORMERS_HPP__' \
	&& echo \
	&& echo '/**' \
	&& echo ' * \file' \
	&& echo ' *' \
	&& echo ' * Including this file allows to use the k-mer transformers defined in the official plugins.' \
	&& echo ' */' \
	&& echo \
	&& for subdir in $(SUBDIRS); do \
	  echo "#include <$(LIBNAME)/$(relative_libdir)/$${subdir}/kmer_transformers_$${subdir}.hpp>" ; \
	done \
	&& echo \
	&& echo '#endif' \
	) > '$(@)'


####################
# Cleaning targets #
####################

MOSTLYCLEANFILES = *~
CLEANFILES = *~
DISTCLEANFILES = *~
MAINTAINERCLEANFILES = *~
