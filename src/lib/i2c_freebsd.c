#ifdef __FreeBSD__
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "i2c.h"

#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
//#define DEBUG_PRINT(...)

int i2c_init(struct amc6821_iic* iic)
{
  DEBUG_PRINT("%s(%p)\n", __func__, iic);
  iic->fid=-1;
  return AIE_OK;
}

int i2c_open(struct amc6821_iic* iic, const char* device)
{
  DEBUG_PRINT("%s(%p, device=\"%s\")\n", __func__, iic, device);

  if(iic->fid >= 0) {
    fprintf(stderr, "%s: A device is already opened!\n", __func__); 
    return AIE_OPEN_FAILED;
  }

  iic->fid = open("/dev/iic0", O_RDWR);

  if(iic->fid < 0) {
    fprintf(stderr, "%s: Error opening the device (%i)\n", __func__, errno); 
    return AIE_OPEN_FAILED;
  }
  return AIE_OK;
}

int i2c_close(struct amc6821_iic* iic)
{
  DEBUG_PRINT("%s(%p)\n", __func__, iic);
  if(iic->fid>=0 && close(iic->fid)) {
    fprintf(stderr, "%s: Error closing the device (%i)\n", __func__, errno); 
    return AIE_CLOSE_FAILED;
  }
  return AIE_OK;
}

int i2c_terminate(struct amc6821_iic* iic)
{
  DEBUG_PRINT("%s(%p)\n", __func__, iic);
  return AIE_OK;
}


int i2c_start(struct amc6821_iic* iic, const uint8_t slave_addr, const ai_rw rw)
{
  DEBUG_PRINT("%s(%p, slave_addr=0x%02X, rw=%u)\n", __func__, iic, slave_addr, rw);
  AIE_CHECK_ADDR(slave_addr);
  struct iiccmd cmd={(slave_addr<<1)|rw, 0, 0, NULL};

  if(ioctl(iic->fid, I2CSTART, &cmd)<0) {
    fprintf(stderr, "%s: Error with I2C start (%i)\n", __func__, errno); 
    return AIE_START_FAILED;
  }
  return AIE_OK;
}

int i2c_repeated_start(struct amc6821_iic* iic, const uint8_t slave_addr, const ai_rw rw)
{
  DEBUG_PRINT("%s(%p, slave_addr=0x%02X, rw=%u)\n", __func__, iic, slave_addr, rw);
  AIE_CHECK_ADDR(slave_addr);
  struct iiccmd cmd={(slave_addr<<1)|rw, 0, 0, NULL};

  if(ioctl(iic->fid, I2CRPTSTART, &cmd)<0) {
    fprintf(stderr, "%s: Error with I2C repeated start (%i)\n", __func__, errno); 
    return AIE_REPEAT_START_FAILED;
  }
  return AIE_OK;
}

int i2c_stop(struct amc6821_iic* iic)
{
  DEBUG_PRINT("%s(%p)\n", __func__, iic);
  if(ioctl(iic->fid, I2CSTOP, NULL)<0) {
    fprintf(stderr, "%s: Error with I2C stop (%i)\n", __func__, errno); 
    return AIE_STOP_FAILED;
  }
  return AIE_OK;
}

int i2c_reset(struct amc6821_iic* iic)
{
  DEBUG_PRINT("%s(%p)\n", __func__, iic);
  struct iiccmd dummy_cmd;

  if(ioctl(iic->fid, I2CRSTCARD, &dummy_cmd)<0) {
    fprintf(stderr, "%s: Error with I2C reset (%i)\n", __func__, errno); 
    return AIE_RESET_FAILED;
  }
  return AIE_OK;
}

int i2c_write(struct amc6821_iic* iic, const uint8_t byte, const bool last)
{
  DEBUG_PRINT("%s(%p, byte=0x%02X, last=%u)\n", __func__, iic, byte, last);
  struct iiccmd cmd={0, 1, last, (char*)&byte};

  if(ioctl(iic->fid, I2CWRITE, &cmd)<0) {
    fprintf(stderr, "%s: Error with I2C write (%i)\n", __func__, errno); 
    return AIE_WRITE_FAILED;
  }
  return AIE_OK;
}

int i2c_write_many(struct amc6821_iic* iic, uint8_t const* bytes, const size_t nbytes, const bool last)
{
  DEBUG_PRINT("%s(%p, bytes=0x", __func__, iic);
  for(int i=0; i<nbytes; ++i) DEBUG_PRINT("%02X", bytes[i]);
  DEBUG_PRINT(", last=%u)\n", last);
  struct iiccmd cmd={0, nbytes, last, (char*)bytes};

  if(ioctl(iic->fid, I2CWRITE, &cmd)<0) {
    fprintf(stderr, "%s: Error with I2C write (%i)\n", __func__, errno); 
    return AIE_WRITE_MANY_FAILED;
  }
  return AIE_OK;
}

int i2c_read(struct amc6821_iic* iic, uint8_t* byte, const bool last)
{
  DEBUG_PRINT("%s(%p, byte=%p, last=%u)\n", __func__, iic, byte, last);
  struct iiccmd cmd={0, 1, last, (char*)byte};

  if(ioctl(iic->fid, I2CREAD, &cmd)<0) {
    fprintf(stderr, "%s: Error with I2C read (%i)\n", __func__, errno); 
    return AIE_READ_FAILED;
  }
  DEBUG_PRINT("%s: 0x%02X was read\n", __func__, *byte);
  return AIE_OK;
}

int i2c_read_many(struct amc6821_iic* iic, uint8_t* bytes, const size_t nbytes, const bool last)
{
  DEBUG_PRINT("%s(%p, bytes=%p, nbytes=%lu, last=%u)\n", __func__, iic, bytes, nbytes, last);
  struct iiccmd cmd={0, nbytes, last, (char*)bytes};

  if(ioctl(iic->fid, I2CREAD, &cmd)<0) {
    fprintf(stderr, "%s: Error with I2C read (%i)\n", __func__, errno); 
    return AIE_READ_MANY_FAILED;
  }
  DEBUG_PRINT("%s: 0x", __func__);
  for(int i=0; i<nbytes; ++i) DEBUG_PRINT("%02X", bytes[i]);
  DEBUG_PRINT(" was read\n");
  return AIE_OK;
}

#endif
