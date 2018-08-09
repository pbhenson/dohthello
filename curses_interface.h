/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#ifndef CURSES_INTERFACE_H
#define CURSES_INTERFACE_H

#include "board_logic.h"

void ci_init(int *argc, char **argv);
void ci_init_display();
void ci_update_display();
void ci_highlight_location(bl_location_t location);
void ci_stat_message(char *message);
void ci_message(char *message);
void ci_input_loop();
void ci_cleanup();
void ci_print_usage();

#endif
