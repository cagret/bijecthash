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

PLUGIN_VERSION = 1.0.0

AM_CXXFLAGS =					\
  -I$(top_srcdir)/src				\
  -DCXX="\"$(CXX)\""                    	\
  -DARCH="\"$(build_cpu)\""             	\
  -DOS="\"$(build_os)\""                	\
  -DPACKAGE_DATADIR="\"$(pkgdatadir)\""		\
  -DPLUGIN_VERSION="\"$(PLUGIN_VERSION)\""

relative_libdir = transformers/basic
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


###########################################
# Basic k-mer transformers plugin library #
###########################################

lib_LTLIBRARIES += kmer-transformers-basic-plugin.la

plugin_main_header = $(srcdir)/kmer_transformers_basic.hpp

plugin_headers = \
  canonical_transformer.hpp		\
  composition_transformer.hpp		\
  identity_transformer.hpp		\
  permutation_bit_transformer.hpp	\
  permutation_transformer.hpp

pkginclude_HEADERS += $(plugin_headers) $(plugin_main_header)

BUILT_SOURCES += $(plugin_main_header)

$(plugin_main_header): $(plugin_headers)
	@$(AM_V_P) || echo "  HEADER   " '$(@)'
	$(AM_V_at)($(SED) -n '/^\/\*\+$$/{p;:a;n;/^\*\+\/$$/{p;q};p;ba}' $(CONFIG_HEADER) \
	&& echo \
	&& echo '#ifndef __KMER_TRANSFORMERS_BASIC_HPP__' \
	&& echo '#define __KMER_TRANSFORMERS_BASIC_HPP__' \
	&& echo \
	&& echo '/**' \
	&& echo ' * \file' \
	&& echo ' *' \
	&& echo ' * Including this file allows to use the k-mer transformers defined in the basic plugin.' \
	&& echo ' */' \
	&& echo \
	&& for header in $(^F); do \
	  echo "#include <$(LIBNAME)/$(relative_libdir)/$${header}>" ; \
	done \
	&& echo \
	&& echo '#endif' \
	) > '$(@)'

kmer_transformers_basic_plugin_ladir = $(abs_srcdir)
kmer_transformers_basic_plugin_la_SOURCES = \
  kmer_transformers_basic_plugin.cpp					\
  $(plugin_main_header)							\
  canonical_transformer.cpp canonical_transformer.hpp			\
  composition_transformer.cpp composition_transformer.hpp		\
  identity_transformer.cpp identity_transformer.hpp			\
  permutation_bit_transformer.cpp permutation_bit_transformer.hpp	\
  permutation_transformer.cpp permutation_transformer.hpp		\
  @top_srcdir@/src/common.hpp						\
  @top_srcdir@/src/exception.hpp					\
  @top_srcdir@/src/transformer.hpp

kmer_transformers_basic_plugin_la_LDFLAGS =	\
  -release "$(PLUGIN_VERSION)"		\
  -shared					\
  -rpath $(libdir)				\
  -module


# Debug version (for tests)
check_LTLIBRARIES += kmer-transformers-basic-plugin-debug.la
kmer_transformers_basic_plugin_debug_ladir       = $(kmer_transformers_basic_plugin_ladir)
kmer_transformers_basic_plugin_debug_la_SOURCES  = $(kmer_transformers_basic_plugin_la_SOURCES)
kmer_transformers_basic_plugin_debug_la_CXXFLAGS = $(AM_CXXFLAGS) -UNDEBUG -UDEBUG -g -O0
kmer_transformers_basic_plugin_debug_la_LDFLAGS  = $(kmer_transformers_basic_plugin_la_LDFLAGS)


#################
# Code Coverage #
#################

GCDA_FILES = $(patsubst %.cpp,%.gcda,$(filter %.cpp,$(SOURCES)))
GCNO_FILES = $(GCDA_FILES:.gcda=.gcno)
GCNO_FILES += $(kmer_transformers_basic_plugin_la_OBJECTS:.lo=.gcno)
GCNO_FILES += $(kmer_transformers_basic_plugin_debug_la_OBJECTS:.lo=.gcno)
GCOV_FILES = $(GCDA_FILES:.gcda=.gcov)


####################
# Cleaning targets #
####################

MOSTLYCLEANFILES = *~
CLEANFILES = *~ $(GCDA_FILES) $(GCNO_FILES) $(GCOV_FILES)
DISTCLEANFILES = *~
MAINTAINERCLEANFILES = *~
