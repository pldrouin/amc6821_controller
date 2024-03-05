#ifndef _AMC6821_SMBUS_LINUX_
#define _AMC6821_SMBUS_LINUX_

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "i2c_common.h"

struct amc6821_smbus
{
  int fid;
  uint8_t addr;
  uint8_t mux_addr;
  uint8_t mux_reg_value;
  uint8_t mux_reg_mask;
};

int _smbus_init(struct amc6821_smbus* smbus);
int _smbus_open(struct amc6821_smbus* smbus, const char* device);
int _smbus_close(struct amc6821_smbus* smbus);
inline static int _smbus_terminate(struct amc6821_smbus* smbus){return AIE_OK;}

int _smbus_raw_send_byte(struct amc6821_smbus* smbus, const uint8_t addr, const uint8_t reg);
int _smbus_raw_recv_byte(struct amc6821_smbus* smbus, const uint8_t addr, uint8_t* value);

int _smbus_send_byte(struct amc6821_smbus* smbus, const uint8_t reg);
int _smbus_recv_byte(struct amc6821_smbus* smbus, uint8_t* value);

int _smbus_write_byte(struct amc6821_smbus* smbus, const uint8_t reg, const uint8_t value);
int _smbus_write_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, const uint8_t* values, const size_t nregs);
int _smbus_read_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, uint8_t* values, const size_t nregs);
inline static int _smbus_read_byte(struct amc6821_smbus* smbus, const uint8_t reg, uint8_t* value){return _smbus_read_bytes(smbus, reg, value, 1);}

#endif
