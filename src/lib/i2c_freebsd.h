#ifndef _AMC6821_I2C_FREEBSD_
#define _AMC6821_I2C_FREEBSD_

#include <unistd.h>
#include <dev/iicbus/iic.h>

#define AI_WRITE (IIC_M_WR)
#define AI_READ (IIC_M_RD)

struct amc6821_iic {
  int fid;
};

#endif
