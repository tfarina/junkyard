#include <stdio.h>

#include "futils.h"

int main(int argc, char **argv) {
  const char *username;

  username = f_get_user_name();

  printf("%s\n", username);

  return 0;
}
