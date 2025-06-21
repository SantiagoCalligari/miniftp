// session.c
#include "session.h"
#include <string.h>
#include <unistd.h>

// Initialize pointer to NULL before session start
ftp_session_t *current_sess = NULL;

static ftp_session_t sess = {
  .control_sock = -1,
  .data_sock = -1,
  .logged_in = 0,
  .current_user = {0},
};

ftp_session_t *session_get(void) {
  return &sess;
}

void session_init(int control_fd) {
  sess.control_sock = control_fd;
  sess.data_sock = -1;
  sess.logged_in = 0;
  memset(sess.current_user, 0, sizeof(sess.current_user));
  memset(&sess.data_addr, 0, sizeof(sess.data_addr));

  current_sess = &sess;
}

void session_cleanup(void) {
  if (sess.control_sock >= 0)
    close(sess.control_sock);
  if (sess.data_sock >= 0)
    close(sess.data_sock);
  sess.control_sock = -1;
  sess.data_sock = -1;
  sess.logged_in = 0;
  memset(sess.current_user, 0, sizeof(sess.current_user));

  current_sess = NULL;
}

