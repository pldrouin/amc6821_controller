#ifndef _AMC6821_AMC6821_I2C_
#define _AMC6821_AMC6821_I2C_

#include "i2c.h"

#define AMC6821_ID_REG (0x3D)
#define AMC6821_DEVICE_ID (0x21)

struct amc6821_device_iic
{
  struct amc6821_iic iic;
  uint8_t addr;
  uint8_t mux_addr;
  uint8_t mux_reg_value;
  uint8_t mux_reg_mask;
};

int amc6821_i2c_init(struct amc6821_device_iic* dev_iic);
int amc6821_i2c_open(struct amc6821_device_iic* dev_iic, const char* device, const uint8_t dev_addr, const uint8_t mux_addr, const uint8_t mux_reg_value, const uint8_t mux_reg_mask);
static inline int amc6821_i2c_close(struct amc6821_device_iic* dev_iic){return i2c_close(&dev_iic->iic);}
static inline int amc6821_i2c_terminate(struct amc6821_device_iic* dev_iic){return i2c_init(&dev_iic->iic);}

int amc6821_i2c_send_byte(struct amc6821_device_iic* dev_iic, const uint8_t reg);
int amc6821_i2c_recv_byte(struct amc6821_device_iic* dev_iic, uint8_t* value);

int amc6821_i2c_write_byte(struct amc6821_device_iic* dev_iic, const uint8_t reg, const uint8_t value);
int amc6821_i2c_write_bytes(struct amc6821_device_iic* dev_iic, const uint8_t first_reg, const uint8_t* values, const size_t nregs);
int amc6821_i2c_read_byte(struct amc6821_device_iic* dev_iic, const uint8_t reg, uint8_t* value);
int amc6821_i2c_read_bytes(struct amc6821_device_iic* dev_iic, const uint8_t first_reg, uint8_t* values, const size_t nregs);

#endif
