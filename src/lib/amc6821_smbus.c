#include "amc6821_smbus.h"

//#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG_PRINT(...)

int _amc6821_smbus_mux_start(struct amc6821_smbus* smbus, const ai_rw rw)
{
  int ret;

  if(smbus->mux_reg_mask) {
    uint8_t mux_reg_value;

    for(;;) {

      if((ret=i2c_start(&smbus->iic, smbus->mux_addr, AI_READ))) goto iic_failure;

      if((ret=i2c_read(&smbus->iic, &mux_reg_value, true))) goto iic_failure;

      DEBUG_PRINT("%s: Returned masked MUX register value is 0x%02X\n", __func__, mux_reg_value&smbus->mux_reg_mask);

      if((ret=i2c_stop(&smbus->iic))) goto iic_failure;

      if((mux_reg_value&smbus->mux_reg_mask) == smbus->mux_reg_value) break;

      if((ret=i2c_start(&smbus->iic, smbus->mux_addr, AI_WRITE))) goto iic_failure;

      if((ret=i2c_write(&smbus->iic, smbus->mux_reg_value, true))) goto iic_failure;

      if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
    }
  }
  if((ret=i2c_start(&smbus->iic, smbus->addr, rw))) goto iic_failure;

  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int amc6821_smbus_init(struct amc6821_smbus* smbus)
{
  smbus->mux_reg_mask=0;
  return i2c_init(&smbus->iic);
}

int amc6821_smbus_open(struct amc6821_smbus* smbus, const char* device, const uint8_t dev_addr)
{
  int ret;
  
  if((ret=i2c_open(&smbus->iic, device))) return ret;

  if((ret=i2c_reset(&smbus->iic))) return ret;

  return AIE_OK;
}

int amc6821_smbus_mux_open(struct amc6821_smbus* smbus, const char* device, const uint8_t dev_addr, const uint8_t mux_addr, const uint8_t mux_reg_value, const uint8_t mux_reg_mask)
{
  smbus->addr=dev_addr;
  smbus->mux_addr=mux_addr;
  smbus->mux_reg_value=mux_reg_value;
  smbus->mux_reg_mask=mux_reg_mask;

  return amc6821_smbus_open(smbus, device, dev_addr);
}

int amc6821_smbus_send_byte(struct amc6821_smbus* smbus, const uint8_t reg)
{
  int ret;
  
  if((ret=_amc6821_smbus_mux_start(smbus, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, reg, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int amc6821_smbus_recv_byte(struct amc6821_smbus* smbus, uint8_t* value)
{
  int ret;
  
  if((ret=_amc6821_smbus_mux_start(smbus, AI_READ))) goto iic_failure;

  if((ret=i2c_read(&smbus->iic, value, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int amc6821_smbus_write_byte(struct amc6821_smbus* smbus, const uint8_t reg, const uint8_t value)
{
  int ret;
  
  if((ret=_amc6821_smbus_mux_start(smbus, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, reg, false))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, value, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_stop(&smbus->iic);
  return ret;
}

int amc6821_smbus_write_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, const uint8_t* values, const size_t nregs)
{
  int ret;
  
  if((ret=_amc6821_smbus_mux_start(smbus, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, first_reg, false))) goto iic_failure;

  if((ret=i2c_write_many(&smbus->iic, values, nregs, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int amc6821_smbus_read_byte(struct amc6821_smbus* smbus, const uint8_t reg, uint8_t* value)
{
  int ret;
  
  if((ret=_amc6821_smbus_mux_start(smbus, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, reg, false))) goto iic_failure;

  if((ret=i2c_repeated_start(&smbus->iic, smbus->addr, AI_READ))) goto iic_failure;

  if((ret=i2c_read(&smbus->iic, value, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}

int amc6821_smbus_read_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, uint8_t* values, const size_t nregs)
{
  int ret;
  
  if((ret=_amc6821_smbus_mux_start(smbus, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&smbus->iic, first_reg, false))) goto iic_failure;

  if((ret=i2c_repeated_start(&smbus->iic, smbus->addr, AI_READ))) goto iic_failure;

  if((ret=i2c_read_many(&smbus->iic, values, nregs, true))) goto iic_failure;

  if((ret=i2c_stop(&smbus->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&smbus->iic);
  return ret;
}
