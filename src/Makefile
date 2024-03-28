###########################
# Compiling Configuration #
###########################

# Compiler and linker
CXX = g++
LD = g++

# Compiling options
override CXXFLAGS += -Wall -Wextra -ansi -pedantic -I. -fopenmp -O3 -std=c++17 # For C++

# Uncomment to enable queue evolution observation.
#override CXXFLAGS += -DWATCH_QUEUE

# Uncomment to enable printing of (lots of) debug informations.
#override CXXFLAGS += -DDEBUG

# Uncomment to disable assertion checking.
override CXXFLAGS += -DNDEBUG

LDFLAGS = -pthread# Linker Options

# Silent production rules
MSQ = @

# (Hidden) Directory containing file dependencies
DEPDIR = .dep

#################
# Project Files #
#################

# Binaries to build
PROGS = BijectHash BijectHash_with_checks BijectHash_full_debug #benchmark

BijectHash_SOURCES = \
  circular_queue.hpp							\
  canonical_transformer.hpp canonical_transformer.cpp			\
  composition_transformer.hpp composition_transformer.cpp		\
  file_reader.hpp file_reader.cpp					\
  gab_transformer.hpp gab_transformer.cpp				\
  lyndon_transformer.hpp lyndon_transformer.cpp				\
  identity_transformer.hpp identity_transformer.cpp			\
  inthash.h inthash.c							\
  inthash_transformer.hpp inthash_transformer.cpp			\
  minimizer_transformer.hpp minimizer_transformer.cpp			\
  bwt_transformer.hpp bwt_transformer.cpp				\
  kmer_collector.hpp kmer_collector.cpp					\
  kmer_index.hpp kmer_index.cpp						\
  kmer_processor.hpp kmer_processor.cpp					\
  locker.hpp locker.cpp							\
  main.cpp								\
  permutations.hpp permutations.cpp					\
  permutation_transformer.hpp permutation_transformer.cpp		\
  permutation_bit_transformer.hpp permutation_bit_transformer.cpp	\
  program_options.hpp program_options.cpp				\
  settings.hpp settings.cpp						\
  transformer.hpp transformer.cpp

BijectHash_with_checks_SOURCES = \
  $(BijectHash_SOURCES) \
  queue_watcher.hpp queue_watcher.tpp
BijectHash_with_checks_CXXFLAGS = -DWATCH_QUEUE -UNDEBUG

BijectHash_full_debug_SOURCES = $(BijectHash_SOURCES)
BijectHash_full_debug_CXXFLAGS = -DDEBUG -UNDEBUG

benchmark_SOURCES = \
  benchmark.cpp				\
  file_reader.hpp file_reader.cpp	\
  inthash.h inthash.c			\
  permutations.h permutations.cpp

############################
# Computing default target #
############################


ifeq ($(wildcard $(DEPDIR)), )
# If directory $(DEPDIR) doesn't exist, default target is to build it then to call make again.
all: $(DEPDIR)
	$(MAKE)
else
# If directory $(DEPDIR) already exists, default target is to build binaries from variable $(PROGS)
all: $(PROGS)
# Since directory $(DEPDIR) exists, it is needed to include dependency files it contains.
include $(wildcard $(DEPDIR)/*)
endif

#########################
# Dependencies handling #
#########################

reduce = $(if $(strip $2),$(call reduce,$1,$(wordlist 2,$(words $2),$2),$(call $1,$(firstword $2),$3)),$3)
check_uniq = $(if $(filter $1,$2),$2,$2 $1)
uniq = $(call reduce,check_uniq,$1)

define add_program =
  $(eval curc := $(filter %.c,$($(1)_SOURCES)))
  $(eval curcpp := $(filter %.cpp,$($(1)_SOURCES)))
  $(1)_OBJECTS = $(patsubst %.c,$(1)_%.o,$(curc)) $(patsubst %.cpp,$(1)_%.o,$(curcpp))
  $(1): $$($(1)_OBJECTS)
	@echo "Building file '$$@' from '$$^'"
	$(MSQ)$(LD) $(LDFLAGS) $($(1)_LDFLAGS) $$^ $($(1)_LIBS:%=-l%) -o $(1)
	@echo
  $(1)_%.o: %.c
	@echo "Compilation of file '$$<'"
	@echo "- building dependency file '$(DEPDIR)/$$(@:.o=.depend)'"
	@echo "- building file '$$@'"
	$(MSQ)$(CXX) $(CXXFLAGS) $($(1)_CXXFLAGS) -MT $$@ -MMD -MP -MF $(DEPDIR)/$$(@:.o=.depend) -c -o $$@ $$<
	@echo
  $(1)_%.o: %.cpp
	@echo "Compilation of file '$$<'"
	@echo "- building dependency file '$(DEPDIR)/$$(@:.o=.depend)'"
	@echo "- building file '$$@'"
	$(MSQ)$(CXX) $(CXXFLAGS) $($(1)_CXXFLAGS) -MT $$@ -MMD -MP -MF $(DEPDIR)/$$(@:.o=.depend) -c -o $$@ $$<
	@echo
  OBJECTS += $$($(1)_OBJECTS)
endef

$(foreach prog, $(PROGS),$(eval $(call add_program,$(prog))))

FILES=$(call uniq,$(foreach prog,$(PROGS),$($(prog)_SOURCES))) Makefile
SOURCES=$(filter %.cpp,$(FILES))
HEADERS=$(filter %.h,$(FILES))


###################
# List of targets #
###################

# Fake targets
.PHONY: all clean

# Handled file Suffixes
.SUFFIXES: .h .c .hpp .cpp .o .depend

# Directory containing dependency files
$(DEPDIR):
	@echo "Building directory to store dependency files"
	$(MSQ)mkdir -p "$@"

# Cleaning
clean:
	@echo "Remove directory storing dependency files ($(DEPDIR))"
	$(MSQ)rm -rf $(DEPDIR)
	@echo "Remove binaries ($(PROGS))"
	$(MSQ)rm -f $(PROGS)
	@echo "Remove binary components ($(OBJECTS))"
	$(MSQ)rm -f $(OBJECTS)
	@echo "Remove backups"
	$(MSQ)rm -f *~
	@echo

