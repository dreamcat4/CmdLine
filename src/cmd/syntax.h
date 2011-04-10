//------------------------------------------------------------------------
// ^FILE: syntax.h - define an object to represent the syntax of a
//                   command-line argument.
//
// ^DESCRIPTION:
//    This file defines an object that parses and records the syntax of
//    a command-line argument.
//
// ^HISTORY:
//    04/29/92	Brad Appleton	<bradapp@enteract.com>	Created
//-^^---------------------------------------------------------------------

#ifndef _syntax_h
#define _syntax_h

#include  "fsm.h"

class  istream;
class  ArgSyntax {
public:
   ArgSyntax(void)
      : arg_syntax(0), arg_char(0), arg_keyword(0), arg_value(0)
      {}

      // Return the syntax flags
   unsigned
   syntax(void) const { return  arg_syntax; }

      // Return the option character
   char
   optchar(void) const { return  arg_char; }

      // Return the keyword name
   const char *
   keyword(void) const { return  arg_keyword; }

      // Return the value name
   const char *
   value(void) const { return  arg_value; }

      // Extract the syntax (compile it) from an input stream
   friend  istream &
   operator>>(istream & is, ArgSyntax & arg);

private:
   unsigned     arg_syntax ;
   char         arg_char;
   const char * arg_keyword;
   const char * arg_value;

private:
   int
   parse_syntax(const char * syntax);

   void
   parse_value(const SyntaxFSM & fsm);

   istream &
   parse_flag(istream & is);
} ;

#endif  /* _syntax_h */
