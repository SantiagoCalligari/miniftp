#pragma once

#include "config.h"

// https://www.rfc-editor.org/rfc/rfc959.html
#define MSG_110 "110 Restart marker replay.\r\n"
#define MSG_120 "120 Service ready in %d minutes.\r\n"
#define MSG_125 "125 Data connection already open; transfer starting.\r\n"
#define MSG_150 "150 File status okay; about to open data connection.\r\n"
#define MSG_200 "200 Command okay.\r\n"
#define MSG_202 "202 Command not implemented, superfluous at this site.\r\n"
#define MSG_211 "211 System status, or system help reply.\r\n"
#define MSG_212 "212 Directory status.\r\n"
#define MSG_213 "213 File status.\r\n"
#define MSG_214 "214 Help message.\r\n"
#define MSG_215 "215 "OS_NAME" system type.\r\n"
#define MSG_220 "220 "BANNER".\r\n"
#define MSG_221 "221 Goodbye.\r\n"
#define MSG_225 "225 Data connection open; no transfer in progress.\r\n"
#define MSG_226 "226 Transfer complete.\r\n"
#define MSG_227 "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).\r\n"
#define MSG_230 "230 User logged in, proceed.\r\n"
#define MSG_250 "250 Requested file action okay, completed.\r\n"
#define MSG_257 "257 %s created.\r\n"
#define MSG_331 "331 User name okay, need password.\r\n"
#define MSG_332 "332 Need account for login.\r\n"
#define MSG_350 "350 Requested file action pending further information.\r\n"
#define MSG_421 "421 Service not available, closing control connection.\r\n"
#define MSG_425 "425 Can’t open data connection.\r\n"
#define MSG_426 "426 Connection closed; transfer aborted.\r\n"
#define MSG_450 "450 Requested file action not taken.\r\n"
#define MSG_451 "451 Requested action aborted: local error in processing.\r\n"
#define MSG_452 "452 Requested action not taken; insufficient storage space.\r\n"
#define MSG_500 "500 Syntax error, command unrecognized.\r\n"
#define MSG_501 "501 Syntax error in parameters or argument.\r\n"
#define MSG_502 "502 Command not implemented.\r\n"
#define MSG_503 "503 Bad sequence of commands.\r\n"
#define MSG_504 "504 Command not implemented for that parameter.\r\n"
#define MSG_530 "530 Not logged in.\r\n"
#define MSG_532 "532 Need account for storing files.\r\n"
#define MSG_550 "550 Requested action not taken; file unavailable %s.\r\n" // %s reasons
#define MSG_551 "551 Requested action aborted: page type unknown.\r\n"
#define MSG_552 "552 Requested file action aborted; Exceeded storage allocation.\r\n"
#define MSG_553 "553 Requested action not taken; File name not allowed.\r\n"

// https://www.rfc-editor.org/rfc/rfc2228
#define MSG_232 "232 User logged in, authorized by security data exchange.\r\n"
#define MSG_234 "234 AUTH command OK. Initializing SSL connection.\r\n"
#define MSG_334 "334 ADAT=base64string.\r\n" // (ADAT=base64string is optional)
#define MSG_335 "335 ADAT=base64string.\r\n" // (ADAT=base64string is optional)
#define MSG_336 "336 Username okay, need password. Challenge is %s.\r\n"
#define MSG_431 "431 Need some unavailable resource to process security.\r\n"
#define MSG_534 "534 Request denied for policy reasons.\r\n"
#define MSG_535 "535 Failed security check.\r\n"
#define MSG_533 "533 Command protection level denied for policy reasons.\r\n"
#define MSG_536 "536 Requested PROT level not supported by mechanism.\r\n"
#define MSG_537 "537 Command protection level not supported by security mechanism.\r\n"

// 401, 253 responses are not standards ???
