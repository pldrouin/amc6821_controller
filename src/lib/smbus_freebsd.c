#ifdef __FreeBSD__
#include "smbus_freebsd.h"

//#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG_PRINT(...)

int _smbus_init(struct amc6821_smbus* smbus)
{
  return i2c_init(&smbus->iic);
}

int _smbus_open(struct amc6821_smbus* smbus, const char* device)
{
  int ret;
  
  if((ret=i2c_open(&smbus->iic, device))) return ret;

  if((ret=i2c_reset(&smbus->iic))) return ret;

  return AIE_OK;
}

int _smbus_raw_send_byte(struct amc6821_smbus* smbus, const uint8_t addr, const uint8_t reg)
{
  int ret;
  
  if((ret=i2c_start(&smbus->iic, addr, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, reg, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int _smbus_raw_recv_byte(struct amc6821_smbus* smbus, const uint8_t addr, uint8_t* value)
{
  int ret;
  
  if((ret=i2c_start(&smbus->iic, addr, AI_READ))) goto iic_failure;

  if((ret=i2c_read(&smbus->iic, value, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int _smbus_send_byte(struct amc6821_smbus* smbus, const uint8_t reg)
{
  int ret;

  if((ret=_smbus_raw_send_byte(smbus, smbus->addr, reg))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int _smbus_recv_byte(struct amc6821_smbus* smbus, uint8_t* value)
{
  int ret;
  
  if((ret=_smbus_raw_recv_byte(smbus, smbus->addr, value))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int _smbus_write_byte(struct amc6821_smbus* smbus, const uint8_t reg, const uint8_t value)
{
  int ret;
  
  if((ret=i2c_start(&smbus->iic, smbus->addr, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, reg, false))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, value, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_stop(&smbus->iic);
  return ret;
}

int _smbus_write_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, const uint8_t* values, const size_t nregs)
{
  int ret;
  
  if((ret=i2c_start(&smbus->iic, smbus->addr, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, first_reg, false))) goto iic_failure;

  if((ret=i2c_write_many(&smbus->iic, values, nregs, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int _smbus_read_byte(struct amc6821_smbus* smbus, const uint8_t reg, uint8_t* value)
{
  int ret;
  
  if((ret=i2c_start(&smbus->iic, smbus->addr, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, reg, false))) goto iic_failure;

  if((ret=i2c_repeated_start(&smbus->iic, smbus->addr, AI_READ))) goto iic_failure;

  if((ret=i2c_read(&smbus->iic, value, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int _smbus_read_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, uint8_t* values, const size_t nregs)
{
  int ret;
  
  if((ret=i2c_start(&smbus->iic, smbus->addr, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, first_reg, false))) goto iic_failure;

  if((ret=i2c_repeated_start(&smbus->iic, smbus->addr, AI_READ))) goto iic_failure;

  if((ret=i2c_read_many(&smbus->iic, values, nregs, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}
#endif
