/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#ifndef BOARD_LOGIC_H
#define BOARD_LOGIC_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#define BL_EMPTY -1
#define BL_WHITE  0
#define BL_BLACK  1

typedef int bl_player_t;

typedef struct {
          int row, column;
        } bl_move_t;

typedef bl_move_t bl_location_t;

void bl_init();
void bl_init_board();
int bl_move_exists(bl_player_t player);
int bl_valid_move(bl_player_t player, bl_move_t move);
void bl_execute_move(bl_player_t player, bl_move_t move);
bl_move_t bl_suggest_move(bl_player_t player, int skill);
bl_player_t bl_loc_status(bl_location_t location);
int bl_count_status(bl_player_t player);



#ifdef BOARD_LOGIC_C
static int valid_move(bl_player_t player, bl_move_t move, int **board);
static void execute_move(bl_player_t player, bl_move_t move, int **board, int *count);
static int moveh_compare(void *a, void *b);

struct moveh_struct {
         bl_move_t move;
         int       h;
};

static int **bl_board;
static int bl_count[2];

#endif


#endif
