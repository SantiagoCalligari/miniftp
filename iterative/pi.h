#pragma once

#include <stddef.h>
#include "session.h"

int welcome(ftp_session_t *sess);
int getexe_command(ftp_session_t *sess);

typedef struct {
  const char *name;
  void (*handler)(const char *args);
} ftp_command_t;

void handle_USER(const char *args);
void handle_PASS(const char *args);
void handle_QUIT(const char *args);
void handle_SYST(const char *args);
void handle_TYPE(const char *args);
void handle_PORT(const char *args);
void handle_RETR(const char *args);
void handle_STOR(const char *args);
void handle_NOOP(const char *args);
