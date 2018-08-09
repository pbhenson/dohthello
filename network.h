/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#ifndef NETWORK_H
#define NETWORK_H

#include "board_logic.h"

#define NET_PROT_VERSION "1.0"

#define NET_EV_ACK     0
#define NET_EV_MOVE    1
#define NET_EV_QUIT    2
#define NET_EV_NEWGAME 3
#define NET_EV_CHAT    4

void net_init(int *argc, char **argv);
void net_send_event(int event, void *event_data);
void net_cleanup();
int net_active();
void net_print_usage();

#ifdef NETWORK_C

static void net_data_ready();
static int net_read_data(char *buf, int len);
static void net_write_data(char *buf, int len);

#endif

#endif
