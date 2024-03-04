#ifndef _AMC6821_SMBUS_
#define _AMC6821_SMBUS_

#ifdef __FreeBSD__
#include "smbus_freebsd.h"
#else
#error "I2C is currently only supported for FreeBSD"
#endif

#define smbus_init _smbus_init
#define smbus_open _smbus_open
#define smbus_mux_open _smbus_mux_open
#define smbus_close _smbus_close
#define smbus_terminate _smbus_terminate

int smbus_send_byte(struct amc6821_smbus* smbus, const uint8_t reg);
int smbus_recv_byte(struct amc6821_smbus* smbus, uint8_t* value);

int smbus_write_byte(struct amc6821_smbus* smbus, const uint8_t reg, const uint8_t value);
int smbus_write_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, const uint8_t* values, const size_t nregs);
int smbus_read_byte(struct amc6821_smbus* smbus, const uint8_t reg, uint8_t* value);
int smbus_read_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, uint8_t* values, const size_t nregs);

#endif
