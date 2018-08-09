/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#define BOARD_LOGIC_C

#include <stdio.h>
#include <memory.h>
#include "board_logic.h"
#include "generic_interface.h"
#include "debug.h"


int **create_board()
{
  int index;
  int **board = (int **)malloc(sizeof(int *) * 8);

  for(index=0; index<8; index++)
    board[index] = (int *)malloc(sizeof(int) * 8);
  
  return board;
}

void free_board(int **board)
{
  int index;

  for(index=0; index<8; index++)
    free((char *)board[index]);

  free((char *)board);
}

void copy_board(int **to_board, int **from_board)
{
  int index;

  for(index=0; index<8; index++)
    memcpy(to_board[index], from_board[index], sizeof(int)*8);
}

void bl_init()
{
  int index;
  bl_board = create_board();
}


void bl_init_board()
{
  int row, column;

  for(row=0; row<8; row++)
    for(column=0; column<8; column++)
      bl_board[row][column] = BL_EMPTY;

  bl_board[3][3] = bl_board[4][4] = BL_WHITE;
  bl_board[3][4] = bl_board[4][3] = BL_BLACK;

  bl_count[BL_WHITE] = bl_count[BL_BLACK] = 2;
}

int bl_move_exists(bl_player_t player)
{
  int row, column;

  for(row=0; row<8; row++)
    for(column=0; column<8; column++)
      if (bl_valid_move(player, (bl_move_t){ row, column }))
        return TRUE;

  return FALSE;
}

int bl_valid_move(bl_player_t player, bl_move_t move)
{
  return valid_move(player, move, bl_board);
}

int valid_move(bl_player_t player, bl_move_t move, int **board)
{
  int row, column;

  if (board[move.row][move.column] != BL_EMPTY)
    return FALSE;

  /* Check up */

  if (move.row > 1) 
    for(row=move.row-1;
        (row > 0) && (board[row][move.column] == !player);
        row--)
      if (board[row-1][move.column] == player)
        return TRUE;

  /* Check up-right */

  if ( (move.row > 1) && (move.column < 6))
    for(row=move.row-1, column=move.column+1;
        (row > 0) && (column < 7) && (board[row][column] == !player);
        row--, column++)
      if (board[row-1][column+1] == player)
        return TRUE;

  /* Check right */

  if (move.column < 6)
    for(column=move.column+1;
        (column < 7) && (board[move.row][column] == !player);
        column++)
      if (board[move.row][column+1] == player)
        return TRUE;

  /* Check down-right */

  if ( (move.row < 6) && (move.column < 6))
    for(row=move.row+1, column=move.column+1;
        (row < 7) && (column < 7) && (board[row][column] == !player);
        row++, column++)
      if (board[row+1][column+1] == player)
        return TRUE;

  /* Check down */

  if (move.row < 6)
    for(row=move.row+1;
        (row < 7) && (board[row][move.column] == !player);
        row++)
      if (board[row+1][move.column] == player)
        return TRUE;

  /* Check down-left */

  if ( (move.row < 6) && (move.column > 1))
    for(row=move.row+1, column=move.column-1;
        (row < 7) && (column > 0) && (board[row][column] == !player);
        row++, column--)
      if (board[row+1][column-1] == player)
        return TRUE;

  /* Check left */

  if (move.column > 1)
    for(column=move.column-1;
        (column > 0) && (board[move.row][column] == !player);
        column--)
      if (board[move.row][column-1] == player)
        return TRUE;

  /* Check up-left */

  if ((move.row > 1) && (move.column > 1))
    for(row=move.row-1, column=move.column-1;
        (row > 0) && (column > 0) && (board[row][column] == !player);
        row--, column--)
      if (board[row-1][column-1] == player)
        return TRUE;

  return FALSE;
}

void bl_execute_move(bl_player_t player, bl_move_t move)
{
  execute_move(player, move, bl_board, bl_count);
}


void execute_move(bl_player_t player, bl_move_t move, int **board, int *count)
{
  int row, column;

  board[move.row][move.column] = player;
  count[player]++;

  /* Check up */

  if (move.row > 1) 
    for(row=move.row-1;
        (row > 0) && (board[row][move.column] == !player);
        row--)
      if (board[row-1][move.column] == player)
        {
          for( ; row < move.row; row++)
            {
              board[row][move.column] = player;
              count[player]++;
              count[!player]--;
            }
          break;
        }

  /* Check up-right */

  if ( (move.row > 1) && (move.column < 6))
    for(row=move.row-1, column=move.column+1;
        (row > 0) && (column < 7) && (board[row][column] == !player);
        row--, column++)
      if (board[row-1][column+1] == player)
        {
          for( ; row < move.row; row++, column--)
            {
              board[row][column] = player;
              count[player]++;
              count[!player]--;
            }
          break;
        }

  /* Check right */

  if (move.column < 6)
    for(column=move.column+1;
        (column < 7) && (board[move.row][column] == !player);
        column++)
      if (board[move.row][column+1] == player)
        {
          for( ; column > move.column; column--)
            {
              board[move.row][column] = player;
              count[player]++;
              count[!player]--;
            }
          break;
        }

  /* Check down-right */

  if ( (move.row < 6) && (move.column < 6))
    for(row=move.row+1, column=move.column+1;
        (row < 7) && (column < 7) && (board[row][column] == !player);
        row++, column++)
      if (board[row+1][column+1] == player)
        {
          for( ; row > move.row; row--, column--)
            {
              board[row][column] = player;
              count[player]++;
              count[!player]--;
            }
          break;
        }

  /* Check down */

  if (move.row < 6)
    for(row=move.row+1;
        (row < 7) && (board[row][move.column] == !player);
        row++)
      if (board[row+1][move.column] == player)
        {
          for( ; row > move.row; row--)
            {
              board[row][move.column] = player;
              count[player]++;
              count[!player]--;
            }
          break;
        }

  /* Check down-left */

  if ( (move.row < 6) && (move.column > 1))
    for(row=move.row+1, column=move.column-1;
        (row < 7) && (column > 0) && (board[row][column] == !player);
        row++, column--)
      if (board[row+1][column-1] == player)
        {
          for( ; row > move.row; row--, column++)
            {
              board[row][column] = player;
              count[player]++;
              count[!player]--;
            }
          break;
        }

  /* Check left */

  if (move.column > 1)
    for(column=move.column-1;
        (column > 0) && (board[move.row][column] == !player);
        column--)
      if (board[move.row][column-1] == player)
        {
          for( ; column < move.column; column++)
            {
              board[move.row][column] = player;
              count[player]++;
              count[!player]--;
            }
          break;
        }

  /* Check up-left */

  if ((move.row > 1) && (move.column > 1))
    for(row=move.row-1, column=move.column-1;
        (row > 0) && (column > 0) && (board[row][column] == !player);
        row--, column--)
      if (board[row-1][column-1] == player)
        {
          for( ; row < move.row; row++, column++)
            {
              board[row][column] = player;
              count[player]++;
              count[!player]--;
            }
          break;
        }
}

static int moveh_compare(void *a, void *b)
{
  return ((struct moveh_struct *)a)->h - ((struct moveh_struct *)b)->h;
}

bl_move_t bl_suggest_move(bl_player_t player, int skill)
{
  int row, column;
  struct moveh_struct movehs[60];
  int num_moves = 0;
  int depth = skill;

  depth -= 2;
  if (depth < 0) depth = 0;
  
  for(row=0; row<8; row++)
    for(column=0; column<8; column++)
      {
        gi_stat_message(NULL);
        if (bl_valid_move(player, (bl_move_t){row, column}))
          {
            movehs[num_moves].move = (bl_move_t){row, column};
            movehs[num_moves++].h = move_h(player, (bl_move_t){row, column}, depth, 1, bl_board, bl_count);
          }
      }

  qsort((char *)movehs, num_moves, sizeof(struct moveh_struct), moveh_compare);

  switch(skill)
    {
    case 0:
      return movehs[0].move;
      break;
    case 1:
      return movehs[num_moves/2].move;
      break;
    default:
      return movehs[num_moves-1].move;
      break;
    }
}

int move_h(bl_player_t player, bl_move_t move, int depth, int coeff, int **board, int *count)
{
  int **new_board;
  int new_count[2];
  int row, column;
  int max_h = -64;
  int tmp_h;

  new_board = create_board();
  copy_board(new_board, board);
  memcpy(new_count, count, sizeof(int)*2);

  gi_stat_message(NULL);
  execute_move((coeff == 1) ? player : !player, move, new_board, new_count);

  if (depth == 0)
    {
      max_h = new_count[player] - new_count[!player];
      if ( ((move.row==0) && (move.column==0)) ||
           ((move.row==0) && (move.column==7)) ||
           ((move.row==7) && (move.column==0)) ||
           ((move.row==7) && (move.column==7)) )
        max_h += coeff*5;
      else if ( (move.row==0) || (move.row==7) || (move.column==0) || (move.column==7) )
        max_h += coeff*3;
      else if ( ((move.row==2) && (move.column>1) && (move.column<6)) ||
                ((move.row==5) && (move.column>1) && (move.column<6)) ||
                ((move.column==2) && (move.row>1) && (move.row<6)) ||
                ((move.column==5)  && (move.row>1) && (move.row<6)) )
        max_h += coeff*1;
    }
  else
    {
      for(row=0; row<8; row++)
        for(column=0; column<8; column++)
          {
            if (valid_move((coeff == 1) ? !player : player, (bl_move_t){row, column}, new_board))
              {
                tmp_h = move_h((coeff == 1) ? !player : player, (bl_move_t){row, column}, depth-1, -coeff, new_board, new_count);
                if ( (coeff*tmp_h) > max_h)
                  max_h = tmp_h;
              }
          }
    }
  free_board(new_board);
  return max_h;
}

bl_player_t bl_loc_status(bl_location_t location)
{
  return bl_board[location.row][location.column];
}

int bl_count_status(bl_player_t player)
{
  return bl_count[player];
}
