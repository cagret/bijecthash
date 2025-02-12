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

EXTRA_DIST = plugin_compilation.dox UTF8-fix.sty

@DX_RULES@

EXTRA_DATA = \
	$(DX_CLEAN_HTML) \
	$(DX_CLEAN_CHM) \
	$(DX_CLEAN_CHI) \
	$(DX_CLEAN_RTF) \
	$(DX_CLEAN_XML) \
	$(DX_CLEAN_PS) \
	$(DX_CLEAN_PDF)

doxygen-doc:

all-local: doxygen-doc

#############
# Man pages #
#############

if BUILD_DOC_MAN

man3_MANS = @DX_DOCDIR@/man/man3/bijecthash_*

$(man3_MANS): doxygen-doc

endif


########
# HTML #
########

install-data-hook: $(EXTRA_DATA)
	@$(NORMAL_INSTALL)
	$(AM_V_at)test -n "$(docdir)" || exit 1
	$(AM_V_at)for p in $(EXTRA_DATA); do \
	  if test -f "$$p"; then \
	    dirname "$$p"; \
	  else \
	    find "$$p" -type d -print; \
	  fi; \
	done | $(SED) 's,^$(DX_DOCDIR),,' | \
	while read dest; do \
	  echo " $(MKDIR_P) '$(DESTDIR)$(docdir)/$$dest'"; \
	  $(MKDIR_P) "$(DESTDIR)$(docdir)/$$dest" || exit 1; \
	done
	$(AM_V_at)list='$(EXTRA_DATA)'; \
	for p in $$list; do \
	  if test -f "$$p"; then \
	    echo "$$p"; \
	  else \
	    find "$$p" -type f -print; \
	  fi; \
	done | \
	while read file; do \
	  dest="$(DESTDIR)$(docdir)/$$(dirname "$${file#$(DX_DOCDIR)/}")"; \
	  echo " $(INSTALL_DATA) '$$file' '$$dest'"; \
	  $(INSTALL_DATA) "$$file" "$$dest" || exit $$?; \
	done

uninstall-hook:
	@$(NORMAL_UNINSTALL)
	$(AM_V_at)list='$(EXTRA_DATA)'; test -n "$(docdir)" || list=; \
	files=`for p in $$list; do echo "$$p"; done | sed -e 's|^.*/||'`; \
	dir='$(DESTDIR)$(docdir)'; \
	test -z "$$files" \
	|| { test ! -d "$$dir" && test ! -f "$$dir" && test ! -r "$$dir"; } \
	|| { echo " ( cd '$$dir' && rm -rf" $$files ")"; \
	         $(am__cd) "$$dir" && rm -rf $$files; }


####################
# Cleaning targets #
####################

MOSTLYCLEANFILES = *~ $(DX_CLEANFILES)
CLEANFILES = *~
DISTCLEANFILES = *~
MAINTAINERCLEANFILES = *~
