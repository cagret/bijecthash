###############################################################################
#                                                                             #
#  Copyright © 2025      -- LIRMM/CNRS/UM                                     #
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

@GCOVR_CODE_COVERAGE_RULES@

# Ignore external libraries/headers in coverage reports.
GCOVR_CORE_OPTS = 							\
  --root "$(top_builddir)"						\
  --exclude-unreachable-branches					\
  --exclude '$(GCOVR_TESTS_DIRECTORY)/'					\
  --exclude '/(.*/)?external/'						\
  --exclude '/(.*/)?variant_filter_((scanner)|(parser)).((cpp)|(h))'	\
  --filter '/(.*/)?src/'

CODE_COVERAGE_LCOV_RMOPTS_DEFAULT  = /usr/* $(GCOVR_TESTS_DIRECTORY) */external/*

# Ignore (and suppress) `unused` and `deprecated` warnings emitted by
# the `lcov` program.
CODE_COVERAGE_LCOV_SHOPTS = $(CODE_COVERAGE_LCOV_SHOPTS_DEFAULT) --ignore-errors unused,unused,deprecated,deprecated,inconsistent,inconsistent,mismatch,mismatch

# Ignore (and suppress) `deprecated` warnings emitted by the `genhtml`
# program.
CODE_COVERAGE_GENHTML_OPTIONS = $(CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT) --ignore-errors deprecated,deprecated,inconsistent,inconsistent,range,range

include $(top_srcdir)/aminclude_static.am

clean-local: coverage-clean

distclean-local: coverage-distclean

####################
# Cleaning targets #
####################

MOSTLYCLEANFILES = *~
CLEANFILES = *~
DISTCLEANFILES = *~
MAINTAINERCLEANFILES = *~
