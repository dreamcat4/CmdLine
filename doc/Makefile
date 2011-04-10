##########################################################################
## ^FILE: Makefile - make file for the documentation for CmdLine product
##
## ^DESCRIPTION:
##    This is the makefile that is used to build and install the
##    documentation CmdLine product.
##
## ^HISTORY:
##    04/28/92  Brad Appleton   <bradapp@enteract.com>   Created
###^^#####################################################################

include ../Config.mk

###
# give a usage message if no target is given
###
usage:
	@echo "Usage: make <target>"
	@echo " "
	@echo "where <target> is one of the following: "
	@echo " "
	@echo "   install  -- to install the documentation"
	@echo "   installman  -- to install the unpacked documentation"
	@echo "   installcatman  -- to install the packed documentation"
	@echo "   print  -- to print the documentation"
	@echo "   view   -- to view the documentation"
	@echo "   text   -- to build text copies of the documentation"
	@echo "   spell  -- to spell check the documentation"
	@echo "   clean  -- to remove all intermediate files"
	@echo "   clobber  -- to remove all created files"

###
# files used
###
INCLUDES=bugs.man caveats.man environ.man example.man files.man \
         macros.man parsing.man
DOCS=cmdparse.man1 cmdline.man3 cmdargs.man3
MAN3FILES=cmdline.3 cmdargs.3
MAN1FILES=cmdparse.1
CATMAN3FILES=cmdline.3.z cmdargs.3.z
CATMAN1FILES=cmdparse.1.z

###
# target dependencies
###
.SUFFIXES: .man1 .man3 .1 .3

.man1.1:
	$(SOELIM) $< >$*.1
	$(MANTOCATMAN) $*.1

.man3.3:
	$(SOELIM) $< >$*.3
	$(MANTOCATMAN) $*.3

###
# installation dependencies
###
all:

install: installman installcatman

installman: installman1 installman3

installcatman: installcatman1 installcatman3

installman1: $(MAN1FILES)
	( $(CHDIR) $(MANDIR)$(MAN1DIR) ; $(RM) $(MAN1FILES) )
	$(CP) $(MAN1FILES) $(MANDIR)$(MAN1DIR)

installcatman1: $(CATMAN1FILES)
	( $(CHDIR) $(CATMANDIR)$(MAN1DIR) ; $(RM) $(CATMAN1FILES) )
	$(CP) $(CATMAN1FILES) $(CATMANDIR)$(MAN1DIR)

installman3: $(MAN3FILES)
	( $(CHDIR) $(MANDIR)$(MAN3DIR) ; $(RM) $(MAN3FILES) )
	$(CP) $(MAN3FILES) $(MANDIR)$(MAN3DIR)

installcatman3: $(CATMAN3FILES)
	( $(CHDIR) $(CATMANDIR)$(MAN3DIR) ; $(RM) $(CATMAN3FILES) )
	$(CP) $(CATMAN3FILES) $(CATMANDIR)$(MAN3DIR)

###
# maintenance dependencies
###
clean:
	$(RM) $(MAN1FILES) $(MAN3FILES) $(CATMAN1FILES) $(CATMAN3FILES) CmdLine.3.z

clobber: clean
	$(RM) *.txt

spell: $(DOCS) $(INCLUDES)
	$(SPELL) $(DOCS) $(INCLUDES)

print: $(MANFILES)
	$(TROFF) $(TRFLAGS) $(DOCS)

text: ascii
txt: ascii
ascii:
	$(NROFF) $(NRFLAGS) cmdparse.man1 | $(COL) > cmdparse1.txt
	$(NROFF) $(NRFLAGS) cmdline.man3  | $(COL) > cmdline3.txt
	$(NROFF) $(NRFLAGS) cmdargs.man3  | $(COL) > cmdargs3.txt

view: $(MANFILES)
	$(NROFF) $(NRFLAGS) $(DOCS)

##
# include dependencies
##
cmdparse.1: cmdparse.man1 $(INCLUDES)

cmdline.3: cmdline.man3 $(INCLUDES)

cmdargs.3: cmdargs.man3 $(INCLUDES)

