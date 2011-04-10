//------------------------------------------------------------------------
// ^FILE: argiter.c - implementation of CmdLineArgIter subclasses
//
// ^DESCRIPTION:
//    This file implements the derived classes of a CmdLineArgIter that
//    are declazred in <cmdline.h>.
//
// ^HISTORY:
//    04/03/92	Brad Appleton	<bradapp@enteract.com>	Created
//-^^---------------------------------------------------------------------

#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <ctype.h>

#include "cmdline.h"

//-------------------------------------------------------- class CmdLineArgIter

CmdLineArgIter::CmdLineArgIter(void) {}

CmdLineArgIter::~CmdLineArgIter(void) {}

//----------------------------------------------------------- class CmdArgvIter

CmdArgvIter::~CmdArgvIter(void) {}

const char *
CmdArgvIter::operator()(void) {
   return  ((index != count) && (array[index])) ? array[index++] : 0 ;
}

int
CmdArgvIter::is_temporary(void) const { return  0; }

//--------------------------------------------------------- class CmdStrTokIter

static const char WHITESPACE[] = " \t\n\r\v\f" ;

   // Constructor
CmdStrTokIter::CmdStrTokIter(const char * tokens, const char * delimiters)
   : tokstr(NULL), seps(NULL), token(NULL)
{
   reset(tokens, delimiters);
}

   // Destructor
CmdStrTokIter::~CmdStrTokIter(void)
{
   delete  tokstr;
}

   // Use a new string and a new set of delimiters
void
CmdStrTokIter::reset(const char * tokens, const char * delimiters)
{
   seps = delimiters;
   if (seps == NULL)  seps = WHITESPACE ;  // use default delimiters

   delete  tokstr;
   tokstr = NULL;
   token = NULL;
   if (tokens) {
      // Make a copy of the token-string (because ::strtok() modifies it)
      // and get the first token from the string
      //
      tokstr = new char[::strlen(tokens) + 1] ;
      (void) ::strcpy(tokstr, tokens);
      token = ::strtok(tokstr, seps);
   }
}

   // Iterator function -- operator()
   //   Just use ::strtok to get the next token from the string
   //
const char *
CmdStrTokIter::operator()(void)
{
   if (seps == NULL)  seps = WHITESPACE ;
   const char * result = token;
   if (token) token = ::strtok(NULL, seps);
   return  result;
}

   // The storage that we return pointers to (in operator())
   // always points to temporary space.
   //
int
CmdStrTokIter::is_temporary(void) const
{
   return  1;
}

//-------------------------------------------------------- class CmdIstreamIter

const unsigned  CmdIstreamIter::MAX_LINE_LEN = 1024 ;

   // Constructor
CmdIstreamIter::CmdIstreamIter(istream & input) : is(input), tok_iter(NULL)
{
}

   // Destructor
CmdIstreamIter::~CmdIstreamIter(void)
{
   delete  tok_iter;
}

#ifdef vms
   enum { c_COMMENT = '#' } ;
#else
   enum { c_COMMENT = '!' } ;
#endif

   // Iterator function -- operator()
   //
   // What we do is this: for each line of text in the istream, we use
   // a CmdStrTokIter to iterate over each token on the line.
   //
   // If the first non-white character on a line is c_COMMENT, then we
   // consider the line to be a comment and we ignore it.
   //
const char *
CmdIstreamIter::operator()(void)
{
   const char * result = NULL;
   if (tok_iter)  result = tok_iter->operator()();
   if (result)  return  result;
   if (! is)  return  NULL;

   char buf[CmdIstreamIter::MAX_LINE_LEN];
   do {
      *buf = '\0';
      is.getline(buf, sizeof(buf));
      char * ptr = buf;
      while (isspace(*ptr)) ++ptr;
      if (*ptr && (*ptr != c_COMMENT)) {
         if (tok_iter) {
            tok_iter->reset(ptr);
         } else {
            tok_iter = new CmdStrTokIter(ptr);
         }
         return  tok_iter->operator()();
      }
   } while (is);
   return  NULL;
}

   // We use a CmdStrTokIterator that is always temporary, thus the
   // the tokens we return are always in temporary storage
   //
int
CmdIstreamIter::is_temporary(void) const
{
   return  1;
}
