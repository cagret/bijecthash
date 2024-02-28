# Subdirs on which to run make recursively
SUBDIRS = src doc resources

# Hide compilation commands
MSQ = @

###########
# Targets #
###########

# Fake targets
.PHONY: all clean

all:
	$(MSQ)for dir in $(SUBDIRS); do \
	  $(MAKE) -C "$${dir}" MSQ=$(MSQ) || exit $?; \
	done

clean:
	@echo "Cleaning files for all subdirs ($(SUBDIRS))"
	$(MSQ)for dir in $(SUBDIRS); do \
	  $(MAKE) -C "$${dir}" clean MSQ=$(MSQ) || exit $$?; \
	done
	@echo "Remove backup files"
	$(MSQ)rm -f *~
	@echo
