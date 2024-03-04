#ifndef _AMC6821_SMBUS_FREEBSD_
#define _AMC6821_SMBUS_FREEBSD_

#include <stdint.h>

#include "i2c.h"

struct amc6821_smbus
{
  struct amc6821_iic iic;
  uint8_t addr;
  uint8_t mux_addr;
  uint8_t mux_reg_value;
  uint8_t mux_reg_mask;
};

int _smbus_init(struct amc6821_smbus* smbus);
int _smbus_open(struct amc6821_smbus* smbus, const char* device, const uint8_t dev_addr);
int _smbus_mux_open(struct amc6821_smbus* smbus, const char* device, const uint8_t dev_addr, const uint8_t mux_addr, const uint8_t mux_reg_value, const uint8_t mux_reg_mask);
static inline int _smbus_close(struct amc6821_smbus* smbus){return i2c_close(&smbus->iic);}
static inline int _smbus_terminate(struct amc6821_smbus* smbus){return i2c_terminate(&smbus->iic);}

int _smbus_send_byte(struct amc6821_smbus* smbus, const uint8_t reg);
int _smbus_recv_byte(struct amc6821_smbus* smbus, uint8_t* value);

int _smbus_write_byte(struct amc6821_smbus* smbus, const uint8_t reg, const uint8_t value);
int _smbus_write_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, const uint8_t* values, const size_t nregs);
int _smbus_read_byte(struct amc6821_smbus* smbus, const uint8_t reg, uint8_t* value);
int _smbus_read_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, uint8_t* values, const size_t nregs);

#endif
