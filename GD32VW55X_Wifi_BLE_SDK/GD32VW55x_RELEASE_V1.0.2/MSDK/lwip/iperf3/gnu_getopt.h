/* Declarations for gnu_getopt.
   Copyright (C) 1989,90,91,92,93,94,96,97 Free Software Foundation, Inc.

   This file is part of the GNU C Library.  Its master source is NOT part of
   the C library, however.  The master source lives in /gd/gnu/lib.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/*
 * modified July 9, 1999 by mark gates <mgates@nlanr.net>
 *          Dec 17, 1999
 *
 * renamed all functions and variables by prepending "gnu_"
 * removed/redid a bunch of stuff under the assumption we're
 *   using a modern standard C compiler.
 *
 * $Id: gnu_getopt.h,v 1.1.1.1 2004/05/18 01:50:44 kgibbs Exp $
 */

#ifndef _GETOPT_H
#define _GETOPT_H


#define NO_ENVIRONMENT

/* For communication from `gnu_getopt' to the caller.
   When `gnu_getopt' finds an option that takes an argument,
   the argument value is returned here.
   Also, when `ordering' is RETURN_IN_ORDER,
   each non-option ARGV-element is returned here.  */

extern char *gnu_optarg;

/* Index in ARGV of the next element to be scanned.
   This is used for communication to and from the caller
   and for communication between successive calls to `gnu_getopt'.

   On entry to `gnu_getopt', zero means this is the first call; initialize.

   When `gnu_getopt' returns -1, this is the index of the first of the
   non-option elements that the caller should itself scan.

   Otherwise, `gnu_optind' communicates from one call to the next
   how much of ARGV has been scanned so far.  */

extern int gnu_optind;

/* Callers store zero here to inhibit the error message `gnu_getopt' prints
   for unrecognized options.  */

extern int gnu_opterr;

/* Set to an option character which was unrecognized.  */

extern int gnu_optopt;

/* Describe the long-named options requested by the application.
   The LONG_OPTIONS argument to gnu_getopt_long or getopt_long_only is a vector
   of `struct option' terminated by an element containing a name which is
   zero.

   The field `has_arg' is:
   no_argument       (or 0) if the option does not take an argument,
   required_argument (or 1) if the option requires an argument,
   optional_argument (or 2) if the option takes an optional argument.

   If the field `flag' is not NULL, it points to a variable that is set
   to the value given in the field `val' when the option is found, but
   left unchanged if the option is not found.

   To have a long-named option do something other than set an `int' to
   a compiled-in constant, such as set a value from `gnu_optarg', set the
   option's `flag' field to zero and its `val' field to a nonzero
   value (the equivalent single-letter option character, if there is
   one).  For long options that have a zero `flag' field, `gnu_getopt'
   returns the contents of the `val' field.  */

/* has_arg can't be an enum because some compilers complain about
   type mismatches in all the code that assumes it is an int.  */

struct option {
   const char *name;
   int has_arg;
   int *flag;
   int val;
};

/* Names for the values of the `has_arg' field of `struct option'.  */

#define no_argument        0
#define required_argument  1
#define optional_argument  2

int gnu_getopt( int argc,
                char *const *argv,
                const char *shortopts );

int gnu_getopt_long( int argc,
                     char *const *argv,
                     const char *shortopts,
                     const struct option *longopts,
                     int *longind );

int gnu_getopt_long_only( int argc,
                          char *const *argv,
                          const char *shortopts,
                          const struct option *longopts,
                          int *longind );

/* Internal only.  Users should not call this directly.  */
int _gnu_getopt_internal( int argc,
                          char *const *argv,
                          const char *shortopts,
                          const struct option *longopts,
                          int *longind,
                          int long_only );

void gnu_getopt_reset( void );


#endif /* _GETOPT_H */
