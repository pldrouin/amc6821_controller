#if __linux__
#include "smbus_linux.h"

//#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG_PRINT(...)

int _smbus_init(struct amc6821_smbus* smbus)
{
  smbus->fid=-1;
  return AIE_OK;
}

int _smbus_open(struct amc6821_smbus* smbus, const char* device)
{

  if(smbus->fid >= 0) {
    fprintf(stderr, "%s: A device is already opened!\n", __func__); 
    return AIE_OPEN_FAILED;
  }
  smbus->fid = open(device, O_RDWR);

  if(smbus->fid < 0) {
     fprintf(stderr, "%s: Error opening the device (%i)\n", __func__, errno); 
    return AIE_OPEN_FAILED;
  }
 
  return AIE_OK;
}

int _smbus_close(struct amc6821_smbus* smbus)
{
  if(smbus->fid>=0 && close(smbus->fid)) {
    fprintf(stderr, "%s: Error closing the device (%i)\n", __func__, errno); 
    return AIE_CLOSE_FAILED;
  }
  return AIE_OK;
}

int _smbus_raw_send_byte(struct amc6821_smbus* smbus, const uint8_t addr, const uint8_t reg)
{
  int ret;

  uint8_t command[] = { reg };
  struct i2c_msg message = { addr, 0, sizeof(command), command };
  struct i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };

  if (ioctl(smbus->fid, I2C_RDWR, &ioctl_data) != 1)
  {
    fprintf(stderr, "%s: Error with I2C write (%i)\n", __func__, errno); 
    ret=AIE_WRITE_FAILED;
    goto iic_failure;
  }
  return AIE_OK;

iic_failure:
  return ret;
}

int _smbus_raw_recv_byte(struct amc6821_smbus* smbus, const uint8_t addr, uint8_t* value)
{
  int ret;

  struct i2c_msg message = { addr, I2C_M_RD, 1, value };
  struct i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };

  if (ioctl(smbus->fid, I2C_RDWR, &ioctl_data) != 1)
  {
    fprintf(stderr, "%s: Error with I2C read (%i)\n", __func__, errno); 
    ret=AIE_READ_FAILED;
    goto iic_failure;
  }
  return AIE_OK;

iic_failure:
  return ret;
}

int _smbus_send_byte(struct amc6821_smbus* smbus, const uint8_t reg)
{
  int ret;

  if((ret=_smbus_raw_send_byte(smbus, smbus->addr, reg))) goto iic_failure;
  return AIE_OK;

iic_failure:
  return ret;
}

int _smbus_recv_byte(struct amc6821_smbus* smbus, uint8_t* value)
{
  int ret;
  
  if((ret=_smbus_raw_recv_byte(smbus, smbus->addr, value))) goto iic_failure;
  return AIE_OK;

iic_failure:
  return ret;
}

int _smbus_write_byte(struct amc6821_smbus* smbus, const uint8_t reg, const uint8_t value)
{
  int ret;
 
  uint8_t command[] = { reg, value };
  struct i2c_msg message = { smbus->addr, 0, sizeof(command), command };
  struct i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };

  if (ioctl(smbus->fid, I2C_RDWR, &ioctl_data) != 1)
  {
    fprintf(stderr, "%s: Error with I2C write (%i)\n", __func__, errno); 
    ret=AIE_WRITE_FAILED;
    goto iic_failure;
  }
  return AIE_OK;

iic_failure:
  return ret;
}

int _smbus_write_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, const uint8_t* values, const size_t nregs)
{
  int ret;
  
  uint8_t command[1+nregs];
  command[0] = first_reg;
  memcpy(command+1, values, nregs);

  struct i2c_msg message = { smbus->addr, 0, sizeof(command), command };
  struct i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };

  if (ioctl(smbus->fid, I2C_RDWR, &ioctl_data) != 1)
  {
    fprintf(stderr, "%s: Error with I2C write (%i)\n", __func__, errno); 
    ret=AIE_WRITE_MANY_FAILED;
    goto iic_failure;
  }
  return AIE_OK;

iic_failure:
  return ret;
}

int _smbus_read_bytes(struct amc6821_smbus* smbus, const uint8_t first_reg, uint8_t* values, const size_t nregs)
{
  int ret;
 
  uint8_t command[] = { first_reg };
  struct i2c_msg messages[] = {
    { smbus->addr, 0, sizeof(command), command },
    { smbus->addr, I2C_M_RD, nregs, values },
  };
  struct i2c_rdwr_ioctl_data ioctl_data = { messages, sizeof(messages)/sizeof(struct i2c_msg) };

  if (ioctl(smbus->fid, I2C_RDWR, &ioctl_data) != sizeof(messages)/sizeof(struct i2c_msg))
  {
    fprintf(stderr, "%s: Error with I2C write/read (%i)\n", __func__, errno); 
    ret=AIE_READ_WRITE_FAILED;
    goto iic_failure;
  }
  return AIE_OK;

iic_failure:
  return ret;
}
#endif
