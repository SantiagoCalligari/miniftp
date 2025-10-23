#include "server.h"
#include "pi.h"
#include "responses.h"
#include "session.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

extern int server_socket;

int server_init(const char *ip, int port) {
  struct sockaddr_in server_addr;

  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    fprintf(stderr, "Error creating socket: ");
    perror(NULL);
    return -1;
  }

  // avoid problem with reuse inmeditely after force quiting
  const int opt = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    fprintf(stderr, "Error setting SO_REUSEADDR: ");
    perror(NULL);
    close(listen_fd);
    return -1;
  }

#ifdef SO_REUSEPORT
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    fprintf(stderr, "Error setting SO_REUSEPORT: ");
    perror(NULL);
    close(listen_fd);
    return -1;
  }
#endif

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
    fprintf(stderr, "Invalid IP address: %s\n", ip);
    close(listen_fd);
    return -1;
  }

  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    fprintf(stderr, "Bind failed: ");
    perror(NULL);
    close(listen_fd);
    return -1;
  }

  char ip_buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &server_addr.sin_addr, ip_buf, sizeof(ip_buf));
  printf("Listening on %s:%d\n", ip_buf, port);

  if (listen(listen_fd, SOMAXCONN) < 0) {
    fprintf(stderr, "Listen failed: ");
    perror(NULL);
    close(listen_fd);
    return -1;
  }

  server_socket = listen_fd;
  return listen_fd;
}

int server_accept(int listen_fd, struct sockaddr_in *client_addr) {

  // client_addr can be NULL if caller doesn't need client info
  socklen_t addrlen = sizeof(*client_addr);
  int new_socket = accept(listen_fd, (struct sockaddr *)client_addr, &addrlen);

  // EINTR for avoid errors by signal reentry
  // https://stackoverflow.com/questions/41474299/checking-if-errno-eintr-what-does-it-mean
  if (new_socket < 0 && errno != EINTR) {
    fprintf(stderr, "Accept failed: ");
    perror(NULL);
    return -1;
  }

  return new_socket;
}

void server_loop(int listen_fd) {
  struct pollfd *fds = malloc(sizeof(struct pollfd) * MAX_CLIENTS);
  ftp_session_t *sessions = calloc(MAX_CLIENTS, sizeof(ftp_session_t));
  int nfds = 1;

  if (!fds || !sessions) {
    fprintf(stderr, "Memory allocation failed\n");
    return;
  }

  for (int i = 0; i < MAX_CLIENTS; i++) {
    sessions[i].control_sock = -1;
    sessions[i].data_sock = -1;
  }

  fds[0].fd = listen_fd;
  fds[0].events = POLLIN;

  while (1) {
    int poll_count = poll(fds, nfds, -1);

    if (poll_count < 0) {
      if (errno == EINTR)
        continue;
      perror("poll");
      break;
    }

    if (fds[0].revents & POLLIN) {
      struct sockaddr_in client_addr;
      int new_socket = server_accept(listen_fd, &client_addr);

      if (new_socket >= 0) {
        if (nfds < MAX_CLIENTS) {
          char client_ip[INET_ADDRSTRLEN];
          inet_ntop(AF_INET, &client_addr.sin_addr, client_ip,
                    sizeof(client_ip));
          printf("Connection from %s:%d accepted (fd=%d, index=%d)\n",
                 client_ip, ntohs(client_addr.sin_port), new_socket, nfds);

          ftp_session_t *sess = &sessions[nfds];
          sess->control_sock = new_socket;
          sess->data_sock = -1;
          sess->logged_in = 0;
          memset(sess->current_user, 0, sizeof(sess->current_user));
          memset(&sess->data_addr, 0, sizeof(sess->data_addr));

          fds[nfds].fd = new_socket;
          fds[nfds].events = POLLIN;

          current_sess = sess;

          printf("[DEBUG] About to send welcome to fd=%d\n",
                 sess->control_sock);

          ssize_t sent = dprintf(new_socket, MSG_220);
          if (sent < 0) {
            perror("Welcome send failed");
            printf("Failed to send welcome to fd=%d, closing connection\n",
                   new_socket);
            close(new_socket);
            sess->control_sock = -1;
          } else {
            printf("[DEBUG] Welcome sent successfully (%zd bytes) to fd=%d\n",
                   sent, new_socket);
            nfds++;
          }
        } else {
          fprintf(stderr, "Max clients reached, rejecting connection\n");
          close(new_socket);
        }
      }
    }

    for (int i = 1; i < nfds; i++) {
      if (fds[i].revents & (POLLIN | POLLERR | POLLHUP)) {
        ftp_session_t *sess = &sessions[i];
        current_sess = sess;

        if (fds[i].revents & POLLIN) {
          if (getexe_command(sess) < 0) {
            printf("Connection closed for fd %d (index %d)\n", fds[i].fd, i);

            if (sess->control_sock >= 0)
              close(sess->control_sock);
            if (sess->data_sock >= 0)
              close(sess->data_sock);

            sess->control_sock = -1;
            sess->data_sock = -1;

            // Remove from poll array by shifting
            for (int j = i; j < nfds - 1; j++) {
              fds[j] = fds[j + 1];
              sessions[j] = sessions[j + 1];
            }
            nfds--;
            i--;
          }
        } else {
          printf("Connection error/hangup for fd %d (index %d)\n", fds[i].fd,
                 i);

          if (sess->control_sock >= 0)
            close(sess->control_sock);
          if (sess->data_sock >= 0)
            close(sess->data_sock);

          sess->control_sock = -1;
          sess->data_sock = -1;

          for (int j = i; j < nfds - 1; j++) {
            fds[j] = fds[j + 1];
            sessions[j] = sessions[j + 1];
          }
          nfds--;
          i--;
        }
      }
    }
  }

  // Cleanup
  for (int i = 1; i < nfds; i++) {
    if (sessions[i].control_sock >= 0)
      close(sessions[i].control_sock);
    if (sessions[i].data_sock >= 0)
      close(sessions[i].data_sock);
  }

  free(fds);
  free(sessions);
#undef MAX_CLIENTS
}
