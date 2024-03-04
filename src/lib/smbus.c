#include "smbus.h"

//#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG_PRINT(...)

int smbus_send_byte(struct amc6821_smbus* smbus, const uint8_t reg)
{
  return _smbus_send_byte(smbus, reg);
}

int smbus_recv_byte(struct amc6821_smbus* smbus, uint8_t* value)
{
  return _smbus_recv_byte(smbus, value);
}

int smbus_write_byte(struct amc6821_smbus* smbus, const uint8_t reg, const uint8_t value)
{
  return _smbus_write_byte(smbus, reg, value);
}

int smbus_write_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, const uint8_t* values, const size_t nregs)
{
  return _smbus_write_bytes(smbus, first_reg, values, nregs);
}

int smbus_read_byte(struct amc6821_smbus* smbus, const uint8_t reg, uint8_t* value)
{
  return _smbus_read_byte(smbus, reg, value);
}

int smbus_read_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, uint8_t* values, const size_t nregs)
{
  return _smbus_read_bytes(smbus, first_reg, values, nregs);
}
