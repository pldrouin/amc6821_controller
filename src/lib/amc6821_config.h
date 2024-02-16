#ifndef _AMC6821_CONFIG_
#define _AMC6821_CONFIG_

#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "args.h"
#include "ht.h"

#include "amc6821_commands.h"

#define CONFIG_EXIT_RET (INT32_MAX)

#define AMC_PBUF_SIZE (1024)

struct amc6821_config
{
  struct ht* ht;
  struct ht* htcs[4];
  args_info a_h;

  char pbuf[AMC_PBUF_SIZE];

  int plength;

  int oldstdout;
  int oldstderr;
  int oout;
  int eout;

  bool interactive;

  struct amc6821_device dev;
};

int amc6821_config(struct amc6821_config* config, int nargs, const char** args);

#endif
