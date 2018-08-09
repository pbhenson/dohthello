/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#ifndef DOHTHELLO_H
#define DOHTHELLO_H

#define DOH_VERSION "Version 0.9"

#include "board_logic.h"

#define DOH_EV_MOVE 0
#define DOH_EV_QUIT 1
#define DOH_EV_SUGGEST 2
#define DOH_EV_COMP_MOVE 3
#define DOH_EV_NEWGAME 4
#define DOH_EV_NEXTMOVE 5
#define DOH_EV_SKILLCHANGE 6
#define DOH_EV_SUGGCHANGE 7
#define DOH_EV_DIE 8
#define DOH_EV_PERROR 9
#define DOH_EV_USAGE 10

#define DOH_GAME_OVER 0
#define DOH_GAME_PLAYING 1

#define DOH_PLAYER_LOCAL 0
#define DOH_PLAYER_COMPUTER 1
#define DOH_PLAYER_NETWORK 2

void doh_event_notify(int event, void *event_data);
int doh_game_status();
bl_player_t doh_current_player();
bl_player_t doh_player_type(bl_player_t player);
int doh_current_skill();
int doh_current_suggskill();

#ifdef DOHTHELLO_C
static void doh_print_usage();
static void init(int *argc, char **argv);
static void doh_init(int *argc, char **argv);
static void cleanup();
#endif


#endif
