/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#define CURSES_INTERFACE_C

#include <curses.h>
#include "curses_interface.h"
#include "dohthello.h"
#include "board_logic.h"
#include "debug.h"
#include "network.h"

bl_move_t curr_move;

void ci_init(int *argc, char **argv)
{
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(2);
  erase();
  refresh();

  curr_move.row = curr_move.column = 3;
}

void ci_init_display()
{
  int row, column;

  mvaddstr(0, 0,  "-------------------------------------------------------------------------------");
  move(1,0);
  printw(         "|  Dohthello %s | by Paul Henson |  Curses Interface  |  h for Help  |", DOH_VERSION);
  mvaddstr(2, 0,  "-------------------------------------------------------------------------------");
  mvaddstr(3, 0,  "|                   |                        |                                |");
  mvaddstr(4, 0,  "|                   |  Current Player:       |       Player Information       |");
  mvaddstr(5, 0,  "|                   |                        |                                |");
  mvaddstr(6, 0,  "|                   |           White:       |  White:                        |");
  mvaddstr(7, 0,  "|                   |           Black:       |  Black:                        |");
  mvaddstr(8, 0,  "|                   |                        |                                |");
  mvaddstr(9, 0,  "|                   |---------------------------------------------------------|");
  mvaddstr(10, 0, "|                   |                                                         |");
  mvaddstr(11, 0, "|___________________|_________________________________________________________|");
  mvaddstr(12, 0, "|                                                                             |");
  mvaddstr(13, 0, "| Quick guide to available keys:                                              |");
  mvaddstr(14, 0, "|                                                                             |");
  mvaddstr(15, 0, "|   Arrow keys - move cursor                                                  |");
  mvaddstr(16, 0, "|   SPACE      - make move at current cursor                                  |");
  mvaddstr(17, 0, "|   'S'        - suggest move to make                                         |");
  mvaddstr(18, 0, "|   'M'        - switch between possible moves                                |");
  mvaddstr(19, 0, "|   '+','-'    - increase,decrease computer skill level                       |");
  mvaddstr(20, 0, "|   '>','<'    - increase,decrease suggestion skill level                     |");
  mvaddstr(21, 0, "|   'N'        - start new game                                               |");
  mvaddstr(22, 0, "|   'Q'        - quit                                                         |");
  mvaddstr(23, 0, "-------------------------------------------------------------------------------");

  mvaddstr(6, 55, (doh_player_type(BL_WHITE) == DOH_PLAYER_COMPUTER) ? "Computer" :
           (doh_player_type(BL_WHITE) == DOH_PLAYER_NETWORK) ? "Network" : "Local Human");

  mvaddstr(7, 55, (doh_player_type(BL_BLACK) == DOH_PLAYER_COMPUTER) ? "Computer" :
           (doh_player_type(BL_BLACK) == DOH_PLAYER_NETWORK) ? "Network" : "Local Human");

  ci_update_display();

}

void ci_update_display()
{
  int row, column;

  for(row=0; row<8; row++)
    for(column=0; column<8; column++)
      {
        switch(bl_loc_status( (bl_location_t){row, column}))
          {
          case BL_EMPTY:
            mvaddch(row+3,2*column+3, '-');
            break;
          case BL_WHITE:
            mvaddch(row+3,2*column+3, 'W');
            break;
          case BL_BLACK:
            mvaddch(row+3,2*column+3, 'B');
            break;
          }
      }

  if (doh_current_player() == BL_WHITE)
    mvaddstr(4, 39, "White");
  else
    mvaddstr(4, 39, "Black");

  move(6,39);
  printw("%d ", bl_count_status(BL_WHITE));
  move(7,39);
  printw("%d ", bl_count_status(BL_BLACK));

  ci_highlight_location(curr_move);
}

void ci_highlight_location(bl_location_t location)
{
  move(location.row+3,2*location.column+3);
  refresh();
}

void ci_stat_message(char *message)
{
  static char stat_symbols[] = {'/', '-', '\\', '|'};
  static int curr_symbol = 0;

  if (message)
    {
      curr_symbol = 0;
      mvaddstr(10, 22, "                                                        ");
      mvaddstr(10, 24, message);
      mvaddch(10, 22, stat_symbols[curr_symbol]);
      refresh();
    }
  else
    if (!((curr_symbol = (curr_symbol + 1) % 40) % 10))
      {
        mvaddch(10, 22, stat_symbols[curr_symbol/10]);
        refresh();
      }
}



void ci_message(char *message)
{
  mvaddstr(10, 22, "                                                        ");
  mvaddstr(10, 22, message);
  ci_highlight_location(curr_move);
  refresh();
}

void ci_input_loop()
{
  int c;
  

  if ( ((doh_player_type(BL_WHITE) == DOH_PLAYER_COMPUTER) && (doh_player_type(BL_BLACK) == DOH_PLAYER_NETWORK)) ||
       ((doh_player_type(BL_BLACK) == DOH_PLAYER_COMPUTER) && (doh_player_type(BL_WHITE) == DOH_PLAYER_NETWORK)))
    nodelay(stdscr, TRUE);

  while(1)
    {
      if ( (doh_player_type(doh_current_player()) == DOH_PLAYER_COMPUTER)
           && (doh_game_status() != DOH_GAME_OVER))
        doh_event_notify(DOH_EV_COMP_MOVE, NULL);
      else
        {
          c = getch();
          switch(c)
            {
            case KEY_UP:
              ci_message("");
              if (curr_move.row > 0)
                {
                  curr_move.row--;
                  ci_highlight_location(curr_move);
                }
              break;
        
            case KEY_DOWN:
              ci_message("");
              if (curr_move.row < 7)
                {
                  curr_move.row++;
                  ci_highlight_location(curr_move);
                }
              break;
        
            case KEY_LEFT:
              ci_message("");
              if (curr_move.column > 0)
                {
                  curr_move.column--;
                  ci_highlight_location(curr_move);
                }
              break;
              
            case KEY_RIGHT:
              ci_message("");
              if (curr_move.column < 7)
                {
                  curr_move.column++;
                  ci_highlight_location(curr_move);
                }
              break;

            case ' ':
              if (doh_game_status() == DOH_GAME_OVER)
                ci_message("Game over! Press 'N' to start new game.");
              else if (doh_player_type(doh_current_player()) == DOH_PLAYER_NETWORK)
                ci_message("Waiting for remote move from network, have patience.");
              else
                doh_event_notify(DOH_EV_MOVE, (void *)&curr_move);
              break;
        
            case 'S':
            case 's':
              if (doh_game_status() == DOH_GAME_OVER)
                ci_message("Game over! Press 'N' to start new game.");
              else if (doh_player_type(doh_current_player()) == DOH_PLAYER_NETWORK)
                ci_message("Waiting for remote move from network, have patience.");
              else
                doh_event_notify(DOH_EV_SUGGEST, (void *)&curr_move);
              break;

            case 'M':
            case 'm':
              if (doh_game_status() == DOH_GAME_OVER)
                ci_message("Game over! Press 'N' to start new game.");
              else if (doh_player_type(doh_current_player()) == DOH_PLAYER_NETWORK)
                ci_message("Waiting for remote move from network, have patience.");
              else
                doh_event_notify(DOH_EV_NEXTMOVE, (void *)&curr_move);
              break;

            case '+':
              {
                int i = 1;
                char buf[128];
            
                doh_event_notify(DOH_EV_SKILLCHANGE, (void *)&i);
                sprintf(buf, "New computer skill level is %d", doh_current_skill());
                ci_message(buf);
                break;
              }

            case '-':
              {
                int i = -1;
                char buf[128];
            
                if (doh_current_skill() > 0)
                  {
                    doh_event_notify(DOH_EV_SKILLCHANGE, (void *)&i);
                    sprintf(buf, "New computer skill level is %d", doh_current_skill());
                    ci_message(buf);
                  }
                else
                  ci_message("Sorry, you can't have a negative computer skill level.");
                break;
              }

            case '>':
              {
                int i = 1;
                char buf[128];
            
                doh_event_notify(DOH_EV_SUGGCHANGE, (void *)&i);
                sprintf(buf, "New suggestion skill level is %d", doh_current_suggskill());
                ci_message(buf);
                break;
              }

            case '<':
              {
                int i = -1;
                char buf[128];
            
                if (doh_current_suggskill() > 0)
                  {
                    doh_event_notify(DOH_EV_SUGGCHANGE, (void *)&i);
                    sprintf(buf, "New suggestion skill level is %d", doh_current_suggskill());
                    ci_message(buf);
                  }
                else
                  ci_message("Sorry, you can't have a negative suggestion skill level.");
                break;
              }

            case 'Q':
            case 'q':
              if (net_active())
                net_send_event(NET_EV_QUIT, NULL);
              doh_event_notify(DOH_EV_QUIT, NULL);
              break;

            case 'H':
            case 'h':
              ci_message("Please see quick guide below.");
              break;

            case 'N':
            case 'n':
              if (net_active())
                net_send_event(NET_EV_NEWGAME, NULL);
              doh_event_notify(DOH_EV_NEWGAME, NULL);
              break;
            case ERR:
              sleep(1);
            }
        }
    }
}

void ci_cleanup()
{
  endwin();
}

void ci_print_usage()
{
}
