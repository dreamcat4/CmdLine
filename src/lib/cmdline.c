//------------------------------------------------------------------------
// ^FILE: cmdline.c - implement CmdLine member functions.
//
// ^DESCRIPTION:
//    Many of the more basic member functions of a CmdLine are implemented
//    in this file. They are as follows:
//
//       Contructors
//       Destructors
//       CmdLine::name()
//       CmdLine::error()
//       CmdLine::append
//
// ^HISTORY:
//    03/21/92	Brad Appleton	<bradapp@enteract.com>	Created
//
//    03/01/93	Brad Appleton	<bradapp@enteract.com>
//    - Added cmd_nargs_parsed field to CmdLine
//    - Added cmd_description field to CmdLine
//    - Added exit_handler() and quit() member-functions to CmdLine
//-^^---------------------------------------------------------------------

#include <stdlib.h>
#include <iostream.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "cmdline.h"
#include "cmdargs.h"
#include "arglist.h"
#include "states.h"

#define  va_CmdArgP(ap)  va_arg(ap, CmdArg *)


//------------------------------------------------------------------- init_args

//-------------------
// ^FUNCTION: init_args - initialize the arg_list member of a CmdLine
//
// ^SYNOPSIS:
//    init_args(list)
//
// ^PARAMETERS:
//    CmdArgListList & * list;
//    -- a reference to the list that is to be initialized.
//
// ^DESCRIPTION:
//    Allocate space for the list of command-arguments and insert
//    The default arguments onto the list.
//
// ^REQUIREMENTS:
//    list should be NULL upon entry
//
// ^SIDE-EFFECTS:
//    creates the arg-list and makes "list" point to it.
//
// ^RETURN-VALUE:
//    None.
//
// ^ALGORITHM:
//    - Create a new arg-list (sure it is NOT self-cleaning, the user is
//                             responsible for deleting inserted items)
//    - Insert the default arguments onto the list
//    - Make list point to the newly created list
//-^^----------------
static void
init_args(CmdArgListList * & list)
{
   static  CmdArgUsage  default_help1('?', "?",    "; print usage and exit.");
   static  CmdArgUsage  default_help2('H', "Help", "; print usage and exit.");

   list = new CmdArgListList ;
   list->self_cleaning(1);

   CmdArgList * arg_list = new CmdArgList;
   arg_list->self_cleaning(0);

   CmdArgList * default_list = new CmdArgList;
   default_list->self_cleaning(0);
   default_list->add(&default_help1);
   default_list->add(&default_help2);

   list->add(arg_list);
   list->add(default_list);
}

//---------------------------------------------------------------- filebasename

//-------
// ^FUNCTION: filebasename
//
// ^SYNOPSIS:
//    static const char * filebasename(filename);
//
// ^PARAMETERS:
//    const char * filename;
//    -- the filename to get the "base" of.
//
// ^DESCRIPTION:
//    Extract and return the basename of "filename".
//
// ^REQUIREMENTS:
//    "filename" should be non-NULL and non-empty.
//
// ^SIDE-EFFECTS:
//    On VAX/VMS, MS-DOS, and OS/2 systems space is allocated (using malloc)
//    for the returned value.
//
// ^RETURN-VALUE:
//    The basename portion of the filename.
//
// ^ALGORITHM:
//    For Unix systems:
//       return everything following the rightmost '/'
//
//    For VAX/VMS systems:
//       make a copy of filename.
//       strip off any device name, any directory name.
//       strip off any "." extension.
//       strip off any version number.
//
//    For MS-DOS systems:
//       make a copy of filename.
//       strip off any drive and/or directory path.
//       strip off any "." extension.
//-^^----
static const char *
filebasename(const char * filename)
{

   if (filename == NULL)  return  filename ;

#if (defined(vms) || defined(msdos) || defined(os2))
   const char * start, * p1, * p2 ;
   char * str, * ext;

# ifdef vms
   char * ver;
   // remove leading directory and/or device name
   p1 = ::strrchr(filename, ':');
   p2 = ::strrchr(filename, ']');
# else
   // remove leading directory and/or drive name
   p1 = ::strrchr(filename, '/');
   p2 = ::strrchr(filename, '\\');
# endif
   if ((p1 == NULL) && (p2 == NULL)) {
      start = filename ;
   } else if (p1 && (p2 == NULL)) {
      start = p1 + 1;
   } else if (p2 && (p1 == NULL)) {
      start = p2 + 1;
   } else {
      start = ((p1 > p2) ? p1 : p2) + 1;
   }

   str = new char[strlen(start) + 1];
   (void) ::strcpy(str, start);

   // remove the extension
   ext = ::strrchr(str, '.');
   if (ext)  *ext = '\0' ;

# ifdef vms
   // remove the version
   ver = ::strrchr(str, ';');
   if (ver)  *ver = '\0' ;
# endif

   return  str ;

#else

   char * p = ::strrchr(filename, '/') ;
   return  (p == NULL) ? filename : (p + 1) ;

#endif /* if (vms || msdos || os2) */

}

//--------------------------------------------------------------- class CmdLine

  // Contructor with a command-name
CmdLine::CmdLine(const char * cmdname)
   : cmd_parse_state(cmd_START_STATE),
     cmd_state(cmd_START_STATE),
     cmd_flags(DEFAULT_CMDFLAGS),
     cmd_status(CmdLine::NO_ERROR),
     cmd_nargs_parsed(0),
     cmd_name(NULL),
     cmd_description(NULL),
     cmd_matched_arg(NULL),
     cmd_args(NULL),
     cmd_err(NULL),
     cmd_quit_handler(NULL)
{
   name(cmdname);
   ::init_args(cmd_args);
}

   // Constructor with a name and CmdArgs
CmdLine::CmdLine(const char * cmdname, CmdArg * cmdarg1 ...)
   : cmd_parse_state(cmd_START_STATE),
     cmd_state(cmd_START_STATE),
     cmd_flags(DEFAULT_CMDFLAGS),
     cmd_status(CmdLine::NO_ERROR),
     cmd_nargs_parsed(0),
     cmd_name(NULL),
     cmd_description(NULL),
     cmd_matched_arg(NULL),
     cmd_args(NULL),
     cmd_err(NULL),
     cmd_quit_handler(NULL)
{
   name(cmdname);
   ::init_args(cmd_args);

   CmdArgListListIter  iter(cmd_args);
   CmdArgList * arg_list = iter();

   va_list  ap;
   va_start(ap, cmdarg1);
   for (CmdArg * cmdarg = cmdarg1 ; cmdarg ; cmdarg = va_CmdArgP(ap)) {
      arg_list->add(cmdarg);
   }
   va_end(ap);
}


   // Constructor with CmdArgs
CmdLine::CmdLine(CmdArg * cmdarg1 ...)
   : cmd_parse_state(cmd_START_STATE),
     cmd_state(cmd_START_STATE),
     cmd_flags(DEFAULT_CMDFLAGS),
     cmd_status(CmdLine::NO_ERROR),
     cmd_nargs_parsed(0),
     cmd_name(NULL),
     cmd_description(NULL),
     cmd_matched_arg(NULL),
     cmd_args(NULL),
     cmd_err(NULL),
     cmd_quit_handler(NULL)
{
   if (cmdarg1 == NULL)  return;
   ::init_args(cmd_args);

   CmdArgListListIter  iter(cmd_args);
   CmdArgList * arg_list = iter();

   va_list  ap;
   va_start(ap, cmdarg1);
   for (CmdArg * cmdarg = cmdarg1 ; cmdarg ; cmdarg = va_CmdArgP(ap)) {
      arg_list->add(cmdarg);
   }
   va_end(ap);
}


   // Destructor
CmdLine::~CmdLine(void)
{
   delete  cmd_args;

#if (defined(vms) || defined(msdos) || defined(os2))
   delete [] cmd_name;
#endif
}


   // Set the name of the command
void
CmdLine::name(const char * progname)
{
   static  const char unknown_progname[] = "<unknown-program>" ;

#if (defined(vms) || defined(msdos) || defined(os2))
   delete [] cmd_name;
#endif
   cmd_name = ::filebasename((progname) ? progname : unknown_progname);
}


   // Print an error message prefix and return a reference to the
   // error output stream for this command
ostream &
CmdLine::error(unsigned  quiet) const
{
   ostream * os = (cmd_err) ? cmd_err : &cerr ;
   if (cmd_name && *cmd_name && !quiet)  *os << cmd_name << ": " ;
   return  *os;
}


  // Add an argument to the current list of CmdArgs
CmdLine &
CmdLine::append(CmdArg * cmdarg)
{
   CmdArgListListIter  iter(cmd_args);
   CmdArgList * arg_list = iter();
   arg_list->add(cmdarg);

   return  *this ;
}

   // terminate parsing altogether
void
CmdLine::quit(int status) {
   if (cmd_quit_handler != NULL) {
      (*cmd_quit_handler)(status);
   } else {
      ::exit(status);
   }
}

//---------------------------------------------------------- CmdLineCmdArgIter

   // Constructors and Destructors

CmdLineCmdArgIter::CmdLineCmdArgIter(CmdLine & cmd)
   : iter(NULL)
{
   if (cmd.cmd_args) {
      CmdArgListListIter  listlist_iter(cmd.cmd_args);
      CmdArgList  * list = listlist_iter();
      if (list)  iter = new CmdArgListIter(list);
   }
}

CmdLineCmdArgIter::CmdLineCmdArgIter(CmdLine * cmd)
   : iter(NULL)
{
   if (cmd->cmd_args) {
      CmdArgListListIter  listlist_iter(cmd->cmd_args);
      CmdArgList  * list = listlist_iter();
      if (list)  iter = new CmdArgListIter(list);
   }
}

CmdLineCmdArgIter::~CmdLineCmdArgIter(void)
{
   delete  iter;
}

   // Return the current argument and advance to the next one.
   // Returns NULL if we are already at the end of the list.
   //
CmdArg *
CmdLineCmdArgIter::operator()(void)
{
   return  (iter) ? iter->operator()() : NULL ;
}

