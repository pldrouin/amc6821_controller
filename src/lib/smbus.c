#include "smbus.h"

//#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG_PRINT(...)

int smbus_init(struct amc6821_smbus* smbus)
{
  smbus->mux_reg_mask=0;
  return _smbus_init(smbus);
}

int smbus_open(struct amc6821_smbus* smbus, const char* device, const uint8_t dev_addr)
{
  smbus->addr=dev_addr;

  return _smbus_open(smbus, device);
}

int smbus_mux_open(struct amc6821_smbus* smbus, const char* device, const uint8_t dev_addr, const uint8_t mux_addr, const uint8_t mux_reg_value, const uint8_t mux_reg_mask)
{
  smbus->addr=dev_addr;
  smbus->mux_addr=mux_addr;
  smbus->mux_reg_value=mux_reg_value;
  smbus->mux_reg_mask=mux_reg_mask;

  return _smbus_open(smbus, device);
}

int _smbus_mux_select(struct amc6821_smbus* smbus)
{
  int ret;

  if(smbus->mux_reg_mask) {
    uint8_t mux_reg_value;

    for(;;) {

      if((ret=_smbus_raw_recv_byte(smbus, smbus->mux_addr, &mux_reg_value))) return ret;

      DEBUG_PRINT("%s: Returned masked MUX register value is 0x%02X\n", __func__, mux_reg_value&smbus->mux_reg_mask);

      if((mux_reg_value&smbus->mux_reg_mask) == smbus->mux_reg_value) break;

      if((ret=_smbus_raw_send_byte(smbus, smbus->mux_addr, smbus->mux_reg_value))) return ret;
    }
  }
  return AIE_OK;
}

int smbus_send_byte(struct amc6821_smbus* smbus, const uint8_t reg)
{
  int ret;
  
  if((ret=_smbus_mux_select(smbus))) return ret;

  return _smbus_send_byte(smbus, reg);
}

int smbus_recv_byte(struct amc6821_smbus* smbus, uint8_t* value)
{
  int ret;
  
  if((ret=_smbus_mux_select(smbus))) return ret;

  return _smbus_recv_byte(smbus, value);
}

int smbus_write_byte(struct amc6821_smbus* smbus, const uint8_t reg, const uint8_t value)
{
  int ret;
  
  if((ret=_smbus_mux_select(smbus))) return ret;

  return _smbus_write_byte(smbus, reg, value);
}

int smbus_write_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, const uint8_t* values, const size_t nregs)
{
  int ret;
  
  if((ret=_smbus_mux_select(smbus))) return ret;

  return _smbus_write_bytes(smbus, first_reg, values, nregs);
}

int smbus_read_byte(struct amc6821_smbus* smbus, const uint8_t reg, uint8_t* value)
{
  int ret;
  
  if((ret=_smbus_mux_select(smbus))) return ret;

  return _smbus_read_byte(smbus, reg, value);
}

int smbus_read_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, uint8_t* values, const size_t nregs)
{
  int ret;
  
  if((ret=_smbus_mux_select(smbus))) return ret;

  return _smbus_read_bytes(smbus, first_reg, values, nregs);
}
