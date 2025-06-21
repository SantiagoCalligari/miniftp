#include "server.h"
#include "pi.h"
#include "responses.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <strings.h>

static ftp_command_t ftp_commands[] = {
  { "USER", handle_USER },
  { "PASS", handle_PASS },
  { "QUIT", handle_QUIT },
  { "SYST", handle_SYST },
  { "TYPE", handle_TYPE },
  { "PORT", handle_PORT },
  { "RETR", handle_RETR },
  { "STOR", handle_STOR },
  { "NOOP", handle_NOOP },
  { NULL, NULL }
};

int welcome(ftp_session_t *sess) {

  // Send initial FTP welcome message
  if (safe_dprintf(sess->control_sock, MSG_220) != sizeof(MSG_220) - 1) {
    fprintf(stderr, "Send error\n");
    close_fd(sess->control_sock, "cliente socket");
    return -1;
  }

  return 0;
}

int getexe_command(ftp_session_t *sess) {
  char buffer[BUFFER_SIZE];

  // Receive string from CC
  ssize_t len = recv(sess->control_sock, buffer, sizeof(buffer) - 1, 0);
  if (len < 0) {
    perror("Receive fail: ");
    close_fd(sess->control_sock, "cliente socket");
    return -1;
  }

  // The connection was closed improperly and we close it
  if (len == 0) {
    sess->current_user[0] = '\0'; // Close session
    close_fd(sess->control_sock, "client socket"); // Close socket
    sess->control_sock = -1;
    return -1;
  }

  buffer[len] = '\0';

  // Strip CRLF
  char *cr = strchr(buffer, '\r');
  if (cr) *cr = '\0';
  char *lf = strchr(buffer, '\n');
  if (lf) *lf = '\0';

  // Separate command and argument
  char *arg = NULL;
  char *cmd = buffer;

  // Case null command
  if (cmd[0] == '\0') {
    safe_dprintf(sess->control_sock, "500 Empty command.\r\n");
    return 0;
  }

  char *space = strchr(buffer, ' ');
  if (space) {
    *space = '\0';
    arg = space + 1;
    while (*arg == ' ') arg++;
  }

  ftp_command_t *entry = ftp_commands;
  while (entry->name) {
    if (strcasecmp(entry->name, cmd) == 0) {
      entry->handler(arg ? arg : "");
      return (sess->control_sock < 0) ? -1 : 0;
    }
    entry++;
  }

  safe_dprintf(sess->control_sock, "502 Command not implemented.\r\n");
  return 0;
}
