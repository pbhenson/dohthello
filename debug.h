/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#ifndef DEBUG_H
#define DEBUG_H

void debug_init();
void debug_cleanup();

#ifdef DEBUG

#include <stdio.h>

#define TRACE(X...) fprintf(tracelog, X); fflush(tracelog)

#ifdef DEBUG_C
FILE *tracelog;
#else
extern FILE *tracelog;
#endif


#else

#define TRACE(X...)

#endif


#endif
