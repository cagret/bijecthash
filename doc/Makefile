# Retrieve current directory
CURRENT_DIRECTORY := $(realpath $(dir $(firstword $(MAKEFILE_LIST))))

# Documentation generator
DOXYGEN_CMD := doxygen

# Configuration file
DOXYGEN_CONFIG := $(CURRENT_DIRECTORY)/Doxyfile
ifeq ($(realpath $(DOXYGEN_CONFIG)),)
  $(error "File '$(DOXYGEN_CONFIG)' not found.")
endif

# Retrieve documentation destination root
DOXYGEN_OUTPUTDIR := $(strip $(shell grep '^OUTPUT_DIRECTORY ' "$(DOXYGEN_CONFIG)" | cut -d'=' -f2))

ifeq ($(DOXYGEN_OUTPUTDIR),)
  # If nothing is specified, all documentation must be in current directory.
  DOXYGEN_OUTPUTDIR := $(CURRENT_DIRECTORY)
else
  ifeq ($(patsubst /%,,$(DOXYGEN_OUTPUTDIR)),)
    # The destination directory is an absolute path.
    DOXYGEN_OUTPUTDIR := $(abspath $(DOXYGEN_OUTPUTDIR))
  else
    # The destination directory is a relative path.
    DOXYGEN_OUTPUTDIR := $(abspath $(CURRENT_DIRECTORY)/$(DOXYGEN_OUTPUTDIR))
  endif
endif

# Documentation targets
TARGETS = 
# List of subdirectories to create
ifeq ($(DOXYGEN_OUTPUTDIR),$(CURRENT_DIRECTORY))
  TARGETS_SUBDIRS =
else
  TARGETS_SUBDIRS = "$(DOXYGEN_OUTPUTDIR)"
endif

# Retrieve if HTML documentation is to be generated
DOXYGEN_HTML := $(strip $(shell grep '^GENERATE_HTML ' "$(DOXYGEN_CONFIG)" | cut -d'=' -f2 | tr '[[:lower:]]' '[[:upper:]]'))
ifeq ($(DOXYGEN_HTML),YES)
  # Retrieve HTML documentation destination directory
  DOXYGEN_HTML_OUTPUTDIR := $(strip $(shell grep '^HTML_OUTPUT ' "$(DOXYGEN_CONFIG)" | cut -d'=' -f2))
  valid := true
  ifeq ($(DOXYGEN_HTML_OUTPUTDIR),)
    DOXYGEN_HTML_OUTPUTDIR := $(DOXYGEN_OUTPUTDIR)
    valid := false
  else
    ifeq ($(patsubst /%,,$(DOXYGEN_HTML_OUTPUTDIR)),)
      # The destination directory is an absolute path.
      DOXYGEN_HTML_OUTPUTDIR := $(abspath $(DOXYGEN_HTML_OUTPUTDIR))
    else
      # The destination directory is a relative path.
      DOXYGEN_HTML_OUTPUTDIR := $(abspath $(DOXYGEN_OUTPUTDIR)/$(DOXYGEN_HTML_OUTPUTDIR))
    endif
    subdir=$(subst $(DOXYGEN_OUTPUTDIR),,$(DOXYGEN_HTML_OUTPUTDIR))
    ifeq ($(subdir),)
      valid := false
    else
      ifeq ($(subdir),$(DOXYGEN_HTML_OUTPUTDIR))
        valid := false
      endif
    endif
  endif
  ifeq ($(valid),true)
    TARGETS += html
    ifeq ($(DOXYGEN_OUTPUTDIR),$(CURRENT_DIRECTORY))
      TARGETS_SUBDIRS += "$(DOXYGEN_HTML_OUTPUTDIR)"
    endif
  else
    $(error "Ce makefile refuse de générer de la documentation HTML si celle-ci n'est pas crée dans un sous-répertoire de '$(DOXYGEN_OUTPUTDIR)' (or votre configuration aboutirait à générer la documentation HTML dans le répertoire '$(DOXYGEN_HTML_OUTPUTDIR)'). Veuillez adapter le paramètre 'HTML_OUTPUT' du fichier '$(DOXYGEN_CONFIG)'")
  endif
endif

# Retrieve if LaTeX documentation is to be generated
DOXYGEN_LATEX := $(strip $(shell grep '^GENERATE_LATEX ' "$(DOXYGEN_CONFIG)" | cut -d'=' -f2 | tr '[[:lower:]]' '[[:upper:]]'))
ifeq ($(DOXYGEN_LATEX),YES)
  # Retrieve LaTeX documentation destination directory
  DOXYGEN_LATEX_OUTPUTDIR := $(strip $(shell grep '^LATEX_OUTPUT ' "$(DOXYGEN_CONFIG)" | cut -d'=' -f2))
  valid := true
  ifeq ($(DOXYGEN_LATEX_OUTPUTDIR),)
    DOXYGEN_LATEX_OUTPUTDIR := $(DOXYGEN_OUTPUTDIR)
    valid := false
  else
    ifeq ($(patsubst /%,,$(DOXYGEN_LATEX_OUTPUTDIR)),)
      DOXYGEN_LATEX_OUTPUTDIR := $(abspath $(DOXYGEN_LATEX_OUTPUTDIR))
    else
      DOXYGEN_LATEX_OUTPUTDIR := $(abspath $(DOXYGEN_OUTPUTDIR)/$(DOXYGEN_LATEX_OUTPUTDIR))
    endif
    subdir=$(subst $(DOXYGEN_OUTPUTDIR),,$(DOXYGEN_LATEX_OUTPUTDIR))
    ifeq ($(subdir),)
      valid := false
    else
      ifeq ($(subdir),$(DOXYGEN_LATEX_OUTPUTDIR))
        valid := false
      endif
    endif
  endif
  ifeq ($(valid),true)
    TARGETS += latex pdf
    ifeq ($(DOXYGEN_OUTPUTDIR),$(CURRENT_DIRECTORY))
      TARGETS_SUBDIRS += "$(DOXYGEN_LATEX_OUTPUTDIR)" "$(DOXYGEN_OUTPUTDIR)/refman.pdf"
    endif
    TARGETS_SUBDIRS += "$(DOXYGEN_OUTPUTDIR)/refman.pdf"
  else
    $(error "Ce makefile refuse de générer de la documentation LaTeX (et donc pdf) si celle-ci n'est pas crée dans un sous-répertoire de '$(DOXYGEN_OUTPUTDIR)' (or votre configuration aboutirait à générer la documentation LaTeX dans le répertoire '$(DOXYGEN_LATEX_OUTPUTDIR)'). Veuillez adapter le paramètre 'LaTeX_OUTPUT' du fichier '$(DOXYGEN_CONFIG)'")
  endif
endif

# Hide compilation commands
MSQ = @

####################
# Liste des cibles #
####################

# Fake targets
.PHONY: all clean $(TARGETS)

all: $(TARGETS)

$(TARGETS): run

run: $(DOXYGEN_CONFIG)
	@echo "Building documentation"
	$(MSQ)$(DOXYGEN_CMD) $(DOXYGEN_CONFIG)

ifneq ($(findstring pdf,$(TARGETS)),)

pdf: $(DOXYGEN_OUTPUTDIR)/documentation.pdf

$(DOXYGEN_OUTPUTDIR)/documentation.pdf: latex
	@echo "Building PDF documentation"
	$(MSQ)$(MAKE) -C "${DOXYGEN_LATEX_OUTPUTDIR}"
	$(MSQ)mv "${DOXYGEN_LATEX_OUTPUTDIR}/refman.pdf" "$@"
	@echo "File '$@' created"

endif

info:
	@echo "CURRENT_DIRECTORY: '$(CURRENT_DIRECTORY)'"
	@echo "DOXYGEN_CMD: '$(DOXYGEN_CMD)'"
	@echo "DOXYGEN_CONFIG: '$(DOXYGEN_CONFIG)'"
	@echo "DOXYGEN_OUTPUTDIR: '$(DOXYGEN_OUTPUTDIR)'"
	@echo "TARGETS: '$(TARGETS)'"
	@echo "DOXYGEN_HTML: '$(DOXYGEN_HTML)'"
	@echo "DOXYGEN_HTML_OUTPUTDIR: '$(DOXYGEN_HTML_OUTPUTDIR)'"
	@echo "DOXYGEN_LATEX: '$(DOXYGEN_LATEX)'"
	@echo "DOXYGEN_LATEX_OUTPUTDIR: '$(DOXYGEN_LATEX_OUTPUTDIR)'"

# Nettoyage
clean:
	@echo "Remove documentation directories that may exist ($(TARGETS_SUBDIRS))"
	$(MSQ)rm -rf $(TARGETS_SUBDIRS)
	@echo "Remove backup files"
	$(MSQ)rm -f *~
	@echo
