#ifndef _AMC6821_I2C_
#define _AMC6821_I2C_

#include<stdint.h>
#include<inttypes.h>
#include<stdbool.h>
#include<stddef.h>
#include<stdio.h>

#ifdef __FreeBSD__
#include "i2c_freebsd.h"
#else
#error "I2C is currently only supported for FreeBSD"
#endif

typedef bool ai_rw;

#define AIE_OK 0
#define AIE_INVALID_VALUE -1
#define AIE_OPEN_FAILED -2
#define AIE_CLOSE_FAILED -3
#define AIE_INVALID_ADDR -4
#define AIE_START_FAILED -5
#define AIE_REPEAT_START_FAILED -6
#define AIE_STOP_FAILED -7
#define AIE_RESET_FAILED -8
#define AIE_WRITE_FAILED -9
#define AIE_WRITE_MANY_FAILED -10
#define AIE_READ_FAILED -11
#define AIE_READ_MANY_FAILED -12

#define AIE_CHECK_ADDR(ADDR) ({if(ADDR&(1<<7)) {fprintf(stderr, "%s: Error: Invalid I2C address (%" PRIu8 ")\n", __func__, ADDR); return AIE_INVALID_ADDR;}})

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
