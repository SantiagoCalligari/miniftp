#pragma once

#define APP_NAME "ausFTP"
#define VERSION "1.0"
#define OS_NAME "Linux"
#define BANNER APP_NAME" version "VERSION" ready"
#define BUG_EMAIL "your.email@example.com"

#define FTP_PORT 21
#define LOCALHOST "127.0.0.1"
#define BUFFER_SIZE 512
#define USERNAME_MAX 64

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define PORT_DOC "Port number (default: " STR(FTP_PORT) ")"
#define ADDR_DOC "Local IP address (default: " LOCALHOST ")"
