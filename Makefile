##########################################################################
## ^FILE: Makefile - make file for the CmdLine product
##
## ^DESCRIPTION:
##    This is the makefile that is used to build and install the CmdLine
##    product.
##
## ^HISTORY:
##    04/28/92	Brad Appleton	<bradapp@enteract.com>	Created
###^^#####################################################################

include Config.mk

SUBDIRS = src doc
SRCDIRS = src
DOCDIRS = doc

###
# target dependencies
###
help:
	@$(ECHO) "Usage: make <target>"
	@$(ECHO) ""
	@$(ECHO) "where <target> is one of the following:"
	@$(ECHO) ""
	@$(ECHO) "    all        -- build (but do not install) the product"
	@$(ECHO) "    install    -- install the product"
	@$(ECHO) "    library    -- build the cmdline library"
	@$(ECHO) "    program    -- build the cmdparse program"
	@$(ECHO) "    docs       -- build the documentation"
	@$(ECHO) "    clean      -- remove all intermediate files"
	@$(ECHO) "    clobber    -- remove all generated files"
	@$(ECHO) ""

all install clean clobber:
	for i in $(SUBDIRS) ; do \
		( $(CHDIR) $$i ; $(BUILD) $@ ) ; \
	done

library program:
	for i in $(SRCDIRS) ; do \
		( $(CHDIR) $$i ; $(BUILD) $@ ) ; \
	done

docs:
	for i in $(DOCDIRS) ; do \
		( $(CHDIR) $$i ; $(BUILD) all ) ; \
	done

