#ifndef _AMC6821_I2C_COMMON_
#define _AMC6821_I2C_COMMON_

#include<stdint.h>
#include<inttypes.h>
#include<stdbool.h>
#include<stddef.h>
#include<stdio.h>

#define AIE_OK 0
#define AIE_INVALID_VALUE -1
#define AIE_OPEN_FAILED -2
#define AIE_CLOSE_FAILED -3
#define AIE_INVALID_ADDR -4
#define AIE_START_FAILED -5
#define AIE_REPEAT_START_FAILED -6
#define AIE_STOP_FAILED -7
#define AIE_RESET_FAILED -8
#define AIE_WRITE_FAILED -9
#define AIE_WRITE_MANY_FAILED -10
#define AIE_READ_FAILED -11
#define AIE_READ_MANY_FAILED -12

#define AIE_CHECK_ADDR(ADDR) ({if(ADDR&(1<<7)) {fprintf(stderr, "%s: Error: Invalid I2C address (%" PRIu8 ")\n", __func__, ADDR); return AIE_INVALID_ADDR;}})

#endif
