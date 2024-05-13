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

STATS_FILE_BASENAME = @PACKAGE@-@PACKAGE_VERSION@
CLOC_FILE = $(STATS_FILE_BASENAME).cloc
OHCOUNT_FILE = $(STATS_FILE_BASENAME).ohcount
LIST_FILE = $(STATS_FILE_BASENAME).filelist
STATS_FILE = $(STATS_FILE_BASENAME).stats

GENERATED_FASTA_FILES = sequences_*.fa
INCLUDED_DATA_FILES = \
  example1.fa example1.fq \
  example2.fa example2.fq

dist_pkgdata_DATA = $(STATS_FILE) $(INCLUDED_DATA_FILES)


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


############################################
# List of files used to compute statistics #
############################################

# named target
.PHONY: filelist
filelist: $(LIST_FILE)

git_ls_tree = $(git_ls_tree_@AM_V@)
git_ls_tree_ = $(git_ls_tree_@AM_DEFAULT_V@)
git_ls_tree_0 = @echo "  GIT-LS-TREE " $@;
git_ls_tree_1 =

# rule
$(LIST_FILE):
	@$(AM_V_P) && echo "Generate package file list" || true
	$(git_ls_tree)git ls-tree --name-only -r  HEAD "$(abs_top_srcdir)" \
	> "$@" || true



#####################################
# CLOC [Count Lines of Code] report #
#####################################

# named target
.PHONY: cloc
cloc: $(CLOC_FILE)

cloc = $(cloc_@AM_V@)
cloc_ = $(cloc_@AM_DEFAULT_V@)
cloc_0 = @echo "  CLOC    " $@;
cloc_1 =

# rule
if CLOC

$(CLOC_FILE): $(LIST_FILE)
	@$(AM_V_P) && echo "Generate cloc summary" || true
	$(AM_V_at)echo "CLoC summary" | sed 'h;s/./=/g;p;x;p;x' > "$@"
	$(AM_V_at)echo >> "$@"
	$(cloc)@CLOC@ --list-file="$<" --quiet | tail -n +2 >> "$@"

else

$(CLOC_FILE):
	@echo "You should install cloc (source code line counter) and reconfigure this package."
	$(AM_V_at)echo "CLoC isn't installed. No report created" > "$@"

endif


###########################################
# Ohloh's source code line counter report #
###########################################

# named target
.PHONY: ohcount
ohcount: $(OHCOUNT_FILE)

ohcount = $(ohcount_@AM_V@)
ohcount_ = $(ohcount_@AM_DEFAULT_V@)
ohcount_0 = @echo "  OHCOUNT " $@;
ohcount_1 =

# rule
if OHCOUNT

$(OHCOUNT_FILE): $(LIST_FILE)
	@$(AM_V_P) && echo "Generate OhCount summary" || true
	$(AM_V_at)echo "OhCount summary" | sed 'h;s/./=/g;p;x;p;x' > "$@"
	$(ohcount)@OHCOUNT@ $$(cat "$<") | tail -n +2 >> "$@"

else

$(OHCOUNT_FILE): $(LIST_FILE)
	@echo "You should install ohcount (source code line counter) and reconfigure this package."
	$(AM_V_at)echo "OhCount isn't installed. No report created" > "$@"

endif


#################
# Global report #
#################

# named target
.PHONY: stats update-stats

stats: $(STATS_FILE)

update-stats:
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) clean
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) stats

stats = $(stats_@AM_V@)
stats_ = $(stats_@AM_DEFAULT_V@)
stats_0 = @echo "  STATS   " $@;
stats_1 =

# rule
$(STATS_FILE): $(CLOC_FILE) $(OHCOUNT_FILE) $(LIST_FILE)
	@$(AM_V_P) && echo "Backup existing code statistics (if any)" || true
	$(AM_V_at)test ! -f "$(abs_srcdir)/$(STATS_FILE)" || cat "$(abs_srcdir)/$(STATS_FILE)" > "$(STATS_FILE).tmp"
	$(stats)echo "Generate code statistics"
	$(AM_V_at)echo "# $@ #" | sed 'h;s/./#/g;p;x;p;x' > "$@"
	$(AM_V_at)echo >> "$@"
	$(AM_V_at)cat $(CLOC_FILE) >> "$@"
	$(AM_V_at)echo >> "$@"
	$(AM_V_at)cat $(OHCOUNT_FILE) >> "$@"
	$(AM_V_at)echo >> "$@"
	$(AM_V_at)printf "%.78d\n" | tr 0 - >> "$@"
	$(AM_V_at)echo >> "$@"
	$(AM_V_at)if test ! -s "$(LIST_FILE)" -a -f "$(STATS_FILE).tmp"; then		\
	  echo "Using existing code statistics backup file since generation failed";	\
	  mv "$(STATS_FILE).tmp" "$(STATS_FILE)";					\
	fi
	@echo "Removing temporary files and backup (if any)"
	$(AM_V_at)$(RM) "$(STATS_FILE).tmp"
	$(AM_V_at)cat "$(STATS_FILE)"


####################
# Cleaning targets #
####################

MOSTLYCLEANFILES=*~
CLEANFILES=*~ $(LIST_FILE) $(CLOC_FILE) $(OHCOUNT_FILE) $(STATS_FILE) $(GENERATED_FASTA_FILES)
DISTCLEANFILES=*~
MAINTAINERCLEANFILES=*~
