#ifndef _AMC6821_SMBUS_
#define _AMC6821_SMBUS_

#ifdef __FreeBSD__
#include "smbus_freebsd.h"
#elif __linux__
#include "smbus_linux.h"
#else
#error "I2C is not currently supported this operating system"
#endif

#define smbus_close _smbus_close
#define smbus_terminate _smbus_terminate

int smbus_init(struct amc6821_smbus* smbus);
int smbus_open(struct amc6821_smbus* smbus, const char* device, const uint8_t dev_addr);
int smbus_mux_open(struct amc6821_smbus* smbus, const char* device, const uint8_t dev_addr, const uint8_t mux_addr, const uint8_t mux_reg_value, const uint8_t mux_reg_mask);

int smbus_send_byte(struct amc6821_smbus* smbus, const uint8_t reg);
int smbus_recv_byte(struct amc6821_smbus* smbus, uint8_t* value);


int smbus_write_byte(struct amc6821_smbus* smbus, const uint8_t reg, const uint8_t value);
int smbus_write_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, const uint8_t* values, const size_t nregs);
int smbus_read_byte(struct amc6821_smbus* smbus, const uint8_t reg, uint8_t* value);
int smbus_read_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, uint8_t* values, const size_t nregs);

#endif
