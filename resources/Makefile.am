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

GENERATED_FASTA_FILES = sequences_*.fa
INCLUDED_DATA_FILES = \
  example1.fa example1.fq \
  example2.fa example2.fq

dist_pkgdata_DATA = $(STATS_FILE) $(INCLUDED_DATA_FILES)

@CODE_STATISTICS_RULES@

all-local: stats
clean-local: clean-stats

####################################
# Generation of random Fasta files #
####################################

# named target
.PHONY: random_fasta_files
random_fasta_files: $(GENERATED_FASTA_FILES)

seq_py = $(seq_py_@AM_V@)
seq_py_ = $(seq_py_@AM_DEFAULT_V@)
seq_py_0 = @echo "  RAND-SEQ-GEN " $@;
seq_py_1 =

# rule
$(GENERATED_FASTA_FILES):
	@$(AM_V_P) && echo "Generate random fasta files" || true
	$(seq_py)./seq.py


####################
# Cleaning targets #
####################

MOSTLYCLEANFILES = *~
CLEANFILES = *~ $(GENERATED_FASTA_FILES)
DISTCLEANFILES = *~
MAINTAINERCLEANFILES = *~
