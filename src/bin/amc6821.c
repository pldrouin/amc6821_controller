#include "amc6821.h"

int main(int argc, char *const argv[])
{

  if(argc!=2) {
    fprintf(stderr, "Usage: %s device\n", argv[0]);
    return 1;
  }
  struct amc6821_device_iic dev_iic;
  int ret;

  if((ret=amc6821_i2c_init(&dev_iic))) {
    fprintf(stderr, "Error: amc6821_i2c_init (%i)\n", ret);
    return 1;
  }

  if((ret=amc6821_i2c_open(&dev_iic, argv[1], 0x18, 0x77, 0x9, 0xF))) {
    fprintf(stderr, "Error: amc6821_i2c_open (%i)\n", ret);
    return 1;
  }

  if((ret=amc6821_i2c_close(&dev_iic))) {
    fprintf(stderr, "Error: amc6821_i2c_close (%i)\n", ret);
    return 1;
  }

  if((ret=amc6821_i2c_terminate(&dev_iic))) {
    fprintf(stderr, "Error: amc6821_i2c_terminate (%i)\n", ret);
    return 1;
  }
  return 0;
}
