##########################################################################
## ^FILE: Makefile - make file for the cmdparse program
##
## ^DESCRIPTION:
##    This is the makefile that is used to build and install the "cmdparse"
##    program. In order to get cmdparse to build on your system you may
##    need to change some of the "variables" mentioned below. You may also
##    need to redefine some of the commands that are used.
##
## ^TARGETS:
##    all     : make the cmdparse program
##    program : same as "all"
##    install : build and install the cmdparse program
##    clean   : remove all intermediate files generated by the build
##    clobber : remove all files generated by the build
##
## ^VARIABLES:
##    OS         : specify the host operating system (default=unix)
##    FLAG       : debugging/optimization flags to the compiler (default=-g)
##    TESTDEFS   : #defines used for testing
##    USRDEFS    : #defines flags used for building (default=unix)
##    OPTIONS    : any other compiler flags to use
##
## ^HISTORY:
##    04/10/92	Brad Appleton	<bradapp@enteract.com>	Created
###^^#####################################################################

include ../../Config.mk

###
# operating-system dependent stuff
###
OS=unix
# OSDEFS=$(DEF)$(OS) $(DEF)$(.UNIVERSE)_universe

##
# Programs
##
PROGRAM=$(PROGNAME)$(EXECEXT)

###
# Program Libraries
###
PROGLIB=$(PROGLIBDIR)lib$(LIBNAME)$(LIBEXT)

###
# compilation options
###
INCLUDES=$(INC)$(PROGLIBDIR)
FLAG=$(OPT)
# FLAG=$(DBG)
TESTDEFS=
USRDEFS=
DEFINES=$(OSDEFS) $(USRDEFS) $(TESTDEFS)
OPTIONS=
CFLAGS=$(FLAG) $(INCLUDES) $(DEFINES) $(OPTIONS)

###
# files used
###
OBJS= argtypes$(OBJEXT) cmdparse$(OBJEXT) fsm$(OBJEXT) main$(OBJEXT) \
   quoted$(OBJEXT) shell_arg$(OBJEXT) shells$(OBJEXT) syntax$(OBJEXT)

PROGLIB=$(PROGLIBDIR)libcmdline$(LIBEXT)

###
# target dependencies
###
all: program

program: library $(PROGRAM)

library:
	( $(CHDIR) $(PROGLIBDIR) ; $(BUILD) $@ ; )

$(PROGRAM): $(OBJS)
	$(CC) $(EXE) $@ $(OBJS) $(PROGLIB)

###
# maintenance dependencies
###
install: $(PROGRAM) $(BINDIR) $(PROGRAM).pl
	-$(RM) $(BINDIR)$(PROGRAM) $(PERLLIB)$(PROGRAM).pl $(TCLLIB)$(PROGRAM).tcl
	$(CP) $(PROGRAM)  $(BINDIR)$(PROGRAM)
	$(STRIP) $(BINDIR)$(PROGRAM)
	-$(CP) $(PROGRAM).pl $(PERLLIB)$(PROGRAM).pl
	-$(CP) $(PROGRAM).tcl $(TCLLIB)$(PROGRAM).tcl

clean:
	-$(RM) *$(OBJEXT) core .exrc *~ \#*\#

clobber: clean
	-$(RM) *$(LIBEXT) $(PROGRAM) tags TAGS

###
# object dependencies
###
argtypes$(OBJEXT) : argtypes$(CEXT) argtypes.h shell_arg.h \
   $(PROGLIBDIR)cmdargs.h $(PROGLIBDIR)cmdline.h shells.h

cmdparse$(OBJEXT) : cmdparse$(CEXT) argtypes.h shell_arg.h \
   $(PROGLIBDIR)cmdargs.h $(PROGLIBDIR)cmdline.h shells.h cmdparse.h \
   syntax.h fsm.h quoted.h

fsm$(OBJEXT) : fsm$(CEXT) fsm.h

main$(OBJEXT) : main$(CEXT) $(PROGLIBDIR)cmdline.h cmdparse.h \
   $(PROGLIBDIR)cmdargs.h

quoted$(OBJEXT) : quoted$(CEXT) quoted.h

shell_arg$(OBJEXT) : shell_arg$(CEXT) shell_arg.h $(PROGLIBDIR)cmdargs.h \
   $(PROGLIBDIR)cmdline.h shells.h

shells$(OBJEXT) : shells$(CEXT) $(PROGLIBDIR)fifolist.h shells.h \
   argtypes.h shell_arg.h $(PROGLIBDIR)cmdargs.h $(PROGLIBDIR)cmdline.h

syntax$(OBJEXT) : syntax$(CEXT) $(PROGLIBDIR)cmdline.h syntax.h fsm.h \
   quoted.h
