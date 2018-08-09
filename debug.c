/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#define DEBUG_C

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "debug.h"


void debug_init()
{
#ifdef DEBUG
  tracelog = fopen("/tmp/dohthello.log", "w");
  chmod("/tmp/dohthello.log",
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IROTH);
  TRACE("dohthello.debug: initializing trace log\n");
#endif
}

void debug_cleanup()
{
#ifdef DEBUG
  fclose(tracelog);
#endif
}
