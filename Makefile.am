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

SUBDIRS = resources src tests extra

.PHONY: always-build

# Fake target to force some rule to be applied
always-build:

if BUILD_DOC

  SUBDIRS += doc

if MAKE_RELEASE

  SUBDIRS += packaging

endif

endif

dist_pkgdata_DATA = \
  AUTHORS ChangeLog README.md \
  LICENSE.md

EXTRA_DIST = \
  COPYING INSTALL NEWS \
  config/fix-timestamp.sh \
  config/version

ACLOCAL_AMFLAGS = -I config/m4


########################
# Build debian package #
########################

.PHONY: deb

if BUILD_DOC

if MAKE_RELEASE

# named target
deb: @PACKAGE@_@PACKAGE_VERSION@_@ARCH@.deb

# rule
@PACKAGE@_@PACKAGE_VERSION@_@ARCH@.deb: always-build
	@if [ "@DEB_BUILDER@" == "" ]; then \
	  echo "You need to install a debian package builder and to reconfigure this package.";\
	  exit 1;\
	fi
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) dist
	$(am__remove_distdir)
	GZIP=$(GZIP_ENV) gunzip -c $(distdir).tar.gz | $(AMTAR) xf -
	$(AM_V_at)$(MKDIR_P) $(distdir)/_build
	$(AM_V_at)$(MKDIR_P) $(distdir)/_inst/_deb/$(prefix)
	$(AM_V_at)chmod a-w $(distdir)
	$(AM_V_at)dc_install_base=`$(am__cd) $(distdir)/_inst/_deb/$(prefix) && pwd | $(SED) -e 's,^[^:\\/]:[\\/],/,'` \
	  && dc_destdir="$${TMPDIR-/tmp}/am-dc-$$$$/" \
	  && cd $(distdir)/_build \
	  && HACKED_PREFIX=$(prefix) ../configure --srcdir=.. --prefix="$$dc_install_base" CC="$(CC)" CXX="$(CXX)" \
	    $(DISTCHECK_CONFIGURE_FLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) localedir=$$localedir \
	  && $(MAKE) $(AM_MAKEFLAGS) check \
	  && $(MAKE) $(AM_MAKEFLAGS) install \
	  && $(MAKE) $(AM_MAKEFLAGS) installcheck \
	  && cd ../_inst/_deb \
	  && cp "$(abs_top_builddir)/resources/@PACKAGE@-@PACKAGE_VERSION@.stats" "./$(pkgdatadir)/" \
	  && find . -type f -exec md5sum {} \; | $(SED) "s,\.\/,,g" > ../md5sums \
          && $(MKDIR_P) DEBIAN \
	  && mv ../md5sums DEBIAN/ \
	  && cp ../../_build/packaging/postinst DEBIAN/ \
	  && chmod 755 DEBIAN/postinst \
	  && cp ../../_build/packaging/prerm DEBIAN/ \
	  && chmod 755 DEBIAN/prerm \
	  && $(SED) "s/@INSTALLED_SIZE@/`du -ck . | tail -1 | cut -f1`/" \
	       ../../_build/packaging/control > DEBIAN/control \
	  && cd ../ \
	  && dpkg -b _deb $@ \
	  && mv $@ ../../ \
	  && rm -f "$$dc_install_base/DEBIAN/control" \
	  && rm -f "$$dc_install_base/DEBIAN/prerm" \
	  && rm -f "$$dc_install_base/DEBIAN/postinst" \
	  && cd ../_build \
	  && $(MAKE) $(AM_MAKEFLAGS) uninstall \
	  && $(MAKE) $(AM_MAKEFLAGS) distuninstallcheck_dir="$$dc_install_base" \
	        distuninstallcheck \
	  && chmod -R a-w "$$dc_install_base" \
	  && ({ \
	       (cd ../.. && $(mkinstalldirs) "$$dc_destdir") \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" install \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" uninstall \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" \
	            distuninstallcheck_dir="$$dc_destdir" distuninstallcheck; \
	      } \
	      || { rm -rf "$$dc_destdir"; exit 1; }) \
	  && rm -rf "$$dc_destdir" \
	  && $(MAKE) $(AM_MAKEFLAGS) distcleancheck
	$(am__remove_distdir)
	@echo "$@ is ready for distribution" | \
	  $(SED) 'h;s/./=/g;p;x;p;x'

else # if not MAKE_RELEASE (but BUILD_DOC)

if CODE_COVERAGE_ENABLED

# Fake target to abort packages construction
deb:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Release construction is disabled since code coverage is enabled."
	@echo "Consider re-run the configure script without --enable-code-coverage"
	@echo "option (or using explicit --disable-code-coverage option)."
	@echo
	@echo "====================================================================="
	@echo
	@false

else # if not CODE_COVERAGE_ENABLED (and not MAKE_RELEASE but BUILD_DOC)

deb:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Release construction is disabled. Your local system seems to miss"
	@echo "the 'dpkg' command."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif

endif

else # if not BUILD_DOC

# Fake target to abort packages construction
deb:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Release construction is disabled. Your local system probably misses"
	@echo "one of the 'doxygen' or 'pdftex' command."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif


#################
# Code Coverage #
#################

# This can be overriden by environment variable
CODE_COVERAGE_OUTPUT_TOP_DIRECTORY ?= $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage

CODE_COVERAGE_OUTPUT_DIRECTORY = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/html-legacy
CODE_COVERAGE_OUTPUT_FILE = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage-html-legacy.info
CODE_COVERAGE_BRANCH_COVERAGE = 1
CODE_COVERAGE_LCOV_RMOPTS_DEFAULT = --remove "$(CODE_COVERAGE_OUTPUT_FILE).tmp" "/usr/*"
CODE_COVERAGE_LCOV_RMOPTS_DEFAULT += --remove "$(CODE_COVERAGE_OUTPUT_FILE).tmp" "*/tests/*"
# Trick to merge headers accessed from symbolic link and headers
# accessed directly from src directory.
abs_top_srcdir_canonical = $(shell readlink -f "$(abs_top_srcdir)")
GENHTML = $(SED) -i 's,$(abs_top_builddir)/sphinx++,$(abs_top_srcdir_canonical)/src,' '$(CODE_COVERAGE_OUTPUT_FILE)' && @GENHTML@

include $(top_srcdir)/aminclude_static.am

code-coverage-capture-hook: coverage-dir

coverage-legacy: code-coverage-capture

coverage: $(DEFAULT_CODE_COVERAGE_TARGETS)

DEFAULT_CODE_COVERAGE_TARGETS   =

coverage-all: $(AVAILABLE_CODE_COVERAGE_TARGETS)

AVAILABLE_CODE_COVERAGE_TARGETS = coverage-legacy

.PHONY: code-coverage-clean coverage-clean code-coverage-dist-clean	\
  coverage-all coverage-dir coverage-legacy coverage-summary		\
  coverage-text coverage-xml coverage-html coverage-json		\
  coverage-sonarqube coverage-cobertura coverage-coveralls		\
  coverage-csv coverage-clean-dir coverage-clean-summary		\
  coverage-clean-text coverage-clean-xml coverage-clean-html		\
  coverage-clean-json coverage-clean-sonarqube				\
  coverage-clean-cobertura coverage-clean-coveralls			\
  coverage-clean-csv coverage

if CODE_COVERAGE_ENABLED

distclean-local: code-coverage-dist-clean

if GCOVR_ENABLED

clean-local: coverage-clean

coverage-clean: code-coverage-clean				 \
  coverage-clean-summary coverage-clean-text			\
  coverage-clean-xml coverage-clean-html coverage-clean-json	\
  coverage-clean-sonarqube coverage-clean-cobertura		\
  coverage-clean-coveralls coverage-clean-csv
	$(AM_V_at)find $(top_builddir) -name "*.gcov" -delete
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) coverage-clean-dir

.NOTPARALLEL:

code_coverage = $(code_coverage_@AM_V@)
code_coverage_ = $(code_coverage_@AM_DEFAULT_V@)
code_coverage_0 = @echo "  GCOVR   " $@;
code_coverage_1 =

GCOVR_CORE_OPTS =			\
  --gcov-executable "$(GCOV)"		\
  --root "$(top_srcdir)"		\
  --exclude-unreachable-branches	\
  --exclude '^(.*/)?tests/'		\
  --filter '^(.*/)?src/'

GCOVR_OPTS = $(GCOVR_CORE_OPTS) --use-gcov-files --keep

### Ensure tests were run

# rule
check_ok = tests/test-suite.log
$(check_ok): always-build
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) $(@F) -C $(@D)

gcov_files:
	$(code_coverage)$(GCOVR) $(GCOVR_CORE_OPTS) -keep
	$(AM_V_at)test "$(abs_top_builddir)" = "$(abs_top_srcdir)" \
	|| find $(top_builddir) \
	  -name "*.gcov" \
	  -exec $(SED) -i 's,0:Source:../$(LIBNAME)/,0:Source:$(top_srcdir)/../src/,' "{}" \;


### Create coverage report directory

# named target and rule

coverage-dir: $(check_ok)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) gcov_files
	$(AM_V_at)$(MKDIR_P) "$(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)"

DEFAULT_CODE_COVERAGE_TARGETS   += coverage-dir
AVAILABLE_CODE_COVERAGE_TARGETS += coverage-dir

coverage-clean-dir:
	$(AM_V_at)if test -d "$(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)"; then \
	  echo "Removing any existing coverage directory"; \
	  rmdir "$(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)"; \
	fi

### Generate all coverage reports

### Code coverage summary

# named target

CODE_COVERAGE_SUMMARY_OUTPUT_FILE = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/summary.txt

coverage-summary: $(CODE_COVERAGE_SUMMARY_OUTPUT_FILE)

DEFAULT_CODE_COVERAGE_TARGETS   += coverage-summary
AVAILABLE_CODE_COVERAGE_TARGETS += coverage-summary

coverage-clean-summary:
	@if test -f "$(CODE_COVERAGE_SUMMARY_OUTPUT_FILE)"; then \
	  echo "Cleaning code coverage summary"; \
	  rm -f "$(CODE_COVERAGE_SUMMARY_OUTPUT_FILE)"; \
	fi

# rule
$(CODE_COVERAGE_SUMMARY_OUTPUT_FILE): $(check_ok) | coverage-dir
	@$(AM_V_P) && echo "Generating code coverage summary" || true
	$(code_coverage)$(GCOVR) $(GCOVR_OPTS) \
	  --print-summary \
	  --output "$@.tmp" > "$@"
	$(AM_V_at)rm -f  "$@.tmp"
	@$(AM_V_P) && echo "=> Code coverage text report (with summary) is available at '$@'." || true


### Code coverage text report
if GCOVR_TXT_ENABLED

# named target

CODE_COVERAGE_TEXT_OUTPUT_FILE = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/report.txt

coverage-text: $(CODE_COVERAGE_TEXT_OUTPUT_FILE)

DEFAULT_CODE_COVERAGE_TARGETS   += coverage-text
AVAILABLE_CODE_COVERAGE_TARGETS += coverage-text

coverage-clean-text:
	@if test -f "$(CODE_COVERAGE_TEXT_OUTPUT_FILE)"; then \
	  echo "Cleaning code coverage text report"; \
	  rm -f "$(CODE_COVERAGE_TEXT_OUTPUT_FILE)"; \
	fi

# rule
$(CODE_COVERAGE_TEXT_OUTPUT_FILE): $(check_ok) | coverage-dir
	@$(AM_V_P) && echo "Generating code coverage text report" || true
	$(code_coverage)$(GCOVR) $(GCOVR_OPTS) \
	  --output "$@"
	@$(AM_V_P) && echo "=> Code coverage text report is available at '$@'." || true

else

coverage-text:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Code coverage text reports using 'gcovr' is disabled since your gcovr"
	@echo "version is too old."
	@echo "Consider updating your gcovr program on your system (and re-run the"
	@echo "configure script) or re-run the configure script using the"
	@echo "--with-gcovr=<path> option in order to use a more recent version of"
	@echo "the 'gcovr' program."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif


### Code coverage XML report
if GCOVR_XML_ENABLED

# named target

CODE_COVERAGE_XML_OUTPUT_FILE = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/report.xml

coverage-xml: $(CODE_COVERAGE_XML_OUTPUT_FILE)

AVAILABLE_CODE_COVERAGE_TARGETS += coverage-xml

coverage-clean-xml:
	@if test -f "$(CODE_COVERAGE_XML_OUTPUT_FILE)"; then \
	  echo "Cleaning code coverage XML report"; \
	  rm -f "$(CODE_COVERAGE_XML_OUTPUT_FILE)"; \
	fi

# rule
$(CODE_COVERAGE_XML_OUTPUT_FILE): $(check_ok) | coverage-dir
	@$(AM_V_P) && echo "Generating code coverage XML report" || true
	$(code_coverage)$(GCOVR) $(GCOVR_OPTS) \
	  --xml --xml-pretty \
	  --output "$@"
	@$(AM_V_P) && echo "=> Code coverage XML report is available at '$@'." || true

else

coverage-xml:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Code coverage XML reports using 'gcovr' is disabled since your gcovr"
	@echo "version is too old."
	@echo "Consider updating your gcovr program on your system (and re-run the"
	@echo "configure script) or re-run the configure script using the"
	@echo "--with-gcovr=<path> option in order to use a more recent version of"
	@echo "the 'gcovr' program."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif


### Code coverage HTML report
if GCOVR_HTML_ENABLED

CODE_COVERAGE_OUTPUT_HTML_DIRECTORY =  $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/html
CODE_COVERAGE_OUTPUT_HTML_FILE = $(CODE_COVERAGE_OUTPUT_HTML_DIRECTORY)/report.html

# named target

coverage-html: $(CODE_COVERAGE_OUTPUT_HTML_FILE)

DEFAULT_CODE_COVERAGE_TARGETS   += coverage-html
AVAILABLE_CODE_COVERAGE_TARGETS += coverage-html

coverage-clean-html:
	@if test -d "$(CODE_COVERAGE_OUTPUT_HTML_DIRECTORY)"; then \
	  echo "Cleaning code coverage HTML report"; \
	  rm -f $(CODE_COVERAGE_OUTPUT_HTML_FILE:.html=.*.html); \
	  rm -f $(CODE_COVERAGE_OUTPUT_HTML_FILE); \
	  rm -f $(CODE_COVERAGE_OUTPUT_HTML_FILE:.html=.css); \
	  rm -rf $(CODE_COVERAGE_OUTPUT_HTML_DIRECTORY); \
	fi

# rule
$(CODE_COVERAGE_OUTPUT_HTML_FILE): $(check_ok) | coverage-dir
	@$(AM_V_P) && echo "Generating code coverage HTML report" || true
	$(AM_V_at)$(MKDIR_P) "$(CODE_COVERAGE_OUTPUT_HTML_DIRECTORY)"
	$(code_coverage)$(GCOVR) $(GCOVR_OPTS) \
	  --html --html-details --html-title "GCOVR - Code Coverage Report" \
	  --merge-mode-functions strict \
          --output "$@"
	@$(AM_V_P) && echo "=> Code coverage HTML report is available at '$@'." || true

else

coverage-html:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Code coverage HTML reports using 'gcovr' is disabled since your gcovr"
	@echo "version is too old."
	@echo "Consider updating your gcovr program on your system (and re-run the"
	@echo "configure script) or re-run the configure script using the"
	@echo "--with-gcovr=<path> option in order to use a more recent version of"
	@echo "the 'gcovr' program."
	@echo
	@echo "====================================================================="
	@echo
	@false
DEFAULT_CODE_COVERAGE_TARGETS += coverage-legacy

endif

### Code coverage JSON report
if GCOVR_JSON_ENABLED

# named target

CODE_COVERAGE_JSON_OUTPUT_FILE = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/report.json

coverage-json: $(CODE_COVERAGE_JSON_OUTPUT_FILE)

AVAILABLE_CODE_COVERAGE_TARGETS += coverage-json

coverage-clean-json:
	@if test -f "$(CODE_COVERAGE_JSON_OUTPUT_FILE)"; then \
	  echo "Cleaning code coverage JSON report"; \
	  rm -f "$(CODE_COVERAGE_JSON_OUTPUT_FILE)"; \
	fi

# rule
$(CODE_COVERAGE_JSON_OUTPUT_FILE): $(check_ok) | coverage-dir
	@$(AM_V_P) && echo "Generating code coverage JSON report" || true
	$(code_coverage)$(GCOVR) $(GCOVR_OPTS) \
	  --json --json-pretty \
	  --output "$@"
	@$(AM_V_P) && echo "=> Code coverage JSON report is available at '$@'." || true

else

coverage-json:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Code coverage JSON reports using 'gcovr' is disabled since your gcovr"
	@echo "version is too old."
	@echo "Consider updating your gcovr program on your system (and re-run the"
	@echo "configure script) or re-run the configure script using the"
	@echo "--with-gcovr=<path> option in order to use a more recent version of"
	@echo "the 'gcovr' program."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif

### Code coverage Sonarqube XML report
if GCOVR_SONARQUBE_ENABLED

# named target

CODE_COVERAGE_SONARQUBE_OUTPUT_FILE = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/sonarqube-report.xml

coverage-sonarqube: $(CODE_COVERAGE_SONARQUBE_OUTPUT_FILE)

AVAILABLE_CODE_COVERAGE_TARGETS += coverage-sonarqube

coverage-clean-sonarqube:
	@if test -f "$(CODE_COVERAGE_SONARQUBE_OUTPUT_FILE)"; then \
	  echo "Cleaning code coverage Sonarqube XML report"; \
	  rm -f "$(CODE_COVERAGE_SONARQUBE_OUTPUT_FILE)"; \
	fi

# rule
$(CODE_COVERAGE_SONARQUBE_OUTPUT_FILE): $(check_ok) | coverage-dir
	@$(AM_V_P) && echo "Generating code coverage Sonarqube XML report" || true
	$(code_coverage)$(GCOVR) $(GCOVR_OPTS) \
	  --json --json-pretty \
	  --output "$@"
	@$(AM_V_P) && echo "=> Code coverage Sonarqube XML report is available at '$@'." || true

else

coverage-sonarqube:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Code coverage Sonarqube XML reports using 'gcovr' is disabled since"
	@echo "your gcovr version is too old."
	@echo "Consider updating your gcovr program on your system (and re-run the"
	@echo "configure script) or re-run the configure script using the"
	@echo "--with-gcovr=<path> option in order to use a more recent version of"
	@echo "the 'gcovr' program."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif

### Code coverage Cobertura XML report
if GCOVR_COBERTURA_ENABLED

# named target

CODE_COVERAGE_COBERTURA_OUTPUT_FILE = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/cobertura-report.xml

coverage-cobertura: $(CODE_COVERAGE_COBERTURA_OUTPUT_FILE)

AVAILABLE_CODE_COVERAGE_TARGETS += coverage-cobertura

coverage-clean-cobertura:
	@if test -f "$(CODE_COVERAGE_COBERTURA_OUTPUT_FILE)"; then \
	  echo "Cleaning code coverage Cobertura XML report"; \
	  rm -f "$(CODE_COVERAGE_COBERTURA_OUTPUT_FILE)"; \
	fi

# rule
$(CODE_COVERAGE_COBERTURA_OUTPUT_FILE): $(check_ok) | coverage-dir
	@$(AM_V_P) && echo "Generating code coverage Cobertura XML report" || true
	$(code_coverage)$(GCOVR) $(GCOVR_OPTS) \
	  --cobertura --cobertura-pretty \
	  --output "$@"
	@$(AM_V_P) && echo "=> Code coverage Cobertura XML report is available at '$@'." || true
else

if GCOVR_XML_ENABLED

CODE_COVERAGE_COBERTURA_OUTPUT_FILE = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/cobertura-report.xml

coverage-cobertura: $(CODE_COVERAGE_COBERTURA_OUTPUT_FILE)

AVAILABLE_CODE_COVERAGE_TARGETS += coverage-cobertura

$(CODE_COVERAGE_COBERTURA_OUTPUT_FILE): $(CODE_COVERAGE_XML_OUTPUT_FILE)
	@$(AM_V_P) && echo "Using XML report as code coverage Cobertura report" || true
	@$(code_coverage)cp "$<" "$@"
	@$(AM_V_P) && echo "=> Code coverage Cobertura XML report is available at '$@'." || true

AVAILABLE_CODE_COVERAGE_TARGETS += coverage-cobertura

coverage-clean-cobertura:
	@if test -f "$(CODE_COVERAGE_COBERTURA_OUTPUT_FILE)"; then \
	  echo "Cleaning code coverage Cobertura XML report"; \
	  rm -f "$(CODE_COVERAGE_COBERTURA_OUTPUT_FILE)"; \
	fi

else

coverage-cobertura:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Code coverage Cobertura XML reports using 'gcovr' is disabled since"
	@echo "your gcovr version is too old."
	@echo "Consider updating your gcovr program on your system (and re-run the"
	@echo "configure script) or re-run the configure script using the"
	@echo "--with-gcovr=<path> option in order to use a more recent version of"
	@echo "the 'gcovr' program."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif

endif

### Code coverage Coveralls JSON report
if GCOVR_COVERALLS_ENABLED

# named target

CODE_COVERAGE_COVERALLS_OUTPUT_FILE = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/coveralls-report.json

coverage-coveralls: $(CODE_COVERAGE_COVERALLS_OUTPUT_FILE)

AVAILABLE_CODE_COVERAGE_TARGETS += coverage-coveralls

coverage-clean-coveralls:
	@if test -f "$(CODE_COVERAGE_COVERALLS_OUTPUT_FILE)"; then \
	  echo "Cleaning code coverage Coveralls JSON report"; \
	  rm -f "$(CODE_COVERAGE_COVERALLS_OUTPUT_FILE)"; \
	fi

# rule
$(CODE_COVERAGE_COVERALLS_OUTPUT_FILE): $(check_ok) | coverage-dir
	@$(AM_V_P) && echo "Generating code coverage Coveralls JSON report" || true
	$(code_coverage)$(GCOVR) $(GCOVR_OPTS) \
	  --coveralls --coveralls-pretty \
	  --output "$@"
	@$(AM_V_P) && echo "=> Code coverage Coveralls JSON report is available at '$@'." || true

else

coverage-coveralls:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Code coverage Coveralls reports using 'gcovr' is disabled since your"
	@echo "gcovr version is too old."
	@echo "Consider updating your gcovr program on your system (and re-run the"
	@echo "configure script) or re-run the configure script using the"
	@echo "--with-gcovr=<path> option in order to use a more recent version of"
	@echo "the 'gcovr' program."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif

### Code coverage CSV report
if GCOVR_CSV_ENABLED

# named target

CODE_COVERAGE_CSV_OUTPUT_FILE = $(CODE_COVERAGE_OUTPUT_TOP_DIRECTORY)/report.csv

coverage-csv: $(CODE_COVERAGE_CSV_OUTPUT_FILE)

AVAILABLE_CODE_COVERAGE_TARGETS += coverage-csv

coverage-clean-csv:
	@if test -f "$(CODE_COVERAGE_CSV_OUTPUT_FILE)"; then \
	  echo "Cleaning code coverage CSV report"; \
	  rm -f "$(CODE_COVERAGE_CSV_OUTPUT_FILE)"; \
	fi

# rule
$(CODE_COVERAGE_CSV_OUTPUT_FILE): $(check_ok) | coverage-dir
	@$(AM_V_P) && echo "Generating code coverage CSV report" || true
	$(code_coverage)$(GCOVR) $(GCOVR_OPTS) \
	  --csv \
	  --output "$@"
	@$(AM_V_P) && echo "=> Code coverage CSV report is available at '$@'." || true

else

coverage-csv:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Code coverage CSV reports using 'gcovr' is disabled since your gcovr"
	@echo "version is too old."
	@echo "Consider updating your gcovr program on your system (and re-run the"
	@echo "configure script) or re-run the configure script using the"
	@echo "--with-gcovr=<path> option in order to use a more recent version of"
	@echo "the 'gcovr' program."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif


else # if not GCOVR_ENABLED (but CODE_COVERAGE_ENABLED)

clean-local: code-coverage-clean

coverage-clean coverage-all coverage-dir coverage-summary		\
coverage-text coverage-xml coverage-html coverage-json			\
coverage-sonarqube coverage-cobertura coverage-cobertura		\
coverage-coveralls coverage-csv coverage-clean-summary			\
coverage-clean-text coverage-clean-xml coverage-clean-html		\
coverage-clean-json coverage-clean-sonarqube coverage-clean-cobertura	\
coverage-clean-cobertura coverage-clean-coveralls coverage-clean-csv	\
coverage:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Code coverage reports using 'gcovr' is disabled since not found while"
	@echo "configure script was ran."
	@echo "Consider re-run the configure script using the --with-gcovr=<path>"
	@echo "option in order to set a correct path for the 'gcovr' program."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif

else # if not CODE_COVERAGE_ENABLED

code-coverage-clean coverage-clean code-coverage-dist-clean		\
coverage-all coverage-dir coverage-legacy coverage-summary		\
coverage-text coverage-xml coverage-html coverage-json			\
coverage-sonarqube coverage-cobertura coverage-coveralls coverage-csv	\
coverage-clean-summary coverage-clean-text coverage-clean-xml		\
coverage-clean-html coverage-clean-json coverage-clean-sonarqube	\
coverage-clean-cobertura coverage-clean-coveralls coverage-clean-csv	\
coverage:
	@echo
	@echo "====================================================================="
	@echo
	@echo "Code coverage computation is disabled."
	@echo "Consider re-run the configure script using the --enable-code-coverage"
	@echo "option."
	@echo
	@echo "====================================================================="
	@echo
	@false

endif

####################
# Cleaning targets #
####################

MOSTLYCLEANFILES=*~ config/*~
CLEANFILES=*~ config/*~
DISTCLEANFILES=*~ $(distdir).tar.gz
DISTCLEANFILES+=@PACKAGE@_@PACKAGE_VERSION@_@ARCH@.deb
DISTCLEANFILES+=$(LIB_SYMLINK)
MAINTAINERCLEANFILES=*~ config/*~
