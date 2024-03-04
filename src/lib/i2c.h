#ifndef _AMC6821_I2C_
#define _AMC6821_I2C_

#include "i2c_common.h"

#ifdef __FreeBSD__
#include "i2c_freebsd.h"
#else
#error "I2C is currently only supported for FreeBSD"
#endif

typedef bool ai_rw;

int i2c_init(struct amc6821_iic* iic);
int i2c_open(struct amc6821_iic* iic, const char* device);
int i2c_close(struct amc6821_iic* iic);
int i2c_terminate(struct amc6821_iic* iic);

int i2c_start(struct amc6821_iic* iic, const uint8_t slave_addr, const ai_rw rw);
int i2c_repeated_start(struct amc6821_iic* iic, const uint8_t slave_addr, const ai_rw rw);
int i2c_stop(struct amc6821_iic* iic);
int i2c_reset(struct amc6821_iic* iic);
int i2c_write(struct amc6821_iic* iic, const uint8_t byte, const bool last);
int i2c_write_many(struct amc6821_iic* iic, uint8_t const* bytes, const size_t nbytes, const bool last);
int i2c_read(struct amc6821_iic* iic, uint8_t* byte, const bool last);
int i2c_read_many(struct amc6821_iic* iic, uint8_t* bytes, const size_t nbytes, const bool last);

#endif
