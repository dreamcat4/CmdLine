//------------------------------------------------------------------------
// ^FILE: quoted.c - implement quoted strings
//
// ^DESCRIPTION:
//     This file implements that class defined in "quoted.h"
//
// ^HISTORY:
//    05/01/92	Brad Appleton	<bradapp@enteract.com>	Created
//-^^---------------------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <ctype.h>

#include "quoted.h"

//--------------------------------------------------------------- Constructors

QuotedString::QuotedString(unsigned  max_size)
   : size(max_size)
{
   buffer = new char[size] ;
}
   

QuotedString::QuotedString(const char * str)
{
   size = ::strlen(str + 1);
   buffer = new char[size];
   if (buffer)  ::strcpy(buffer, str);
}

QuotedString::QuotedString(const char * str, unsigned  max_size)
   : size(max_size)
{
   buffer = new char[size];
   if (buffer)  ::strcpy(buffer, str);
}

QuotedString::QuotedString(const QuotedString & qstr)
   : size(qstr.size)
{
   buffer = new char[size];
   if (buffer)  ::strcpy(buffer, qstr.buffer);
}

//--------------------------------------------------------------- Destructor

QuotedString::~QuotedString(void)
{
   delete [] buffer ;
}

//--------------------------------------------------------------- Assignment

QuotedString &
QuotedString::operator=(const QuotedString & qstr)
{
   delete [] buffer ;
   size = qstr.size;
   buffer = new char[size];
   if (buffer)  ::strcpy(buffer, qstr.buffer);
   return  *this ;
}

QuotedString &
QuotedString::operator=(const char * str)
{
   delete [] buffer ;
   size = ::strlen(str) + 1;
   buffer = new char[size];
   if (buffer)  ::strcpy(buffer, str);
   return  *this ;
}

//--------------------------------------------------------------- operator>>

istream &
operator>>(istream & is, QuotedString & qstr)
{
      // get the first non-white character
   char  ch;
   is >> ch;
   if (! is) {
      if (is.eof()) {
         cerr << "Premature end of input.\n"
              << "\texpecting a single or a double quote." << endl ;
      } else {
         cerr << "Unable to extract quoted string from input." << endl ;
      }
      return  is;
   }

   int  single_quote = 0, double_quote = 0;

   switch (ch) {
   case '\'' :
      single_quote = 1;  break;
   case '"'  :
      double_quote = 1;  break;
   default :
      cerr << "Unexpected character '" << ch << "'.\n" 
           << "\texpecting a single or a double quotation mark." << endl ;
      is.clear(ios::failbit);
      return  is;
   } //switch


   // Now fetch into "dest" until we see the ending quote.
   char    * dest = qstr.buffer;
   unsigned  end_quote = 0;
   unsigned  len = 0;
   int  c;
   while (! end_quote) {
      int  escape = 0;
      c = is.get();
      if (! is) {
         if (is.eof()) {
            cerr << "Unmatched " << (single_quote ? "\"'\"" : "'\"'")
                 << "quote." << endl ;
         } else {
            cerr << "Unable to extract quoted string from input." << endl ;
         }
         return  is;
      }
      if (c == '\\') {
         escape = 1;
         c = is.get();
         if (! is) {
            if (is.eof()) {
               cerr << "Unmatched " << (single_quote ? "\"'\"" : "'\"'")
                    << " quote." << endl ;
            } else {
               cerr << "Unable to extract quoted string from input." << endl ;
            }
            return  is;
         }
      }
      if ((c == '\'') && single_quote && !escape) {
         end_quote = 1;
      } else if ((c == '"') && double_quote && !escape) {
         end_quote = 1;
      } else if (len < qstr.size) {
         dest[len++] = c;
      } else {
         cerr << "Error - quoted string is too long.\n\tmust be less than "
              << qstr.size << " characters." << endl ;
         is.clear(ios::failbit);
         return  is;
      }
   } //while
   
   dest[len++] = '\0' ;   // dont forget to NUL-terminate
   return  is;
}

