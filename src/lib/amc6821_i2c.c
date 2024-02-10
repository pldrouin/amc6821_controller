#include "amc6821_i2c.h"

#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
//#define DEBUG_PRINT(...)

int _amc6821_i2c_mux_start(struct amc6821_device_iic* dev_iic, const ai_rw rw)
{
  int ret;

  if(dev_iic->mux_reg_mask) {
    uint8_t mux_reg_value;

    for(;;) {

      if((ret=i2c_start(&dev_iic->iic, dev_iic->mux_addr, AI_READ))) goto iic_failure;

      if((ret=i2c_read(&dev_iic->iic, &mux_reg_value, true))) goto iic_failure;

      DEBUG_PRINT("%s: Returned masked MUX register value is 0x%02X\n", __func__, mux_reg_value&dev_iic->mux_reg_mask);

      if((ret=i2c_stop(&dev_iic->iic))) goto iic_failure;

      if((mux_reg_value&dev_iic->mux_reg_mask) == dev_iic->mux_reg_value) break;

      if((ret=i2c_start(&dev_iic->iic, dev_iic->mux_addr, AI_WRITE))) goto iic_failure;

      if((ret=i2c_write(&dev_iic->iic, dev_iic->mux_reg_value, true))) goto iic_failure;

      if((ret=i2c_stop(&dev_iic->iic))) goto iic_failure;
    }
  }
  if((ret=i2c_start(&dev_iic->iic, dev_iic->addr, rw))) goto iic_failure;

  return AIE_OK;

iic_failure:
  i2c_reset(&dev_iic->iic);
  return ret;
}

int amc6821_i2c_init(struct amc6821_device_iic* dev_iic)
{
  dev_iic->mux_reg_mask=0;
  return i2c_init(&dev_iic->iic);
}

int amc6821_i2c_open(struct amc6821_device_iic* dev_iic, const char* device, const uint8_t dev_addr, const uint8_t mux_addr, const uint8_t mux_reg_value, const uint8_t mux_reg_mask)
{
  dev_iic->addr=dev_addr;
  dev_iic->mux_addr=mux_addr;
  dev_iic->mux_reg_value=mux_reg_value;
  dev_iic->mux_reg_mask=mux_reg_mask;

  int ret;
  
  if((ret=i2c_open(&dev_iic->iic, device))) return ret;

  if((ret=i2c_reset(&dev_iic->iic))) return ret;

  uint8_t id;

  if((ret=amc6821_i2c_read_byte(dev_iic, AMC6821_ID_REG, &id))) return ret;

  if(id != AMC6821_DEVICE_ID) {
    fprintf(stderr,"%s: Error; Device ID (%" PRIu8 ") is invalid!\n", __func__, id);
    i2c_close(&dev_iic->iic);
    return AIE_INVALID_VALUE;
  }

  return AIE_OK;
}

int amc6821_i2c_send_byte(struct amc6821_device_iic* dev_iic, const uint8_t reg)
{
  int ret;
  
  if((ret=_amc6821_i2c_mux_start(dev_iic, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&dev_iic->iic, reg, true))) goto iic_failure;

  if((ret=i2c_stop(&dev_iic->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&dev_iic->iic);
  return ret;
}

int amc6821_i2c_recv_byte(struct amc6821_device_iic* dev_iic, uint8_t* value)
{
  int ret;
  
  if((ret=_amc6821_i2c_mux_start(dev_iic, AI_READ))) goto iic_failure;

  if((ret=i2c_read(&dev_iic->iic, value, true))) goto iic_failure;

  if((ret=i2c_stop(&dev_iic->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&dev_iic->iic);
  return ret;
}

int amc6821_i2c_write_byte(struct amc6821_device_iic* dev_iic, const uint8_t reg, const uint8_t value)
{
  int ret;
  
  if((ret=_amc6821_i2c_mux_start(dev_iic, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&dev_iic->iic, reg, false))) goto iic_failure;

  if((ret=i2c_write(&dev_iic->iic, value, true))) goto iic_failure;

  if((ret=i2c_stop(&dev_iic->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_stop(&dev_iic->iic);
  return ret;
}

int amc6821_i2c_write_bytes(struct amc6821_device_iic* dev_iic, const uint8_t first_reg, const uint8_t* values, const size_t nregs)
{
  int ret;
  int i;
  
  if((ret=_amc6821_i2c_mux_start(dev_iic, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&dev_iic->iic, first_reg, false))) goto iic_failure;

  for(i=0; i<nregs; ++i) if((ret=i2c_write(&dev_iic->iic, values[i], true))) goto iic_failure;

  if((ret=i2c_stop(&dev_iic->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&dev_iic->iic);
  return ret;
}

int amc6821_i2c_read_byte(struct amc6821_device_iic* dev_iic, const uint8_t reg, uint8_t* value)
{
  int ret;
  
  if((ret=_amc6821_i2c_mux_start(dev_iic, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&dev_iic->iic, reg, false))) goto iic_failure;

  if((ret=i2c_repeated_start(&dev_iic->iic, dev_iic->addr, AI_READ))) goto iic_failure;

  if((ret=i2c_read(&dev_iic->iic, value, true))) goto iic_failure;

  if((ret=i2c_stop(&dev_iic->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&dev_iic->iic);
  return ret;
}

int amc6821_i2c_read_bytes(struct amc6821_device_iic* dev_iic, const uint8_t first_reg, uint8_t* values, const size_t nregs)
{
  int ret;
  int i;
  
  if((ret=_amc6821_i2c_mux_start(dev_iic, AI_WRITE))) goto iic_failure;

  if((ret=i2c_write(&dev_iic->iic, first_reg, false))) goto iic_failure;

  if((ret=i2c_repeated_start(&dev_iic->iic, dev_iic->addr, AI_READ))) goto iic_failure;

  for(i=0; i<nregs-1; ++i) if((ret=i2c_read(&dev_iic->iic, values+i, false))) goto iic_failure;

  if((ret=i2c_read(&dev_iic->iic, values+nregs-1, true))) goto iic_failure;

  if((ret=i2c_stop(&dev_iic->iic))) goto iic_failure;
  return AIE_OK;

iic_failure:
  i2c_reset(&dev_iic->iic);
  return ret;
}
