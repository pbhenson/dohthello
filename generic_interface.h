/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#ifndef GENERIC_INTERFACE_H
#define GENERIC_INTERFACE_H

#include "board_logic.h"

void gi_init(int *argc, char **argv);
void gi_init_display();
void gi_update_display();
void gi_highlight_location(bl_location_t location);
void gi_stat_message(char *message);
void gi_message(char *message);
void gi_input_loop();
void gi_cleanup();
void gi_print_usage();

#endif
