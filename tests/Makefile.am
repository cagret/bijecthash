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

AM_CXXFLAGS = \
  -I$(top_builddir) \
  -I$(top_srcdir)/src \
  -DRESOURCES_DIR='"@top_srcdir@/resources/"'

AM_LDFLAGS = -avoid-version -shared -rpath $(libdir)

check_LTLIBRARIES =
check_PROGRAMS =
check_SCRIPTS =
TESTS =
XFAIL_TESTS =
EXTRA_DIST = $(check_SCRIPTS)


####################################
# kmer-reader library test program #
####################################

check_PROGRAMS += test_kmer_reader
TESTS += test_kmer_reader

test_kmer_reader_SOURCES = test_kmer_reader.cpp
test_kmer_reader_LDADD = $(top_builddir)/src/libkmer-reader-debug.la


###############################
# LcpStats class test program #
###############################

check_PROGRAMS += test_lcp_stats
TESTS += test_lcp_stats

test_lcp_stats_SOURCES = test_lcp_stats.cpp
test_lcp_stats_LDADD = \
  $(top_builddir)/src/libbijecthash-core-debug.la \
  $(top_builddir)/src/libkmer-reader-debug.la


#############################
# test program dependencies #
#############################

force_create:

$(top_builddir)/src/lib%-debug.la: force_create
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) "$(@F)" -C "$(@D)"


#################
# Code Coverage #
#################

GCDA_FILES = $(patsubst %.cpp,%.gcda,$(filter %.cpp %.c,$(SOURCES)))
GCNO_FILES = $(GCDA_FILES:.gcda=.gcno)
GCOV_FILES = $(GCDA_FILES:.gcda=.gcov)


####################
# Cleaning targets #
####################

MOSTLYCLEANFILES = *~
CLEANFILES = *~ $(GCDA_FILES) $(GCNO_FILES) $(GCOV_FILES)
DISTCLEANFILES = *~
MAINTAINERCLEANFILES = *~
