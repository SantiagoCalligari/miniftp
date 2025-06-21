#pragma once

#define PWDFILE "/etc/ausftp/ftpusers"

int check_credentials(char *user, char *pass);
