#ifndef _AMC6821_DATA_TYPES
#define _AMC6821_DATA_TYPES

#include<stdint.h>
#include<stdbool.h>
#include<inttypes.h>
#include<stdlib.h>
#include<stdio.h>
#include<endian.h>

struct amc6821_device;

typedef int (*amc_dtype_set)(uint8_t* const regs, void const* const data);
typedef void (*amc_dtype_get)(uint8_t const* const regs, void* const data);
typedef int (*amc_dtype_fmt)(void const* const data, char* str, const size_t len);
typedef int (*amc_dtype_print)(void const* const data, FILE* stream);

struct amc_dtype {
  amc_dtype_set set_func;
  amc_dtype_get get_func;
};

enum fan_control_slope {
  fan_control_slope_2_dcy_per_C=0b100, fan_control_slope_4_dcy_per_C=0b011, fan_control_slope_8_dcy_per_C=0b010,
  fan_control_slope_16_dcy_per_C=0b001, fan_control_slope_32_dcy_per_C=0
};

struct fan_control_data {
  uint8_t low_temp;
  enum fan_control_slope slope;
};

#define CHECK_FAN_CONTROL_SLOPE(slope) ({\
  switch(slope) {\
    case fan_control_slope_2_dcy_per_C:\
    case fan_control_slope_4_dcy_per_C:\
    case fan_control_slope_8_dcy_per_C:\
    case fan_control_slope_16_dcy_per_C:\
    case fan_control_slope_32_dcy_per_C:\
      break;\
    default:\
    return AIE_INVALID_VALUE;\
  }\
})

enum dcy_ramp_step {
  dcy_ramp_step_1_dcy=0, dcy_ramp_step_2_dcy=1, dcy_ramp_step_4_dcy=2, dcy_ramp_step_8_dcy=3
};

#define CHECK_DCY_RAMP_STEP(step) ({\
    if(step<0 || step>0b11) return AIE_INVALID_VALUE;\
})

enum dcy_ramp_rate {
  dcy_ramp_rate_1_16_s=0, dcy_ramp_rate_1_8_s=1, dcy_ramp_rate_1_4_s=2, dcy_ramp_rate_1_2_s=3, dcy_ramp_rate_1_s=4, dcy_ramp_rate_2_s=5, dcy_ramp_rate_4_s=6, dcy_ramp_rate_8_s=7
};

#define CHECK_DCY_RAMP_RATE(rate) ({\
    if(rate<0 || rate>0b111) return AIE_INVALID_VALUE;\
})

enum dcy_ramp_threshold {
  dcy_ramp_thresh_1_dcy=0, dcy_ramp_thresh_2_dcy=1, dcy_ramp_thresh_3_dcy=2, dcy_ramp_thresh_4_dcy=3
};

#define CHECK_DCY_RAMP_THRESHOLD(threshold) ({\
    if(threshold<0 || threshold>0b11) return AIE_INVALID_VALUE;\
})

struct dcy_ramp_data {
  bool enabled;
  enum dcy_ramp_step step;
  enum dcy_ramp_rate rate;
  enum dcy_ramp_threshold threshold;
};

#define AMC_NDTYPES			7

#define AMC_UINT8_DTYPE_IDX		0
#define AMC_UINT8_DTYPE_DECODED		uint8_t
#define AMC_UINT16_DTYPE_IDX		1
#define AMC_UINT16_DTYPE_DECODED	uint16_t
#define AMC_FAN_CONTROL_DTYPE_IDX	2
#define AMC_FAN_CONTROL_DTYPE_DECODED	struct fan_control_data
#define AMC_DCY_RAMP_DTYPE_IDX		3
#define AMC_DCY_RAMP_DTYPE_DECODED	struct dcy_ramp_data
#define AMC_TEMP_LOW_RES_DTYPE_IDX	4
#define AMC_TEMP_LOW_RES_DTYPE_DECODED	int8_t
#define AMC_REMOTE_TEMP_HIGH_RES_DTYPE_IDX	5
#define AMC_REMOTE_TEMP_HIGH_RES_DTYPE_DECODED	int16_t
#define AMC_LOCAL_TEMP_HIGH_RES_DTYPE_IDX	6
#define AMC_LOCAL_TEMP_HIGH_RES_DTYPE_DECODED	int16_t

extern const struct amc_dtype amc_dtypes[AMC_NDTYPES];

int uint8_fmt(void const* const data, char* str, const size_t len);
int uint8_print(void const* const data, FILE* stream);
int uint8_hex_fmt(void const* const data, char* str, const size_t len);
int uint8_hex_print(void const* const data, FILE* stream);
int uint8_bin_fmt(void const* const data, char* str, const size_t len);
int uint8_bin_print(void const* const data, FILE* stream);
int uint16_fmt(void const* const data, char* str, const size_t len);
int uint16_print(void const* const data, FILE* stream);
int conf1_fmt(void const* const data, char* str, const size_t len);
int conf1_print(void const* const data, FILE* stream);
int conf2_fmt(void const* const data, char* str, const size_t len);
int conf2_print(void const* const data, FILE* stream);
int conf3_fmt(void const* const data, char* str, const size_t len);
int conf3_print(void const* const data, FILE* stream);
int conf4_fmt(void const* const data, char* str, const size_t len);
int conf4_print(void const* const data, FILE* stream);
int status1_fmt(void const* const data, char* str, const size_t len);
int status1_print(void const* const data, FILE* stream);
int status2_fmt(void const* const data, char* str, const size_t len);
int status2_print(void const* const data, FILE* stream);
int fan_control_fmt(void const* const data, char* str, const size_t len);
int fan_control_print(void const* const data, FILE* stream);
int dcy_ramp_fmt(void const* const data, char* str, const size_t len);
int dcy_ramp_print(void const* const data, FILE* stream);
int tach_fmt(void const* const data, char* str, const size_t len);
int tach_print(void const* const data, FILE* stream);
int temp_low_res_fmt(void const* const data, char* str, const size_t len);
int temp_low_res_print(void const* const data, FILE* stream);
int temp_high_res_fmt(void const* const data, char* str, const size_t len);
int temp_high_res_print(void const* const data, FILE* stream);

#endif
