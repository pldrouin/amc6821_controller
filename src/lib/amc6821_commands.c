#include "amc6821_commands.h"

int _amc6821_rw_single_reg_init(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx)
{
  int ret;

  if(!AMC_INITIALIZED_RW_REG(reg_rw_idx)) {

    if((ret=amc6821_smbus_read_byte(&dev->smbus, reg, dev->amc_rw_orig_regs + reg_rw_idx))) return ret;
    dev->amc_rw_regs_initialized[(reg_rw_idx)>>3] |= (1<<(reg_rw_idx%8));
  }
  return 0;
}

int _amc6821_rw_single_reg_load(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx, void* const data, const uint8_t dtype_idx)
{
  int ret;
  uint8_t cur_reg;

  if((ret=amc6821_smbus_read_byte(&dev->smbus, reg, &cur_reg))) return ret;

  if(!AMC_INITIALIZED_RW_REG(reg_rw_idx)) {
    dev->amc_rw_orig_regs[reg_rw_idx] = cur_reg;
    dev->amc_rw_regs_initialized[reg_rw_idx>>3] |= (1<<(reg_rw_idx%8));
  }
  amc_dtypes[dtype_idx].get_func(&cur_reg, data);
  return 0;
}

int _amc6821_rw_multiple_regs_load(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx, const size_t nbytes, void* const data, const uint8_t dtype_idx)
{
  int ret;
  uint8_t cur_regs[nbytes];
  size_t i;

  if((ret=amc6821_smbus_read_bytes(&dev->smbus, reg, cur_regs, nbytes))) return ret;

  for(i=reg_rw_idx; i<reg_rw_idx+nbytes; ++i) {

    if(!AMC_INITIALIZED_RW_REG(i)) {
      dev->amc_rw_orig_regs[i] = cur_regs[i-reg_rw_idx];
      dev->amc_rw_regs_initialized[i>>3] |= (1<<(i%8));
    }
  }
  amc_dtypes[dtype_idx].get_func(cur_regs, data);
  return 0;
}

int _amc6821_rw_single_reg_load_original(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx, void* const data, const uint8_t dtype_idx)
{
  int ret;

  if(!AMC_INITIALIZED_RW_REG(reg_rw_idx)) {

    if((ret=amc6821_smbus_read_byte(&dev->smbus, reg, dev->amc_rw_orig_regs+reg_rw_idx))) return ret;
    dev->amc_rw_regs_initialized[reg_rw_idx>>3] |= (1<<(reg_rw_idx%8));
  }

  amc_dtypes[dtype_idx].get_func(dev->amc_rw_orig_regs+reg_rw_idx, data);
  return 0;
}

int _amc6821_rw_multiple_regs_load_original(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx, const size_t nbytes, void* const data, const uint8_t dtype_idx)
{
  int ret;
  size_t i;

  for(i=reg_rw_idx; i<reg_rw_idx+nbytes; ++i) {

    if(!AMC_INITIALIZED_RW_REG(i)) {

      if((ret=amc6821_smbus_read_bytes(&dev->smbus, reg, dev->amc_rw_orig_regs+i, nbytes))) return ret;
      dev->amc_rw_regs_initialized[i>>3] |= (1<<(i%8));
    }
  }
  amc_dtypes[dtype_idx].get_func(dev->amc_rw_orig_regs+reg_rw_idx, data);
  return 0;
}

int _amc6821_w_single_reg_store(struct amc6821_device* dev, const uint8_t reg, void const* const data, const uint8_t dtype_idx)
{
  int ret;
  uint8_t reg_val;

  if((ret=amc_dtypes[dtype_idx].set_func(&reg_val, data))) return ret;

  if((ret=amc6821_smbus_write_byte(&dev->smbus, reg, reg_val))) return ret;
  return 0;
}

int _amc6821_w_multiple_regs_store(struct amc6821_device* dev, const uint8_t reg, const size_t nbytes, void const* const data, const uint8_t dtype_idx)
{
  int ret;
  uint8_t reg_vals[nbytes];

  if((ret=amc_dtypes[dtype_idx].set_func(reg_vals, data))) return ret;

  if((ret=amc6821_smbus_write_bytes(&dev->smbus, reg, reg_vals, nbytes))) return ret;
  return 0;
}

int _amc6821_rw_single_reg_restore(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx)
{
  int ret;

  if((ret=amc6821_smbus_write_byte(&dev->smbus, reg, dev->amc_rw_orig_regs[reg_rw_idx]))) return ret;
  return 0;
}

int _amc6821_rw_multiple_regs_restore(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx, const size_t nbytes)
{
  int ret;

  if((ret=amc6821_smbus_write_bytes(&dev->smbus, reg, dev->amc_rw_orig_regs+reg_rw_idx, nbytes))) return ret;
  return 0;
}

int _amc6821_ro_single_reg_load(struct amc6821_device* dev, const uint8_t reg, void* const data, const uint8_t dtype_idx)
{
  int ret;
  uint8_t cur_reg;

  if((ret=amc6821_smbus_read_byte(&dev->smbus, reg, &cur_reg))) return ret;
  amc_dtypes[dtype_idx].get_func(&cur_reg, data);
  return 0;
}

int _amc6821_ro_multiple_regs_load(struct amc6821_device* dev, const uint8_t reg, const size_t nbytes, void* const data, const uint8_t dtype_idx)
{
  int ret;
  uint8_t cur_regs[nbytes];

  if((ret=amc6821_smbus_read_bytes(&dev->smbus, reg, cur_regs, nbytes))) return ret;
  amc_dtypes[dtype_idx].get_func(cur_regs, data);
  return 0;
}

int amc6821_init(struct amc6821_device* dev)
{
  memset(dev->amc_rw_regs_initialized, 0, (AMC_N_RW_REGS+7)>>3);
  return amc6821_smbus_init(&dev->smbus);
}

int _amc6821_id(struct amc6821_device* dev)
{
  int ret;

  uint8_t ids[2];

  if((ret=amc6821_smbus_read_bytes(&dev->smbus, AMC_DEVICE_ID_REG, ids, 2))) return ret;

  if(ids[0] != AMC_DEVICE_ID_DEFAULT) {
    fprintf(stderr,"%s: Error; Device ID (%" PRIu8 ") is invalid!\n", __func__, ids[0]);
    amc6821_smbus_close(&dev->smbus);
    return AIE_INVALID_VALUE;
  }

  if(ids[1] != AMC_COMPANY_ID_DEFAULT) {
    fprintf(stderr,"%s: Error; Company ID (%" PRIu8 ") is invalid!\n", __func__, ids[1]);
    amc6821_smbus_close(&dev->smbus);
    return AIE_INVALID_VALUE;
  }

  return AIE_OK;
}

int amc6821_open(struct amc6821_device* dev, const char* device, const uint8_t dev_addr)
{
  int ret;

  if((ret=amc6821_smbus_open(&dev->smbus, device, dev_addr))) return ret;

  return _amc6821_id(dev);
}

int amc6821_mux_open(struct amc6821_device* dev, const char* device, const uint8_t dev_addr, const uint8_t mux_addr, const uint8_t mux_reg_value, const uint8_t mux_reg_mask)
{
  int ret;

  if((ret=amc6821_smbus_mux_open(&dev->smbus, device, dev_addr, mux_addr, mux_reg_value, mux_reg_mask))) return ret;

  return _amc6821_id(dev);
}

int amc6821_close(struct amc6821_device* dev)
{
  return amc6821_smbus_close(&dev->smbus);
}

int amc6821_terminate(struct amc6821_device* dev)
{
  return amc6821_smbus_terminate(&dev->smbus);
}
