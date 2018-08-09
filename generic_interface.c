/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#include "generic_interface.h"
#include "curses_interface.h"
#include "xmotif_interface.h"

#define USING_CURSES 0
#define USING_XMOTIF 1

static int current_interface;

void gi_init(int *argc, char **argv) {
  if (xmi_init(argc, argv))
    current_interface = USING_XMOTIF;
  else
    {
      ci_init(argc, argv);
      current_interface = USING_CURSES;
    }
}
  
void gi_init_display()
{
  switch (current_interface)
    {
    case USING_CURSES:
      ci_init_display();
      break;
    case USING_XMOTIF:
      xmi_init_display();
      break;
    }
}
      
void gi_update_display()
{
  switch (current_interface)
    {
    case USING_CURSES:
      ci_update_display();
      break;
    case USING_XMOTIF:
      xmi_update_display();
      break;
    }
}
     
void gi_highlight_location(bl_location_t location)
{
  switch (current_interface)
    {
    case USING_CURSES:
      ci_highlight_location(location);
      break;
    case USING_XMOTIF:
      xmi_highlight_location(location);
      break;
    }
}
     
void gi_stat_message(char *message)
{
  switch (current_interface)
    {
    case USING_CURSES:
      ci_stat_message(message);
      break;
    case USING_XMOTIF:
      xmi_stat_message(message);
      break;
    }
}
     
void gi_message(char *message)
{
  switch (current_interface)
    {
    case USING_CURSES:
      ci_message(message);
      break;
    case USING_XMOTIF:
      xmi_message(message);
      break;
    }
}
     
void gi_input_loop()
{
  switch (current_interface)
    {
    case USING_CURSES:
      ci_input_loop();
      break;
    case USING_XMOTIF:
      xmi_input_loop();
      break;
    }
}
     
void gi_cleanup()
{
  switch (current_interface)
    {
    case USING_CURSES:
      ci_cleanup();
      break;
    case USING_XMOTIF:
      xmi_cleanup();
      break;
    }
}

void gi_print_usage()
{
  /* if curses support */
  ci_print_usage();
  /* if Xmotif support */
  xmi_print_usage();
}
