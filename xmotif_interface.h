/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#ifndef XMOTIF_INTERFACE_H
#define XMOTIF_INTERFACE_H

#include "board_logic.h"

int xmi_init(int *argc, char **argv);
void xmi_init_display();
void xmi_update_display();
void xmi_highlight_location(bl_location_t location);
void xmi_stat_message(char *message);
void xmi_message(char *message);
void xmi_input_loop();
void xmi_cleanup();
void xmi_print_usage();

#endif
