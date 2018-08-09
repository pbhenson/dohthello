/* Copyright (C) 1996 Paul Henson. Please see file COPYRIGHT for details */

#define NETWORK_C

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
/* Added for Solaris to define FASYNC */
/* Should it be #ifdef'd, or is it harmless on other platforms? */
#include <sys/file.h>

#include "debug.h"
#include "network.h"
#include "dohthello.h"
#include "board_logic.h"
#include "generic_interface.h"

static int sock = 0;
static int ack_wait = 0;

void net_init(int *argc, char **argv)
{
  char **index = argv+1;
  char *host = NULL;
  int port = 6432;

  while (*index)
    {
      if (!strcmp(*index, "-host"))
        if (*(++index))
          host = *index;
        else
          doh_event_notify(DOH_EV_DIE, "dohthello.network: -host option needs argument\n");
      else if (!strcmp(*index, "-port"))
        if (*(++index))
          {
            port = atoi(*index);
            if ( (port < 1025) || (port > 60000) )
              doh_event_notify(DOH_EV_DIE, "dohthello.network: invalid argument for -port option\n");
          }
        else
          doh_event_notify(DOH_EV_DIE, "dohthello.network: -port option needs argument\n");

      index++;
    }

  
  if ( (doh_player_type(BL_BLACK) == DOH_PLAYER_NETWORK) && (doh_player_type(BL_WHITE) == DOH_PLAYER_NETWORK) )
    doh_event_notify(DOH_EV_DIE, "dohthello.network: *both* sides can't be network players!\n");
  else if (doh_player_type(BL_WHITE) == DOH_PLAYER_NETWORK) /*black is local?*/
    {
      struct sockaddr_in sockinfo;
      int size;
      char buf[strlen(NET_PROT_VERSION)+1];

      sockinfo.sin_family = AF_INET;
      sockinfo.sin_port = htons(port);
      sockinfo.sin_addr.s_addr = htonl(INADDR_ANY);

      if ( (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        doh_event_notify(DOH_EV_PERROR, "dohthello.network: socket");

      if (bind(sock, (struct socketaddr *)&sockinfo, sizeof(sockinfo)) < 0)
        doh_event_notify(DOH_EV_PERROR, "dohthello.network: bind");

      if (listen(sock, 1) < 0)
        doh_event_notify(DOH_EV_PERROR, "dohthello.network: listen");

      gi_message("Waiting for connection from remote player");

      size = sizeof(sockinfo);
      if ( (sock = accept(sock, &sockinfo, &size)) < 0)
        doh_event_notify(DOH_EV_PERROR, "dohthello.network: accept");

      gi_message("Connection established. Waiting for protocol init");
      net_read_data(buf, strlen(NET_PROT_VERSION));
      buf[strlen(NET_PROT_VERSION)] = '\0';

      gi_message("Received protocol init. Sending response.");
      net_write_data(NET_PROT_VERSION, strlen(NET_PROT_VERSION));

      if (strcmp(buf, NET_PROT_VERSION))
        doh_event_notify(DOH_EV_DIE, "dohthello: protocol init error (different versions)\n");

      gi_message("Protocol init successful. Good luck!");

      TRACE("Protocol init successful. Setting socket options\n");

      signal(SIGIO, net_data_ready);
      
      fcntl(sock, F_SETOWN, getpid());
      fcntl(sock, F_SETFL, FNDELAY | FASYNC);


    }
  else if (doh_player_type(BL_BLACK) == DOH_PLAYER_NETWORK) /*white is local?*/
    {
      struct sockaddr_in sockinfo;
      struct hostent *hostentry;
      int timeout;
      char buf[strlen(NET_PROT_VERSION)+1];

      if (!host)
        doh_event_notify(DOH_EV_DIE, "dohthello.network: -host option required for networked black player\n");

      if ( (hostentry = gethostbyname(host)) == NULL)
        doh_event_notify(DOH_EV_DIE, "dohthello.network: nonexistant host\n");

      sockinfo.sin_family = AF_INET;
      sockinfo.sin_port = htons(port);
      sockinfo.sin_addr = (*(struct in_addr *)hostentry->h_addr);

      if ( (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        doh_event_notify(DOH_EV_PERROR, "dohthello.network: socket");

      gi_message("Attempting to connect to remote player");

      timeout = 15;
      while (connect(sock, (struct sockaddr *)&sockinfo, sizeof(sockinfo)) < 0)
        {
          timeout--;
          if (!timeout)
            doh_event_notify(DOH_EV_PERROR, "dohthello.network: connect");
          close(sock);
          sleep(2);
          if ( (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            doh_event_notify(DOH_EV_PERROR, "dohthello.network: socket");
        }

      gi_message("Connection established. Sending protocol init");
      net_write_data(NET_PROT_VERSION, strlen(NET_PROT_VERSION));
      
      gi_message("Sent protocol init. Waiting for response.");

      net_read_data(buf, strlen(NET_PROT_VERSION));
      buf[strlen(NET_PROT_VERSION)] = '\0';

      if (strcmp(buf, NET_PROT_VERSION))
        doh_event_notify(DOH_EV_DIE, "dohthello: protocol init error (different versions)\n");

      gi_message("Protocol init successful. Good luck!");

      TRACE("Protocol init successful. Setting socket options\n");

      signal(SIGIO, net_data_ready);

      fcntl(sock, F_SETOWN, getpid());
      fcntl(sock, F_SETFL, FNDELAY | FASYNC);


    }
  
}

void net_send_event(int event, void *event_data)
{
  char buf[4];

  TRACE("dohthello.net_send_event: called with event %c\n", event);
  
  if (ack_wait)
    doh_event_notify(DOH_EV_DIE, "dohthello.net_send_event: syncronization problem\n");
  
  switch (event)
    {
    case NET_EV_QUIT:
    case NET_EV_NEWGAME:
      ack_wait = 1;
    case NET_EV_ACK:
      *buf = event;
      net_write_data(buf, 1);
      break;
    case NET_EV_MOVE:
      {
        bl_move_t *move = (bl_move_t *)event_data;
        sprintf(buf, "%c%d %d", NET_EV_MOVE, move->row, move->column);
        net_write_data(buf, 4);
        ack_wait = 1;
        break;      
      }
    case NET_EV_CHAT:
      break;
    }

}

static void net_data_ready()
{
  char buf[4];
  int reentry = 0;

  /* Added for SYSV types that reset signal handlers */
  /* Should it be #ifdef'd, or is the overhead harmless on BSD systems? */
  signal(SIGIO, net_data_ready);
  
  if (reentry)
    return;
  else
    reentry++;
  
  if (!net_read_data(buf, 1))
    {
      TRACE("dohthello.net_data_ready: signal received but no data read\n");
    }
  else
    switch (*buf)
      {
      case NET_EV_ACK:
        TRACE("dohthello.net_data_ready: received NET_EV_ACK\n");
        if (ack_wait)
          ack_wait = 0;
        else
          doh_event_notify(DOH_EV_DIE, "dohthello.net_data_ready: syncronization problem\n");
        break;
      case NET_EV_MOVE:
        {
          bl_move_t move;
          
          TRACE("dohthello.net_data_ready: received NET_EV_MOVE\n");
          
          if (!net_read_data(buf, 3))
            doh_event_notify(DOH_EV_DIE, "dohthello.net_data_ready: data expected, none found\n");
          
          sscanf(buf, "%d %d", &move.row, &move.column);
          
          TRACE("dohthello.net_data_ready: network move was (%d,%d), executing\n", move.row, move.column);
          
          doh_event_notify(DOH_EV_MOVE, &move);
          
          TRACE("dohthello.net_data_ready: sending ACK\n");
          net_send_event(NET_EV_ACK, NULL);
          break;
        }

      case NET_EV_QUIT:
        TRACE("dohthello.net_data_ready: received NET_EV_QUIT\n");
        doh_event_notify(DOH_EV_DIE, "dohthello.net_data_ready: remote player has quit the game\n");        
        break;
      case NET_EV_NEWGAME:
        TRACE("dohthello.net_data_ready: received NET_EV_NEWGAME\n");
        doh_event_notify(DOH_EV_NEWGAME, NULL);
        TRACE("dohthello.net_data_ready: sending ACK\n");
        net_send_event(NET_EV_ACK, NULL);
        gi_message("Remote player has started a new game");
        break;
      case NET_EV_CHAT:
        break;
      default:
        TRACE("dohthello.net_data_ready: received unknown message %c (%d)\n", *buf, *buf);
        doh_event_notify(DOH_EV_DIE, "dohthello.net_data_ready: unknown message type\n");
      }
  reentry--;
}

static int net_read_data(char *buf, int len)
{
  int status;

  TRACE("dohthello.net_read_data: trying to read %d bytes\n", len);
  
  if ( (status = read(sock, buf, len)) != len)
    {
      if (status < 0)
        {
          TRACE("dohthello.net_read_data: error returned %d\n", errno);
          if (errno == EWOULDBLOCK)
            {
              TRACE("dohthello.net_read_data: error was EWOULDBLOCK\n");
              return 0;
            }
          else
            {
              TRACE("dohthello.net_read_data: unexpected error, dying\n");
              doh_event_notify(DOH_EV_PERROR, "dohthello.net_read_data");
            }
        }
      else if (status == 0)
        {
          TRACE("dohthello.net_read_data: read succeeded, but nothing read\n");
          return 0;
        }
      else
        {
          TRACE("dohthello.net_read_data: read succeeded, but only read %d bytes\n", status);
          return 0;
        }
    }
  else
    {
      TRACE("dohthello.net_read_data: successfully read %d bytes\n", len);
      return 1;
    }
}

static void net_write_data(char *buf, int len)
{
  int r, timeout=15;
  char *index = buf;

  while (len > 0)
    {
      while ( (r = write(sock, index, len)) < 0 )
        if (!(--timeout))
          doh_event_notify(DOH_EV_PERROR, "dohthello.net_write_data");
          
      index += r;
      len -= r;
    }

}

void net_cleanup()
{
  if (sock)
    shutdown(sock, 2);
}

int net_active()
{
  return (sock != 0);
}

void net_print_usage()
{
  printf("Network options:\n\n");
  printf("            -host               specify remote host\n\n");
  printf("            -port               specify port on remote host\n");
  printf("                                (optional, defaults to 6432)\n\n");
}
