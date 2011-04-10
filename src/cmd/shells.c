//------------------------------------------------------------------------
// ^FILE: shells.c - implement classes for the various Unix shells
//
// ^DESCRIPTION:
//     This file packages all the information we need to know about each
//  of the shells that cmdparse(1) will support into a set of (sub)classes.
//
// ^HISTORY:
//    04/19/92	Brad Appleton	<bradapp@enteract.com>	Created
//-^^---------------------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <ctype.h>

#include <fifolist.h>

#include "shells.h"
#include "argtypes.h"

//--------------------------------------------------------------- ShellVariable

ShellVariable::ShellVariable(const char * name)
   : var_name(name), var_value(NULL)
{
}

ShellVariable::~ShellVariable(void)
{
}

//------------------------------------------------------------ ShellArrayValues

DECLARE_FIFO_LIST(CharPtrList, char *);

struct  ShellArrayValues {
   CharPtrList       list;
   CharPtrListArray  array;

   ShellArrayValues(void);
} ;

ShellArrayValues::ShellArrayValues(void)
   : array(list)
{
   list.self_cleaning(1);
}

//------------------------------------------------------------------ ShellArray

ShellArray::ShellArray(const char * name)
   : array_name(name), array_value(NULL)
{
}

ShellArray::~ShellArray(void)
{
   delete  array_value ;
}

void
ShellArray::append(const char * value)
{
   if (array_value == NULL) {
      array_value = new ShellArrayValues ;
   }
   char ** valptr = new char* ;
   if (valptr) {
      *valptr = (char *)value;
      array_value->list.add(valptr);
   }
}

unsigned
ShellArray::count(void) const
{
   return  ((array_value) ? array_value->list.count() : 0);
}

const char *
ShellArray::operator[](unsigned  index) const
{
   return  ((array_value) ? array_value->array[index] : NULL);
}

//----------------------------------------------------------- AbstractUnixShell

AbstractUnixShell::~AbstractUnixShell(void)
{
}

//------------------------------------------------------------------- UnixShell

UnixShell::UnixShell(const char * shell_name)
   : shell(NULL), valid(1)
{
   if (::strcmp(BourneShell::NAME, shell_name) == 0) {
      shell = new BourneShell ;
   } else if (::strcmp("ash", shell_name) == 0) {
      shell = new BourneShell ;
   } else if (::strcmp(KornShell::NAME, shell_name) == 0) {
      shell = new KornShell ;
   } else if (::strcmp(BourneAgainShell::NAME, shell_name) == 0) {
      shell = new BourneAgainShell ;
   } else if (::strcmp(CShell::NAME, shell_name) == 0) {
      shell = new CShell ;
   } else if (::strcmp("tcsh", shell_name) == 0) {
      shell = new CShell ;
   } else if (::strcmp("itcsh", shell_name) == 0) {
      shell = new CShell ;
   } else if (::strcmp(ZShell::NAME, shell_name) == 0) {
      shell = new ZShell ;
   } else if (::strcmp(Plan9Shell::NAME, shell_name) == 0) {
      shell = new Plan9Shell ;
   } else if (::strcmp(PerlShell::NAME, shell_name) == 0) {
      shell = new PerlShell ;
   } else if (::strcmp(TclShell::NAME, shell_name) == 0) {
      shell = new TclShell ;
   } else {
      valid = 0;
   }
}

UnixShell::~UnixShell(void)
{
   delete  shell;
}

const char *
UnixShell::name(void) const
{
   return  ((shell) ? shell->name() : NULL);
}

void
UnixShell::unset_args(const char * name) const
{
   if (shell)  shell->unset_args(name);
}

int
UnixShell::is_positionals(const char * name) const
{
   return  ((shell) ? shell->is_positionals(name) : 0);
}

void
UnixShell::set(const ShellVariable & variable) const
{
   if (shell)  shell->set(variable);
}

void
UnixShell::set(const ShellArray & array, int variant) const
{
   if (shell)  shell->set(array, variant);
}

//----------------------------------------------------------------- varname

// Remove any "esoteric" portions of a vraible name (such as a leading '$')
//
inline  static  const char *
varname(const char * name, char skip)
{
   return  ((*name == skip) && (*(name + 1))) ? (name + 1): name ;
}

//----------------------------------------------------------------- BourneShell

const char * BourneShell::NAME = "sh" ;

BourneShell::BourneShell(void)
{
}

BourneShell::~BourneShell(void)
{
}

const char *
BourneShell::name(void) const
{
   return  BourneShell::NAME ;
}

void
BourneShell::unset_args(const char *) const
{
   cout << "shift $# ;" << endl ;
}

int
BourneShell::is_positionals(const char * name) const
{
   name = varname(name, '$');
   return  ((::strcmp(name, "--") == 0) || (::strcmp(name, "-") == 0) ||
            (::strcmp(name, "@") == 0)  || (::strcmp(name, "*") == 0)) ;
}

void
BourneShell::set(const ShellVariable & variable) const
{
   const char * name = varname(variable.name(), '$');
   if (is_positionals(name)) {
      cout << "set -- '" ;
   } else {
      cout << name << "='" ;
   }
   escape_value(variable.value());
   cout << "';" << endl ;
}

void
BourneShell::set(const ShellArray & array, int variant) const
{
   int  ndx;
   const char * name = varname(array.name(), '$');

   if (is_positionals(name)) {
      // set -- 'arg1' 'arg2' ...
      cout << "set -- ";
      for (ndx = 0 ; ndx < array.count() ; ndx++) {
         if (ndx)  cout << ' ' ;
         cout << '\'' ;
         escape_value(array[ndx]);
         cout << '\'' ;
      }
      cout << ';' << endl ;
   } else if (variant) {
      // argname_count=N
      // argname1='arg1'
      //   ...
      // argnameN='argN'
      cout << name << "_count=" << array.count() << ';' << endl ;
      for (ndx = 0 ; ndx < array.count() ; ndx++) {
         cout << name << (ndx + 1) << "='";
         escape_value(array[ndx]);
         cout << "';" << endl ;
      }
   } else {
      // argname='arg1 arg2 ...'
      cout << name << "='";
      for (ndx = 0 ; ndx < array.count() ; ndx++) {
         if (ndx)  cout << ' ' ;
         escape_value(array[ndx]);
      }
      cout << "';" << endl ;
   }
}

void
BourneShell::escape_value(const char * value) const
{
   for ( ; *value ; value++) {
      switch (*value) {
      case '\'' :
         cout << "'\\''" ;
         break ;

      case '\\' :
      case '\b' :
      case '\r' :
      case '\v' :
      case '\f' :
         cout << '\\' ;    // fall thru to default case
      default :
         cout << char(*value) ;
      }
   } //for
}

//------------------------------------------------------------------- KornShell

const char * KornShell::NAME = "ksh" ;

KornShell::KornShell(void)
{
}

KornShell::~KornShell(void)
{
}

const char *
KornShell::name(void) const
{
   return  KornShell::NAME ;
}

void
KornShell::unset_args(const char *) const
{
   cout << "set -- ;" << endl ;
}

void
KornShell::set(const ShellVariable & variable) const
{
   BourneShell::set(variable);
} 

void
KornShell::set(const ShellArray & array, int variant) const
{
   const char * name = varname(array.name(), '$');
   if (is_positionals(name)) {
      cout << "set -- " ;
   } else {
      cout << "set " << (variant ? '+' : '-') << "A " << name << ' ' ;
   }
   for (int  ndx = 0 ; ndx < array.count() ; ndx++) {
      if (ndx)  cout << ' ' ;
      cout << '\'' ;
      escape_value(array[ndx]);
      cout << '\'' ;
   }
   cout << ';' << endl ;
}

//------------------------------------------------------------ BourneAgainShell

const char * BourneAgainShell::NAME = "bash" ;

BourneAgainShell::BourneAgainShell(void)
{
}

BourneAgainShell::~BourneAgainShell(void)
{
}

const char *
BourneAgainShell::name(void) const
{
   return  BourneAgainShell::NAME ;
}

void
BourneAgainShell::set(const ShellVariable & variable) const
{
   BourneShell::set(variable);
} 

void
BourneAgainShell::set(const ShellArray & array, int variant) const
{
   BourneShell::set(array, variant);
}

//---------------------------------------------------------------------- CShell

const char * CShell::NAME = "csh" ;

CShell::CShell(void)
{
}

CShell::~CShell(void)
{
}

const char *
CShell::name(void) const
{
   return  CShell::NAME ;
}

void
CShell::unset_args(const char *) const
{
   cout << "set argv=();" << endl ;
}

int
CShell::is_positionals(const char * name) const
{
   name = varname(name, '$');
   return  (::strcmp(name, "argv") == 0);
}

void
CShell::set(const ShellVariable & variable) const
{
   const char * name = varname(variable.name(), '$');
   int  posl = is_positionals(name);
   cout << "set " << name << '=' ;
   if (posl)  cout << '(' ;
   cout << '\'' ;
   escape_value(variable.value());
   cout << '\'' ;
   if (posl)  cout << ')' ;
   cout << ';' << endl ;;
}

void
CShell::set(const ShellArray & array, int ) const
{
   cout << "set " << varname(array.name(), '$') << "=(" ;
   for (int  ndx = 0 ; ndx < array.count() ; ndx++) {
      if (ndx)  cout << ' ' ;
      cout << '\'' ;
      escape_value(array[ndx]);
      cout << '\'' ;
   }
   cout << ");" << endl ;
}

void
CShell::escape_value(const char * value) const
{
   for ( ; *value ; value++) {
      switch (*value) {
      case '\'' :
         cout << "'\\''" ;
         break ;

      case '!' :
      case '\n' :
      case '\b' :
      case '\r' :
      case '\v' :
      case '\f' :
         cout << '\\' ;    // fall thru to default case
      default :
         cout << char(*value) ;
      }
   } //for
}

//---------------------------------------------------------------------- ZShell

const char * ZShell::NAME = "zsh" ;

ZShell::ZShell(void)
{
}

ZShell::~ZShell(void)
{
}

const char *
ZShell::name(void) const
{
   return  ZShell::NAME ;
}

void
ZShell::unset_args(const char *) const
{
   cout << "argv=();" << endl ;
}

int
ZShell::is_positionals(const char * name) const
{
   name = varname(name, '$');
   return  ((::strcmp(name, "--") == 0) || (::strcmp(name, "-") == 0) ||
            (::strcmp(name, "@") == 0)  || (::strcmp(name, "*") == 0) ||
            (::strcmp(name, "argv") == 0));
}

void
ZShell::set(const ShellVariable & variable) const
{
   const char * name = varname(variable.name(), '$');
   int  posl = is_positionals(name);
   cout << name << '=' ;
   if (posl)  cout << '(' ;
   cout << '\'' ;
   escape_value(variable.value());
   cout << '\'' ;
   if (posl)  cout << ')' ;
   cout << ';' << endl ;;
}

void
ZShell::set(const ShellArray & array, int ) const
{
   cout << varname(array.name(), '$') << "=(" ;
   for (int  ndx = 0 ; ndx < array.count() ; ndx++) {
      if (ndx)  cout << ' ' ;
      cout << '\'' ;
      escape_value(array[ndx]);
      cout << '\'' ;
   }
   cout << ");" << endl ;
}

void
ZShell::escape_value(const char * value) const
{
   for ( ; *value ; value++) {
      switch (*value) {
      case '\'' :
         cout << "'\\''" ;
         break ;

      case '!' :
      case '\\' :
      case '\b' :
      case '\r' :
      case '\v' :
      case '\f' :
         cout << '\\' ;    // fall thru to default case
      default :
         cout << char(*value) ;
      }
   } //for
}

//------------------------------------------------------------------ Plan9Shell

const char * Plan9Shell::NAME = "rc" ;

Plan9Shell::Plan9Shell(void)
{
}

Plan9Shell::~Plan9Shell(void)
{
}

const char *
Plan9Shell::name(void) const
{
   return  Plan9Shell::NAME ;
}

void
Plan9Shell::unset_args(const char *) const
{
   cout << "*=();" << endl ;
}

int
Plan9Shell::is_positionals(const char * name) const
{
   name = varname(name, '$');
   return  (::strcmp(name, "*") == 0);
}

void
Plan9Shell::set(const ShellVariable & variable) const
{
   const char * name = varname(variable.name(), '$');
   int  posl = is_positionals(name);
   cout << name << '=' ;
   if (posl)  cout << '(' ;
   cout << '\'' ;
   escape_value(variable.value());
   cout << '\'' ;
   if (posl)  cout << ')' ;
   cout << ';' << endl ;;
}

void
Plan9Shell::set(const ShellArray & array, int ) const
{
   cout << varname(array.name(), '$') << "=(" ;
   for (int  ndx = 0 ; ndx < array.count() ; ndx++) {
      if (ndx)  cout << ' ' ;
      cout << '\'' ;
      escape_value(array[ndx]);
      cout << '\'' ;
   }
   cout << ");" << endl ;
}

void
Plan9Shell::escape_value(const char * value) const
{
   for ( ; *value ; value++) {
      switch (*value) {
      case '\'' :
         cout << "''" ;
         break ;

      case '\\' :
      case '\b' :
      case '\r' :
      case '\v' :
      case '\f' :
         cout << '\\' ;    // fall thru to default case
      default :
         cout << char(*value) ;
      }
   } //for
}

//------------------------------------------------------------------- PerlShell

const char * PerlShell::NAME = "perl" ;

PerlShell::PerlShell(void)
{
   static const char perl_true[]  = "1" ;
   static const char perl_false[] = "0" ;

      // use different defaults for TRUE and FALSE
   ShellCmdArgBool::True(perl_true);
   ShellCmdArgBool::False(perl_false);
}

PerlShell::~PerlShell(void)
{
}

const char *
PerlShell::name(void) const
{
   return  PerlShell::NAME ;
}

void
PerlShell::unset_args(const char *) const
{
   cout << "@ARGV = ();" << endl ;
}

int
PerlShell::is_positionals(const char * name) const
{
   name = varname(name, '@');
   return  (::strcmp(name, "ARGV") == 0);
}

void
PerlShell::set(const ShellVariable & variable) const
{
   const char * name = varname(variable.name(), '$');
   int array = (*name == '@') ;
   cout << (array ? "" : "$") << name << " = " ;
   if (array)  cout << '(' ;
   cout << '\'' ;
   escape_value(variable.value());
   cout << '\'' ;
   if (array)  cout << ')' ;
   cout << ';' << endl ;;
}

void
PerlShell::set(const ShellArray & array, int ) const
{
   const char * name = varname(array.name(), '@');
   int scalar = (*name == '$') ;
   cout << (scalar ? "" : "@") << name << " = " ;
   cout << (scalar ? '\'' : '(') ;
   for (int  ndx = 0 ; ndx < array.count() ; ndx++) {
      if (ndx)  cout << (scalar ? " " : ", ") ;
      if (! scalar)  cout << '\'' ;
      escape_value(array[ndx]);
      if (! scalar)  cout << '\'' ;
   }
   cout << (scalar ? '\'' : ')') ;
   cout << ";" << endl ;
}

void
PerlShell::escape_value(const char * value) const
{
   for ( ; *value ; value++) {
      switch (*value) {
      case '\t' :  cout << "\\t" ; break ;
      case '\n' :  cout << "\\n" ; break ;
      case '\b' :  cout << "\\b" ; break ;
      case '\r' :  cout << "\\r" ; break ;
      case '\v' :  cout << "\\v" ; break ;
      case '\f' :  cout << "\\f" ; break ;

      case '\'' :
      case '\\' :
         cout << "\\" ;    // fall thru to default
      default :
         cout << char(*value) ;
      }
   } //for
}

//------------------------------------------------------------------- TclShell

const char * TclShell::NAME = "tcl" ;

TclShell::TclShell(void)
{
   static const char tcl_true[]  = "1" ;
   static const char tcl_false[] = "0" ;

      // use different defaults for TRUE and FALSE
   ShellCmdArgBool::True(tcl_true);
   ShellCmdArgBool::False(tcl_false);
}

TclShell::~TclShell(void)
{
}

const char *
TclShell::name(void) const
{
   return  TclShell::NAME ;
}

void
TclShell::unset_args(const char * name) const
{
   cout << "set " << varname(name, '$') << " {};" << endl ;
}

int
TclShell::is_positionals(const char * name) const
{
   name = varname(name, '$');
   return  ((::strcmp(name, "argv") == 0) || (::strcmp(name, "args") == 0));
}

void
TclShell::set(const ShellVariable & variable) const
{
   const char * name = varname(variable.name(), '$');
   cout << "set " << name << ' ' ;
   cout << '"' ;
   escape_value(variable.value());
   cout << '"' ;
   cout << ';' << endl ;;
}

void
TclShell::set(const ShellArray & array, int ) const
{
   const char * name = varname(array.name(), '@');
   int scalar = (*name == '$') ;
   cout << "set " << name << " [ list " ;
   for (int  ndx = 0 ; ndx < array.count() ; ndx++) {
      if (ndx)  cout << ' ' ;
      cout << '"' ;
      escape_value(array[ndx]);
      cout << '"' ;
   }
   cout << " ]" ;
   cout << ";" << endl ;
}

void
TclShell::escape_value(const char * value) const
{
   for ( ; *value ; value++) {
      switch (*value) {
      case '\t' :  cout << "\\t" ; break ;
      case '\n' :  cout << "\\n" ; break ;
      case '\b' :  cout << "\\b" ; break ;
      case '\r' :  cout << "\\r" ; break ;
      case '\v' :  cout << "\\v" ; break ;
      case '\f' :  cout << "\\f" ; break ;

      case '\'' :
      case '\\' :
      case '{' :
      case '}' :
      case '[' :
      case ']' :
      case '$' :
      case ';' :
      case '"' :
         cout << "\\" ;    // fall thru to default
      default :
         cout << char(*value) ;
      }
   } //for
}

