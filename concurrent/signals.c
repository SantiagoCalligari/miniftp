// signal.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>   // for pid_t

#include "signals.h"
#include "session.h"    // for current_sess
#include "utils.h"      // for close_fd()

int server_socket = -1;

static void handle_sigint(int sig) {
  (void)sig;
  static volatile sig_atomic_t in_handler = 0;


  if (in_handler) {
    fprintf(stderr, "SIGINT handler reentered!\n");
    return; // Avoid running handler twice concurrently
  }
  in_handler = 1;

  static int sigint_count = 0;
  fprintf(stderr, "SIGINT handler called (count = %d) in PID %d\n", ++sigint_count, getpid());

  printf("[+] SIGINT received. Shutting down...\n");
  fflush(stdout);

  // Close listening socket
  if (server_socket >= 0) {
    close_fd(server_socket,"listen socket");
    server_socket = -1;
  }

  // Block SIGINT while performing shutdown
  sigset_t blockset, oldset;
  sigemptyset(&blockset);
  sigaddset(&blockset, SIGINT);
  if (sigprocmask(SIG_BLOCK, &blockset, &oldset) < 0) {
    perror("sigprocmask");
  }

  // Kill entire process group
  pid_t pgid = getpgrp(); // or getpgid(0)
  if (killpg(pgid, SIGTERM) < 0) {
    perror("killpg");
  }

  // Reap any children
  while (waitpid(-1, NULL, WNOHANG) > 0);

  // Restore previous signal mask (optional here since we're exiting)
  sigprocmask(SIG_SETMASK, &oldset, NULL);


  exit(EXIT_SUCCESS);
}

// --- SIGTERM handler for parent ---
static void handle_sigterm(int sig) {
  (void)sig;

  static volatile sig_atomic_t in_handler = 0;
  if (in_handler) {
    fprintf(stderr, "SIGTERM handler reentered in parent!\n");
    return;
  }
  in_handler = 1;

  fprintf(stderr, "[+] SIGTERM received in parent. Shutting down (PID %d)...\n", getpid());

  // Close listening socket if open
  if (server_socket >= 0) {
    close_fd(server_socket, "listen socket");
    server_socket = -1;
  }

  // Kill all children in process group
  pid_t pgid = getpgrp();
  fprintf(stderr, "[DEBUG] Sending SIGTERM to (GROUP %d)...\n", (int)pgid);
  if (killpg(pgid, SIGTERM) < 0) {
    perror("killpg (parent)");
  }

  // Reap children
  while (waitpid(-1, NULL, WNOHANG) > 0);

  exit(EXIT_SUCCESS);
}

// --- SIGTERM handler for children ---
static void handle_sigterm_child(int sig) {
  (void)sig;

  printf("[*] Child PID %d received SIGTERM, cleaning up...\n", getpid());
  fflush(stdout);

  if (current_sess) {
    if (current_sess->control_sock >= 0) {
      close_fd(current_sess->control_sock, "control socket");
      current_sess->control_sock = -1;
    }
    if (current_sess->data_sock >= 0) {
      close_fd(current_sess->data_sock, "data socket");
      current_sess->data_sock = -1;
    }
  }

  exit(EXIT_SUCCESS);
}

void setup_signals(void) {
  struct sigaction sa;

    // Set parent as group leader
  if (setpgid(0, 0) < 0) {
    perror("setpgid parent");
    exit(EXIT_FAILURE);
  }

  printf("[DEBUG] Setting up signal handlers for parent in PID %d with PGID %d\n", getpid(), getpgrp());

  // Setup SIGINT and SIGTERM for parent

  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGINT);  // Block SIGINT while handler runs

  sa.sa_flags = SA_RESTART;        // Restart interrupted syscalls

  sa.sa_handler = handle_sigint;

  // Handle SIGINT
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction SIGINT");
    exit(EXIT_FAILURE);
  }
  printf("[DEBUG] SIGINT handler installed in PID %d\n", getpid());

  // Handle SIGTERM, same mask and flags, but different handler
  sa.sa_handler = handle_sigterm;

  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    perror("sigaction SIGTERM");
    exit(EXIT_FAILURE);
  }

  // Ignore SIGCHLD to avoid zombie children
  sa.sa_handler = SIG_IGN;
  sigemptyset(&sa.sa_mask); // Clear mask for SIG_IGN
  sa.sa_flags = 0;

  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction SIGCHLD");
    exit(EXIT_FAILURE);
  }
}

void setup_child_signals(void) {
  struct sigaction sa;

  printf("[DEBUG] Setting up signal handlers for child in PID %d\n", getpid());

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_sigterm_child;

  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    perror("sigaction SIGTERM (child)");
    exit(EXIT_FAILURE);
  }
  printf("[DEBUG] SIGTERM handler for child installed in PID %d\n", getpid());

  // Ignore SIGINT in child so SIGTERM handler can work properly
  sa.sa_handler = SIG_IGN;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction SIGINT (child)");
    exit(EXIT_FAILURE);
  }

  // restore default for SIGCHLD
  sa.sa_handler = SIG_DFL;
  sigaction(SIGCHLD, &sa, NULL);
}

// --- Restore all defaults (used by children before exec or clean exit)
void reset_signals(void) {
  struct sigaction sa;

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = SIG_DFL;

  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGCHLD, &sa, NULL);
}
