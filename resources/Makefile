GENERATED_FASTA_FILES = sequences_*.fa

# Hide compilation commands
MSQ = @

###########
# Targets #
###########

# Fake targets
.PHONY: all clean

# Handled Suffixes
.SUFFIXES: .fa .fasta

all: $(GENERATED_FASTA_FILES)

$(GENERATED_FASTA_FILES):
	@echo "Generating fasta sample files"
	@echo "This will take a while... May be the time for you to take a coffee break ^-^"
	$(MSQ)python seq.py

# Nettoyage
clean:
	@echo "Remove generated files ($(GENERATED_FASTA_FILES))"
	$(MSQ)rm -f $(GENERATED_FASTA_FILES)
	@echo "Remove backup files"
	$(MSQ)rm -f *~
	@echo
