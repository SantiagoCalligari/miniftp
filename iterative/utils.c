#define _POSIX_C_SOURCE 200809L
#include "server.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void close_fd(int fd, const char *label) {

  if (close(fd) < 0) {
    fprintf(stderr, "Error closing %s: ", label);
    perror(NULL);
  }
}

ssize_t safe_dprintf(int fd, const char *format, ...) {
  va_list args;
  va_start(args, format);
  ssize_t ret = vdprintf(fd, format, args);
  va_end(args);

  if (ret < 0) {
    perror("dprintf error: ");
  }
  return ret;
}
