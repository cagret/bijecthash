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

SUBDIRS = transformers

AM_CXXFLAGS =				\
  -I$(top_srcdir)/src			\
  -DCXX="\"$(CXX)\""                    \
  -DARCH="\"$(build_cpu)\""             \
  -DOS="\"$(build_os)\""                \
  -DPACKAGE_DATADIR="\"$(pkgdatadir)\"" \
  -DPACKAGE_LIBDIR="\"$(pkglibdir)/\""

install_dirname = $(PACKAGE_NAME)-$(PACKAGE_VERSION)
pkgincludedir   = $(includedir)/$(install_dirname)
pkglibdir      = $(libdir)/$(install_dirname)

# Bug fix for path with spaces
bindir = "@bindir@"
libdir = "@libdir@"
# End of bug fix

BUILT_SOURCES      =
lib_LTLIBRARIES    =
noinst_LTLIBRARIES =
check_LTLIBRARIES  =
bin_PROGRAMS       =
pkginclude_HEADERS =


##############################
# k-mer transformers library #
##############################

lib_LTLIBRARIES += libkmer-transformers.la

pkginclude_HEADERS += transformer.hpp

libkmer_transformers_ladir = $(abs_srcdir)
libkmer_transformers_la_SOURCES =	\
  common.hpp				\
  transformer.cpp transformer.hpp

libkmer_transformers_la_configdir   = $(pkglibdir)/kmer-transformers
libkmer_transformers_la_config_DATA = $(CONFIG_HEADER)
libkmer_transformers_la_LDFLAGS     = -version-info "$(LIB_VERSION)" $(AM_LDFLAGS) $(sphinxpp_LDFLAGS)
if INCLUDED_sphinxpp
libkmer_transformers_la_LIBADD      = @top_builddir@/external/Sphinxpp-library/src/libsphinx++.la
endif

# Debug version (for tests)
check_LTLIBRARIES += libkmer-transformers-debug.la
libkmer_transformers_debug_ladir       = $(libkmer_transformers_ladir)
libkmer_transformers_debug_la_SOURCES  = $(libkmer_transformers_la_SOURCES)
libkmer_transformers_debug_la_CXXFLAGS = $(AM_CXXFLAGS) -UNDEBUG -UDEBUG -g -O0
libkmer_transformers_debug_la_LDFLAGS  = $(libkmer_transformers_la_LDFLAGS)
libkmer_transformers_debug_la_LIBADD   = $(libkmer_transformers_la_LIBADD)


#######################
# kmer-reader library #
#######################

lib_LTLIBRARIES += libkmer-reader.la

libkmer_reader_main_header = kmer_reader.hpp

libkmer_reader_headers =	\
  circular_queue.hpp		\
  file_reader.hpp		\
  kmer_collector.hpp		\
  kmer_processor.hpp		\
  locker.hpp			\
  threaded_processor_helper.hpp

pkginclude_HEADERS += $(libkmer_reader_headers) $(libkmer_reader_main_header)

BUILT_SOURCES += $(srcdir)/$(libkmer_reader_main_header)

$(srcdir)/$(libkmer_reader_main_header): $(libkmer_reader_headers)
	@$(AM_V_P) || echo "  HEADER   " '$(@)'
	$(AM_V_at)($(SED) -n '/^\/\*\+$$/{p;:a;n;/^\*\+\/$$/{p;q};p;ba}' $(CONFIG_HEADER) \
	&& echo \
	&& echo '#ifndef __KMER_READER_HPP__' \
	&& echo '#define __KMER_READER_HPP__' \
	&& echo \
	&& echo '/**' \
	&& echo ' * \file' \
	&& echo ' *' \
	&& echo ' * Including this file is enough to use the full kmer-reader library API.' \
	&& echo ' */' \
	&& echo \
	&& for header in $(^F); do \
	  echo "#include <$(LIBNAME)/$${header}>" ; \
	done \
	&& echo \
	&& echo '#endif' \
	) > '$(@)'

libkmer_reader_ladir = $(abs_srcdir)
libkmer_reader_la_SOURCES =		\
  circular_queue.hpp			\
  common.hpp				\
  file_reader.cpp file_reader.hpp	\
  kmer_collector.cpp kmer_collector.hpp	\
  kmer_processor.cpp kmer_processor.hpp	\
  locker.cpp locker.hpp			\
  threaded_processor_helper.hpp

libkmer_reader_la_configdir    = $(pkglibdir)/kmer-reader
libkmer_reader_la_config_DATA  = $(CONFIG_HEADER)
libkmer_reader_la_LDFLAGS      = -version-info "$(LIB_VERSION)" $(AM_LDFLAGS)

# Debug version (for tests)
check_LTLIBRARIES += libkmer-reader-debug.la
libkmer_reader_debug_ladir           = $(libkmer_reader_ladir)
libkmer_reader_debug_la_SOURCES      = $(libkmer_reader_la_SOURCES)
libkmer_reader_debug_la_CXXFLAGS     = $(AM_CXXFLAGS) -UNDEBUG -UDEBUG -g -O0
libkmer_reader_debug_la_LDFLAGS      = $(libkmer_reader_la_LDFLAGS)


###########################
# bijecthash-core library #
###########################

noinst_LTLIBRARIES += libbijecthash-core.la

libbijecthash_core_ladir = $(abs_srcdir)
libbijecthash_core_la_SOURCES = 		\
  bh_kmer_collector.cpp bh_kmer_collector.hpp	\
  bh_kmer_index.cpp bh_kmer_index.hpp		\
  bh_kmer_processor.cpp bh_kmer_processor.hpp	\
  common.hpp					\
  exception.hpp					\
  lcp_stats.cpp lcp_stats.hpp			\
  program_options.cpp program_options.hpp	\
  settings.cpp settings.hpp

libbijecthash_core_la_LDFLAGS      = -avoid-version $(AM_LDFLAGS)

# Debug version (for tests)
check_LTLIBRARIES += libbijecthash-core-debug.la
libbijecthash_core_debug_ladir      = $(libbijecthash_core_ladir)
libbijecthash_core_debug_la_SOURCES = $(libbijecthash_core_la_SOURCES)
libbijecthash_core_debug_la_CXXFLAGS = $(AM_CXXFLAGS) -UNDEBUG -UDEBUG -g -O0
libbijecthash_core_debug_la_LDFLAGS  = $(libbijecthash_core_la_LDFLAGS)


##############
# BijectHash #
##############

bin_PROGRAMS += BijectHash

BijectHash_SOURCES = biject_hash.cpp threaded_reader_writer.hpp

BijectHash_LDADD = libkmer-transformers.la libkmer-reader.la libbijecthash-core.la

BijectHash_LDFLAGS = -lkmer-transformers -lkmer-reader -lbijecthash-core $(AM_LDFLAGS)

BijectHash_DEPENDENCIES = $(BijectHash_LDADD)


##########################
# BijectHash_with_checks #
##########################

bin_PROGRAMS += BijectHash_with_checks

BijectHash_with_checks_SOURCES =	\
  $(BijectHash_SOURCES)			\
  queue_watcher.hpp queue_watcher.tpp

BijectHash_with_checks_LDADD = $(BijectHash_LDADD)

BijectHash_with_checks_LDFLAGS = $(BijectHash_LDFLAGS)

BijectHash_with_checks_CXXFLAGS = $(AM_CXXFLAGS) -DWATCH_QUEUE

BijectHash_with_checks_DEPENDENCIES = $(BijectHash_with_checks_LDADD)


###############################
# BijectHash_with_cache_stats #
###############################

bin_PROGRAMS += BijectHash_with_cache_stats

BijectHash_with_cache_stats_SOURCES =		\
  $(BijectHash_SOURCES)				\
  cache_statistics.cpp cache_statistics.hpp

BijectHash_with_cache_stats_LDADD = $(BijectHash_LDADD)

BijectHash_with_cache_stats_LDFLAGS = $(BijectHash_LDFLAGS)

BijectHash_with_cache_stats_CXXFLAGS = $(AM_CXXFLAGS) -DENABLE_CACHE_STATISTICS

BijectHash_with_cache_stats_DEPENDENCIES = $(BijectHash_with_cache_stats_LDADD)


#########################
# Dependencies handling #
#########################

# named target
.PHONY: force-rebuild

# rule
force-rebuild:

$(top_builddir)/external/%: force-rebuild
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) '$(@F)' -C '$(@D)'


#################
# Code Coverage #
#################

GCDA_FILES = $(patsubst %.cpp,%.gcda,$(filter %.cpp,$(SOURCES)))
GCNO_FILES = $(GCDA_FILES:.gcda=.gcno)
GCNO_FILES += $(libkmer_transformers_la_OBJECTS:.lo=.gcno)
GCNO_FILES += $(libkmer_transformers_debug_la_OBJECTS:.lo=.gcno)
GCNO_FILES += $(libkmer_reader_la_OBJECTS:.lo=.gcno)
GCNO_FILES += $(libkmer_reader_debug_la_OBJECTS:.lo=.gcno)
GCNO_FILES += $(libbijecthash_core_debug_la_OBJECTS:.lo=.gcno)
GCNO_FILES += $(libbijecthash_core_la_OBJECTS:.lo=.gcno)
GCNO_FILES += $(BijectHash_OBJECTS:.lo=.gcno)
GCNO_FILES += $(BijectHash_with_checks_OBJECTS:.lo=.gcno)
GCNO_FILES += $(BijectHash_with_cache_stats_OBJECTS:.lo=.gcno)
GCOV_FILES = $(GCDA_FILES:.gcda=.gcov)


##########################
# install/uninstall hook #
##########################

install-data-hook:
	$(AM_V_at)test "`readlink "$(DESTDIR)$(includedir)/$(LIBNAME)"`" == "$(install_dirname)"	 \
	|| ( cd "$(DESTDIR)$(includedir)" && $(LN_S) -f -n "$(install_dirname)" "$(LIBNAME)" )
	$(AM_V_at)test "`readlink "$(DESTDIR)$(libdir)/$(LIBNAME)"`" == "$(install_dirname)" \
	|| ( cd "$(DESTDIR)$(libdir)" && $(LN_S) -f -n "$(install_dirname)" "$(LIBNAME)" )

uninstall-hook:
	$(AM_V_at)test "`readlink "$(DESTDIR)$(includedir)/$(LIBNAME)"`" != "$(install_dirname)" \
	|| rm -f "$(DESTDIR)$(includedir)/$(LIBNAME)"
	$(AM_V_at)rm -rf "$(DESTDIR)$(includedir)/$(install_dirname)"
	$(AM_V_at)test "`readlink "$(DESTDIR)$(libdir)/$(LIBNAME)"`" != "$(install_dirname)" \
	|| rm -f "$(DESTDIR)$(libdir)/$(LIBNAME)"
	$(AM_V_at)rm -rf "$(DESTDIR)$(libdir)/$(install_dirname)"


####################
# Cleaning targets #
####################

MOSTLYCLEANFILES = *~
CLEANFILES = *~ $(GCDA_FILES) $(GCNO_FILES) $(GCOV_FILES)
DISTCLEANFILES = *~
MAINTAINERCLEANFILES = *~
