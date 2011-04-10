//------------------------------------------------------------------------
// ^FILE: dump.c - debugging/dumping functions of the CmdLine library
//
// ^DESCRIPTION:
//    If DEBUG_CMDLINE is #defined when this file is compiled, then
//    the functions that print out debugging information for a CmdLine
//    and a CmdArg are implemented.
//
// ^HISTORY:
//    04/01/92	Brad Appleton	<bradapp@enteract.com>	Created
//
//    03/01/93	Brad Appleton	<bradapp@enteract.com>
//    - Added arg_sequence field to CmdArg
//    - Added cmd_nargs_parsed field to CmdLine
//    - Added cmd_description field to CmdLine
//-^^---------------------------------------------------------------------

#include  "cmdline.h"

#ifdef DEBUG_CMDLINE
# include  <iostream.h>
# include  <string.h>

# include  "arglist.h"
# include  "states.h"
#endif


#ifdef DEBUG_CMDLINE

   // Indent a line corresponding to a given indent level.
   // The number of spaces to indent is 3x the indent level
   //
static ostream &
indent(ostream & os, unsigned level)
{
   os.width(level * 3);
   return  (os << "");
}


   // Dump the arg_syntax field of a CmdArg in a mnemonic format
   //
static ostream &
dump_arg_syntax(ostream & os, unsigned  syntax)
{
   if (syntax & CmdArg::isREQ) {
      os << "isREQ" ;
   } else {
      os << "isOPT" ;
   }

   if (syntax & CmdArg::isVALREQ) {
      os << "+isVALREQ" ;
   }
   if (syntax & CmdArg::isVALOPT) {
      os << "+isVALOPT" ;
   }
   if (syntax & CmdArg::isVALSEP) {
      os << "+isVALSEP" ;
   }
   if (syntax & CmdArg::isVALSTICKY) {
      os << "+isVALSTICKY" ;
   }
   if (syntax & CmdArg::isLIST) {
      os << "+isLIST" ;
   }
   if (syntax & CmdArg::isPOS) {
      os << "+isPOS" ;
   }
   if (syntax & CmdArg::isHIDDEN) {
      os << "+isHID" ;
   }

   return  os;
}


   // Dump the arg_flags field of a CmdArg in a mnemonic format
static ostream &
dump_arg_flags(ostream & os, unsigned  flags)
{
   if (flags & CmdArg::GIVEN) {
      os << "GIVEN" ;
   } else {
      os << "NOTGIVEN";
   }
   if (flags & CmdArg::VALGIVEN) {
      os << "+VALGIVEN";
   }
   if (flags & CmdArg::OPTION) {
      os << "+OPTION";
   }
   if (flags & CmdArg::KEYWORD) {
      os << "+KEYWORD";
   }
   if (flags & CmdArg::POSITIONAL) {
      os << "+POSITIONAL";
   }
   if (flags & CmdArg::VALSEP) {
      os << "+VALSEP";
   } else if (flags & CmdArg::VALGIVEN) {
      os << "+VALSAME";
   }

   return  os;
}


   // Dump the cmd_flags field of a CmdLine in a mnemonic format
static ostream &
dump_cmd_flags(ostream & os, unsigned flags)
{
   if (flags & CmdLine::NO_ABORT) {
      os << "NO_ABORT" ;
   } else {
      os << "ABORT" ;
   }
   if (flags & CmdLine::ANY_CASE_OPTS) {
      os << "+ANY_CASE_OPTS";
   }
   if (flags & CmdLine::PROMPT_USER) {
      os << "+PROMPT_USER";
   }
   if (flags & CmdLine::OPTS_FIRST) {
      os << "+OPTS_FIRST";
   }
   if (flags & CmdLine::OPTS_ONLY) {
      os << "+OPTS_ONLY";
   }
   if (flags & CmdLine::KWDS_ONLY) {
      os << "+KWDS_ONLY";
   }
   if (flags & CmdLine::TEMP) {
      os << "+TEMP";
   }
   if (flags & CmdLine::QUIET) {
      os << "+QUIET";
   }
   if (flags & CmdLine::NO_GUESSING) {
      os << "+NO_GUESSING";
   }

   return  os;
}


   // Dump the status of a CmdLine in a mnemonic format
static ostream &
dump_cmd_status(ostream & os, unsigned  status)
{
   if (! status) {
      os << "NO_ERROR";
      return  os;
   } else {
      os << "ERROR";
   }
   if (status & CmdLine::ARG_MISSING) {
      os << "+ARG_MISSING";
   }
   if (status & CmdLine::VAL_MISSING) {
      os << "+VAL_MISSING";
   }
   if (status & CmdLine::VAL_NOTSTICKY) {
      os << "+VAL_NOTSTICKY";
   }
   if (status & CmdLine::VAL_NOTSEP) {
      os << "+VAL_NOTSEP";
   }
   if (status & CmdLine::KWD_AMBIGUOUS) {
      os << "+KWD_AMBIG";
   }
   if (status & CmdLine::BAD_OPTION) {
      os << "+BAD_OPT";
   }
   if (status & CmdLine::BAD_KEYWORD) {
      os << "+BAD_KWD";
   }
   if (status & CmdLine::BAD_VALUE) {
      os << "+BAD_VAL";
   }
   if (status & CmdLine::TOO_MANY_ARGS) {
      os << "+TOO_MANY";
   }

   return  os;
}


   // Dump the state of a CmdLine in a mnemonic format
static ostream &
dump_cmd_state(ostream & os, unsigned  state)
{
   if (! state) {
      os << "NO_OPTIONS";
   } else {
      os << "ARGS";
   }
   if (state & cmd_END_OF_OPTIONS) {
      os << "+ENDOPTS";
   }
   if (state & cmd_OPTIONS_USED) {
      os << "+OPTS_USED";
   }
   if (state & cmd_KEYWORDS_USED) {
      os << "+KWDS_USED";
   }
   if (state & cmd_GUESSING) {
      os << "+GUESSING";
   }

   return  os;
}


   // Dump the parse_state of a CmdLine in a mnemonic format
static ostream &
dump_cmd_parse_state(ostream & os, unsigned parse_state)
{
   switch (parse_state) {
   case cmd_START_STATE :
      os << "START_STATE";
      break;

   case cmd_TOK_REQUIRED :
      os << "TOK_REQUIRED";
      break;

   case cmd_WANT_VAL :
      os << "WANT_VAL";
      break;

   case cmd_NEED_VAL :
      os << "NEED_VAL";
      break;


#ifdef vms_style
   case cmd_WANT_VALSEP :
      os << "WANT_VALSEP";
      break;

   case cmd_NEED_VALSEP :
      os << "NEED_VALSEP";
      break;

   case cmd_WANT_LISTSEP :
      os << "WANT_LISTSEP";
      break;

   case cmd_NEED_LISTSEP :
      os << "NEED_LISTSEP";
      break;

#endif

   default :
      os << parse_state;
   }

   return  os;
}


   // Dump the arguments (including the default arguments) in an arg_list
static ostream &
dump_cmd_args(ostream & os, CmdArgListList * arg_list, unsigned level)
{
   ::indent(os, level) << "CmdLine::cmd_args {\n" ;

   CmdArgListListIter  list_iter(arg_list);
   for (CmdArgList * alist = list_iter() ; alist ; alist = list_iter()) {
      CmdArgListIter iter(alist);
      for (CmdArg * cmdarg = iter() ; cmdarg ; cmdarg = iter()) {
         cmdarg->dump(os, level + 1);
      }
   }

   ::indent(os, level) << "}" << endl;
   return  os;
}

#endif  /* DEBUG_CMDLINE */


   // Dump a CmdArg
void
CmdArg::dump(ostream & os, unsigned level) const
{
#ifdef DEBUG_CMDLINE
   ::indent(os, level) << "CmdArg {\n" ;

   ::indent(os, level + 1) << "option='" << char(arg_char_name) << "', "
                         << "keyword=\"" << arg_keyword_name << "\", "
                         << "value=\"" << arg_value_name << "\"\n" ;

   ::indent(os, level + 1) << "syntax=" ;
   dump_arg_syntax(os, arg_syntax) << "\n";

   ::indent(os, level + 1) << "flags=" ;
   dump_arg_flags(os, arg_flags) << "\n";

   ::indent(os, level + 1) << "sequence=" << arg_sequence << "\n";

   ::indent(os, level) << "}" << endl;
#endif
}


   // Dump a CmdLine
void
CmdLine::dump(ostream & os, unsigned level) const
{
#ifdef DEBUG_CMDLINE
   ::indent(os, level) << "CmdLine {\n" ;

   ::indent(os, level + 1) << "name=\"" << cmd_name << "\"\n";

   ::indent(os, level + 1) << "description=\"" << cmd_description << "\"\n";

   ::indent(os, level + 1) << "flags=" ;
   dump_cmd_flags(os, cmd_flags) << "\n";

   ::indent(os, level + 1) << "status=" ;
   dump_cmd_status(os, cmd_status) << "\n";

   ::indent(os, level + 1) << "state=" ;
   dump_cmd_state(os, cmd_state) << "\n";

   ::indent(os, level + 1) << "parse_state=" ;
   dump_cmd_parse_state(os, cmd_parse_state) << "\n";

   ::indent(os, level + 1);
   if (cmd_matched_arg == NULL) {
      os << "matched_arg=NULL\n";
   } else {
      os << "matched_arg=" << (void *)cmd_matched_arg << "\n";
   }

   ::indent(os, level + 1) << "# valid-args-parsed="
                           << cmd_nargs_parsed << "\n" ;

   ::indent(os, level) << "}" << endl;
#endif
}


   // Dump the arguments of a CmdLine
void
CmdLine::dump_args(ostream & os, unsigned level) const
{
#ifdef DEBUG_CMDLINE
   dump_cmd_args(os, cmd_args, level);
#endif
}

