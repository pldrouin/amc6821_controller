#include <stdlib.h>

#include "amc6821.h"

int main(int nargs, char const* args[])
{
  int ret;
  struct amc6821_config config;

  if((ret=amc6821_init(&config.dev))) {
    fprintf(stderr, "Error: amc6821_init (%i)\n", ret);
    exit(3);
  }

  args+=1;
  nargs-=1;

  amc6821_config(&config, nargs, args);

  if((ret=amc6821_close(&config.dev))) {
    fprintf(stderr, "Error: amc6821_close (%i)\n", ret);
    return 1;
  }

  if((ret=amc6821_terminate(&config.dev))) {
    fprintf(stderr, "Error: amc6821_terminate (%i)\n", ret);
    return 1;
  }
  return 0;
}
