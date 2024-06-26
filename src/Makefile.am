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


########################
# Global configuration #
########################

AM_CXXFLAGS =				\
  -I$(top_srcdir)			\
  -DCXX="\"$(CXX)\""                    \
  -DARCH="\"$(build_cpu)\""             \
  -DOS="\"$(build_os)\""                \
  -DPACKAGE_DATADIR="\"$(pkgdatadir)\""

install_dirname = $(PACKAGE_NAME)-$(PACKAGE_VERSION)
pkgincludedir = $(includedir)/$(install_dirname)

pkginclude_HEADERS =			\
  bwt_transformer.hpp			\
  canonical_transformer.hpp		\
  composition_transformer.hpp		\
  gab_transformer.hpp			\
  identity_transformer.hpp		\
  inthash_transformer.hpp		\
  lyndon_transformer.hpp		\
  minimizer_transformer.hpp		\
  permutation_bit_transformer.hpp	\
  permutation_transformer.hpp		\
  transformer.hpp			\
  circular_queue.hpp			\
  file_reader.hpp			\
  kmer_collector.hpp			\
  kmer_index.hpp			\
  kmer_processor.hpp			\
  locker.hpp



# Bug fix for path with spaces
bindir="@bindir@"
libdir="@libdir@"
# End of bug fix

lib_LTLIBRARIES =
noinst_LTLIBRARIES =
check_LTLIBRARIES =
bin_PROGRAMS =


#############################
# kmer-transformers library #
#############################

lib_LTLIBRARIES += libkmer-transformers.la

libkmer_transformers_ladir = $(abs_srcdir)
libkmer_transformers_la_SOURCES = \
  bwt_transformer.cpp bwt_transformer.hpp				\
  canonical_transformer.cpp canonical_transformer.hpp			\
  common.hpp								\
  composition_transformer.cpp composition_transformer.hpp		\
  gab_transformer.cpp gab_transformer.hpp				\
  identity_transformer.cpp identity_transformer.hpp			\
  inthash.c inthash.h							\
  inthash_transformer.cpp inthash_transformer.hpp			\
  lyndon_transformer.cpp lyndon_transformer.hpp				\
  minimizer_transformer.cpp minimizer_transformer.hpp			\
  permutation_bit_transformer.cpp permutation_bit_transformer.hpp	\
  permutation_transformer.cpp permutation_transformer.hpp		\
  transformer.cpp transformer.hpp

libkmer_transformers_la_configdir    = $(libdir)/$(install_dirname)/include
libkmer_transformers_la_config_DATA  = $(CONFIG_HEADER)
libkmer_transformers_la_LDFLAGS      = -version-info "$(LIB_VERSION)" $(AM_LDFLAGS)
libkmer_transformers_la_DEPENDENCIES = $(libkmer_transformers_la_LIBADD)


#######################
# kmer-reader library #
#######################

lib_LTLIBRARIES += libkmer-reader.la

libkmer_reader_ladir = $(abs_srcdir)
libkmer_reader_la_SOURCES = \
  circular_queue.hpp							\
  common.hpp								\
  file_reader.cpp file_reader.hpp					\
  kmer_collector.cpp kmer_collector.hpp					\
  kmer_index.cpp kmer_index.hpp						\
  kmer_processor.cpp kmer_processor.hpp					\
  locker.cpp locker.hpp

libkmer_reader_la_configdir    = $(libdir)/$(install_dirname)/include
libkmer_reader_la_config_DATA  = $(CONFIG_HEADER)
libkmer_reader_la_LDFLAGS      = -version-info "$(LIB_VERSION)" $(AM_LDFLAGS)
libkmer_reader_la_DEPENDENCIES = $(libkmer_reader_la_LIBADD)


###########################
# bijecthash-core library #
###########################

noinst_LTLIBRARIES += libbijecthash-core.la

libbijecthash_core_ladir = $(abs_srcdir)
libbijecthash_core_la_SOURCES = \
  common.hpp								\
  program_options.cpp program_options.hpp				\
  settings.cpp settings.hpp

libbijecthash_core_la_configdir    = $(libdir)/$(install_dirname)/include
libbijecthash_core_la_config_DATA  = $(CONFIG_HEADER)
libbijecthash_core_la_LDFLAGS      = -version-info "$(LIB_VERSION)" $(AM_LDFLAGS)
libbijecthash_core_la_DEPENDENCIES = $(libbijecthash_core_la_LIBADD)


##############
# BijectHash #
##############

bin_PROGRAMS += BijectHash

BijectHash_SOURCES = biject_hash.cpp

BijectHash_LDADD = libkmer-transformers.la libkmer-reader.la libbijecthash-core.la

BijectHash_LDFLAGS = -lkmer-transformers -lkmer-reader -lbijecthash-core $(AM_LDFLAGS)

BijectHash_DEPENDENCIES = $(BijectHash_LDADD)


##########################
# BijectHash_with_checks #
##########################

bin_PROGRAMS += BijectHash_with_checks

BijectHash_with_checks_SOURCES =		\
  $(BijectHash_SOURCES)				\
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


#################
# Code Coverage #
#################

GCDA_FILES =						\
  $(libkmer_transformers_la_OBJECTS:.lo=.gcda)		\
  $(libkmer_reader_la_OBJECTS:.lo=.gcda)		\
  $(libbijecthash_core_la_OBJECTS:.lo=.gcda)		\
  $(BijectHash_OBJECTS:.lo=.gcda)			\
  $(BijectHash_with_checks_OBJECTS:.lo=.gcda)		\
  $(BijectHash_with_cache_stats_OBJECTS:.lo=.gcda)
GCNO_FILES = $(GCDA_FILES:.gcda=.gcno)
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
