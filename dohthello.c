/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#define DOHTHELLO_C

#include <stdio.h>

#include "dohthello.h"
#include "board_logic.h"
#include "generic_interface.h"
#include "network.h"
#include "debug.h"

static bl_player_t current_player;
static int player_type[2];
static int game_status;
static int skill;
static int suggskill;

static void init(int *argc, char **argv)
{
  debug_init();
  
  bl_init();
  gi_init(argc, argv);
  doh_init(argc, argv);
  net_init(argc, argv);
}

static void doh_init(int *argc, char **argv)
{
  char **index = argv+1;

  current_player = BL_BLACK;
  game_status = DOH_GAME_PLAYING;

  if (argc)
    {
      player_type[BL_WHITE] = DOH_PLAYER_LOCAL;
      player_type[BL_BLACK] = DOH_PLAYER_LOCAL;
      skill = 2;
      suggskill = 2;
      while (*index)
        {
          TRACE("dohthello.doh_init: current argument is %s\n", *index);
          if (!strcmp(*index, "-help"))
            doh_event_notify(DOH_EV_USAGE, NULL);
          else if (!strcmp(*index, "-version"))
            {
              char buf[128];
              sprintf(buf, "dohthello: version %s\n", DOH_VERSION);
              doh_event_notify(DOH_EV_DIE, buf);
            }
          else if (!strcmp(*index, "-white"))
            if (*(++index))
              {
                if (!strcmp(*index, "local"))
                  player_type[BL_WHITE] = DOH_PLAYER_LOCAL;
                else if (!strcmp(*index, "computer"))
                  player_type[BL_WHITE] = DOH_PLAYER_COMPUTER;
                else if (!strcmp(*index, "network"))
                  player_type[BL_WHITE] = DOH_PLAYER_NETWORK;
                else
                  doh_event_notify(DOH_EV_DIE, "dohthello: invalid parameter to argument -white\n");
              }
            else
              doh_event_notify(DOH_EV_DIE, "dohthello: argument -white needs parameter {local|computer|network}\n");
          else if (!strcmp(*index, "-black"))
            if (*(++index))
              {
                if (!strcmp(*index, "local"))
                  player_type[BL_BLACK] = DOH_PLAYER_LOCAL;
                else if (!strcmp(*index, "computer"))
                  player_type[BL_BLACK] = DOH_PLAYER_COMPUTER;
                else if (!strcmp(*index, "network"))
                  player_type[BL_BLACK] = DOH_PLAYER_NETWORK;
                else
                  doh_event_notify(DOH_EV_DIE, "dohthello: invalid parameter to argument -black\n");
              }
            else
              doh_event_notify(DOH_EV_DIE, "dohthello: argument -black needs parameter {local|computer|network}\n");
          else if (!strcmp(*index, "-skill"))
            if (*(++index))
              {
                skill = atoi(*index);
                if ( (skill < 0) || (skill > 10))
                  doh_event_notify(DOH_EV_DIE, "dohthello: invalid parameter to argument -skill\n");
              }
            else
              doh_event_notify(DOH_EV_DIE, "dohthello: argument -skill needs parameter {0<=int<=10}\n");
          else if (!strcmp(*index, "-suggskill"))
            if (*(++index))
              {
                suggskill = atoi(*index);
                if ( (suggskill < 0) || (suggskill > 10))
                  doh_event_notify(DOH_EV_DIE, "dohthello: invalid parameter to argument -suggskill\n");
              }
            else
              doh_event_notify(DOH_EV_DIE, "dohthello: argument -suggskill needs parameter {0<=int<=10}\n");
          
          index++;
        }
    }

  bl_init_board();
  gi_init_display();
}

bl_player_t doh_current_player()
{
  return current_player;
}

int doh_game_status()
{
  return game_status;
}

int doh_player_type(bl_player_t player)
{
  return player_type[player];
}

int doh_current_skill()
{
  return skill;
}

int doh_current_suggskill()
{
  return suggskill;
}

void doh_event_notify(int event, void *event_data)
{
  TRACE("dohthello.doh_event_notify: called with event type %d\n", event);
  switch (event)
    {
    case DOH_EV_MOVE:
      {
        bl_move_t *move = (bl_move_t *)event_data;

        if (bl_valid_move(current_player, *move))
          {
            TRACE("dohthello.doh_event_notify: executing move %s (%d,%d)\n",
                  (current_player == BL_WHITE) ? "white" : "black", move->row, move->column);

            bl_execute_move(current_player, *move);

            current_player = !current_player;

            if ((net_active()) && (doh_player_type(current_player) == DOH_PLAYER_NETWORK))
              net_send_event(NET_EV_MOVE, (void *)move);
            
            gi_update_display();
            if (!bl_move_exists(current_player))
              if (!bl_move_exists(!current_player))
                {
                  gi_message("Neither player has a valid move. Game over.");
                  game_status = DOH_GAME_OVER;
                }
              else
                {
                  gi_message("No valid move for current player. Passing turn.");
                  current_player = !current_player;
                  gi_update_display();
                }
          }
        else
          gi_message("That is not a valid move. Please try again.");
        
        break;
      }

    case DOH_EV_COMP_MOVE:
      {
        bl_move_t move;

        gi_stat_message("Computer is thinking...");
        move = bl_suggest_move(current_player, skill);
        gi_message("Ah hah!");
        doh_event_notify(DOH_EV_MOVE, (void *)&move);
        
      break;
      }
      
    case DOH_EV_QUIT:
      cleanup();
      exit(0);
      break;

    case DOH_EV_DIE:
      cleanup();
      fprintf(stderr, "%s\n", (char *)event_data);
      exit(1);
      break;

    case DOH_EV_PERROR:
      cleanup();
      perror((char *)event_data);
      exit(1);
      break;

    case DOH_EV_USAGE:
      cleanup();
      doh_print_usage();
      exit(0);
      break;
      
    case DOH_EV_SUGGEST:
      {
        bl_move_t *move = (bl_move_t *)event_data;
        gi_stat_message("What move should you make...");
        *move = bl_suggest_move(current_player, suggskill);
        gi_highlight_location(*move);
        gi_message("How about this one?");
        break;
      }

    case DOH_EV_NEXTMOVE:
      {
        bl_move_t *move = (bl_move_t *)event_data;
        static int cell = 0;
        int cellcount = 0;

        for( ; cellcount++ < 64; cell = (cell + 1) % 64)
          if (bl_valid_move(current_player, (bl_move_t){cell / 8, cell % 8}))
              {
                *move = (bl_move_t){cell / 8, cell % 8};
                gi_highlight_location(*move);
                cell = (cell + 1) % 64;
                return;
              }
        break;
      }

    case DOH_EV_SKILLCHANGE:
      {
        int *i = (int *)event_data;

        skill += *i;
        break;
      }
      
    case DOH_EV_SUGGCHANGE:
      {
        int *i = (int *)event_data;

        suggskill += *i;
        break;
      }
      
    case DOH_EV_NEWGAME:
      doh_init(NULL, NULL);
      break;
    }
}

static void cleanup()
{
  gi_cleanup();
  net_cleanup();
}

static void doh_print_usage()
{
  printf("Usage: dohthello [options] \n\n");
  printf("General options:\n\n");
  printf("            -help               prints this usage summary\n\n");
  printf("            -version            prints dohthello version number\n\n");
  printf("            -white [local|computer|network]\n");
  printf("                                sets type of white player\n");
  printf("                                (defaults to local)\n");
  printf("                                if white player is network, does not require\n");
  printf("                                -host option, but may include -port option\n\n");
  printf("            -black [local|computer|network]\n");
  printf("                                sets type of black player\n");
  printf("                                (defaults to local)\n");
  printf("                                if black player is network, requires -host \n");
  printf("                                option, and may include -port option\n\n");
  printf("            -skill int          sets computer skill level\n");
  printf("                                valid levels are 0-10 inclusive\n");
  printf("                                defaults to 2\n\n");
  printf("            -suggskill int      sets suggestion skill level\n");
  printf("                                valid levels are 0-10 inclusive\n");
  printf("                                defaults to 2\n\n");
  
  net_print_usage();
  gi_print_usage();
}

int main(int argc, char **argv)
{
  init(&argc, argv);

  TRACE("dohthello: entering interface input loop\n");
  gi_input_loop();
}

