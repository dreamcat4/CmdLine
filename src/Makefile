##########################################################################
## ^FILE: Makefile - make file for the CmdLine product
##
## ^DESCRIPTION:
##    This is the makefile that is used to build and install the CmdLine
##    library and the cmdparse program.
##
## ^HISTORY:
##    04/28/92	Brad Appleton	<bradapp@enteract.com>	Created
###^^#####################################################################

include ../Config.mk

SUBDIRS = lib cmd 
LIBDIRS = lib
PROGDIRS = cmd

###
# target dependencies
###
help:
	@$(ECHO) "Usage: make <target>"
	@$(ECHO) ""
	@$(ECHO) "where <target> is one of the following:"
	@$(ECHO) ""
	@$(ECHO) "    all        -- build (but do not install) the product"
	@$(ECHO) "    install    -- install CmdLine(3C++) and cmdparse(1)
	@$(ECHO) "    library    -- build the cmdline library"
	@$(ECHO) "    program    -- build the cmdparse program"
	@$(ECHO) "    clean      -- remove all intermediate files"
	@$(ECHO) "    clobber    -- remove all generated files"
	@$(ECHO) ""

all install clean clobber:
	for i in $(SUBDIRS) ; do \
		( $(CHDIR) $$i ; $(BUILD) $@ ) ; \
	done

program:
	for i in $(PROGDIRS) ; do \
		( $(CHDIR) $$i ; $(BUILD) $@ ) ; \
	done

library:
	for i in $(LIBDIRS) ; do \
		( $(CHDIR) $$i ; $(BUILD) $@ ) ; \
	done

