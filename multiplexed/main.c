#include "arguments.h"
#include "server.h"
#include "signals.h"
#include "utils.h"
#include <arpa/inet.h> // for inet_ntoa()
#include <stdio.h>
#include <stdlib.h> // EXIT_*
#include <unistd.h> // for close()

int main(int argc, char **argv) {
  struct arguments args;

  if (parse_arguments(argc, argv, &args) != 0)
    return EXIT_FAILURE;

  printf("Starting server on %s:%d\n", args.address, args.port);

  int listen_fd = server_init(args.address, args.port);
  if (listen_fd < 0)
    return EXIT_FAILURE;

  setup_signals();

  server_loop(listen_fd);

  close_fd(listen_fd, "listening socket");

  // https://en.cppreference.com/w/c/program/EXIT_status
  return EXIT_SUCCESS;
}
