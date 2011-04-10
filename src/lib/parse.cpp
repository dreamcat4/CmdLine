//------------------------------------------------------------------------
// ^FILE: parse.c - parsing portion of the CmdLine library
//
// ^DESCRIPTION:
//     The following functions defined in <cmdline.h> are implemented:
//
//        CmdLine::prologue() -- initialize stuff for parsing
//        CmdLine::epilogue() -- finish up stuff for parsing
//        CmdLine::parse()     -- parse arguments from an iterator
//
// ^HISTORY:
//    12/05/91	Brad Appleton	<bradapp@enteract.com>	Created
//
//    03/01/93	Brad Appleton	<bradapp@enteract.com>
//    - Added cmd_nargs_parsed field to CmdLine
//    - Added exit_handler() and quit() member-functions to CmdLine
//-^^---------------------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "exits.h"
#include "states.h"
#include "arglist.h"
#include "cmdline.h"


//-------
// ^FUNCTION: CmdLine::prologue - initialize before parsing
//
// ^SYNOPSIS:
//    unsigned CmdLine::prologue(void)
//
// ^PARAMETERS:
//    None.
//
// ^DESCRIPTION:
//    Before we can begin parsing argument from the command-line, we need
//    to set (or reset) certain attributes of the CmdLine object. Among
//    other things, we need to reset its state and status, and we need to
//    reset the state of each of its arguments.
//
// ^REQUIREMENTS:
//    None.
//
// ^SIDE-EFFECTS:
//    Modifies all parts of the CmdLine object and its arguments.
//
// ^RETURN-VALUE:
//    A combination of bitmasks of type CmdLine::CmdStatus corresponding to
//    what (if anything) went wrong.
//
// ^ALGORITHM:
//    Follow along - its not too complicated.
//-^^----
unsigned
CmdLine::prologue(void)
{
   // reset parse-specific attributes
   cmd_parse_state = cmd_START_STATE ;
   cmd_state = 0 ;
   cmd_status = NO_ERROR ;
   cmd_nargs_parsed = 0 ;

   // reset parse-specific attributes for each argument
   CmdArgListListIter  list_iter(cmd_args);
   for (CmdArgList * alist = list_iter() ; alist ; alist = list_iter()) {
      CmdArgListIter  iter(alist);
      for (CmdArg * cmdarg = iter() ; cmdarg ; cmdarg = iter()) {
         cmdarg->clear();
      }
   }

   return  cmd_status ;
}

//-------
// ^FUNCTION: CmdLine::epilogue - clean up after parsing
//
// ^SYNOPSIS:
//    unsigned CmdLine::epilogue(void)
//
// ^PARAMETERS:
//    None.
//
// ^DESCRIPTION:
//
// ^REQUIREMENTS:
//    None.
//
// ^SIDE-EFFECTS:
//    Modifies the command-line obejct.
//
//    Prints messages on cerr (if QUIET is not set) if there are
//    missing required arguments or values (and prompts for them if
//    PROMPT_USER is set of $PROMPT_USER is exists and is non-empty).
//
//    Prints a usage message if there were syntax error.
//
//    Terminates program execution by calling quit(e_SYNTAX) if
//    (NO_ABORT is NOT set and the command-status is NOT NO_ERROR).
//
// ^RETURN-VALUE:
//    A combination of bitmasks of type CmdLine::CmdStatus corresponding to
//    what (if anything) went wrong.
//
// ^ALGORITHM:
//    - See if we left an argument dangling without a required value,
//    - Check for missing required arguments
//    - Print usage if required
//    - Exit if required
//-^^----
unsigned
CmdLine::epilogue(void)
{
   if (cmd_err == NULL)  cmd_err = &cerr;

   // see if we left an argument dangling without a value
   ck_need_val() ;

   // check for any missing required arguments
   cmd_status |= missing_args();

   // print usage if necessary
   if (cmd_status & ARG_MISSING) {
      usage();
      quit(e_SYNTAX);
   } else if (cmd_status && (! (cmd_flags & NO_ABORT))) {
      usage();
      quit(e_SYNTAX);
   }

   return  cmd_status ;
}

//-------------------
// ^FUNCTION: parse - parse arguments from an iterator
//
// ^SYNOPSIS:
//    parse(arg_iter, auto_processing)
//
// ^PARAMETERS:
//    CmdLineArgIter & arg_iter;
//    -- collection of string arguments from the command-line
//
//    int  auto_processing;
//    -- if this is NON-zero, then automatically call prologue() and
//       epilogue() to do pre- and post- processing.
//
// ^DESCRIPTION:
//    Parse all the argument in a given argument iterator.
//    If auto_processing is NON-zero then the programmer is
//    directly responsible for calling prologue() and epilogue().
//
// ^REQUIREMENTS:
//    None.
//
// ^SIDE-EFFECTS:
//    - Uses up all remaining arguments in arg_iter
//    - Modifies the CmdLine
//
// ^RETURN-VALUE:
//    The resultant status of the CmdLine:
//
// ^ALGORITHM:
//    Trivial - just iterate through calling parse_arg.
//-^^----------------
unsigned
CmdLine::parse(CmdLineArgIter & arg_iter, int  auto_processing)
{
   // NOTE: If arg_iter.is_temporary() is TRUE then we MUST remember
   //       to set the CmdLine::TEMP flags before parsing (and put it
   //       back the way it was when we are finished.
   //
   if (auto_processing)  (void) prologue();
   unsigned  save_flags = cmd_flags;
   if (arg_iter.is_temporary())  cmd_flags |= TEMP;
   for (const char * arg = arg_iter() ; arg ; arg = arg_iter()) {
      (void) parse_arg(arg);
   }
   if (arg_iter.is_temporary())  cmd_flags = save_flags;
   if (auto_processing)  (void) epilogue();
   return  cmd_status ;
}

