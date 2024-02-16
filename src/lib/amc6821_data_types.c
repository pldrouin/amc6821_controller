#include "amc6821_data_types.h"
#include "amc6821_commands.h"

#define AMC_DTYPE_DEFINE(id) {id ## _set, id ## _get}

int uint8_set(uint8_t* const regs, void const* const data)
{
  *regs = *(uint8_t const*)data;
  return 0;
}

void uint8_get(uint8_t const* const regs, void* const data)
{
  *(uint8_t*)data = *regs;
}

int uint8_fmt(void const* const data, char* str, const size_t len)
{
  return snprintf(str, len, "%"PRIu8, *(uint8_t*)data);
}

int uint8_print(void const* const data, FILE* stream)
{
  return fprintf(stream, "%"PRIu8, *(uint8_t*)data);
}

int uint8_hex_fmt(void const* const data, char* str, const size_t len)
{
  return snprintf(str, len, "0x%02X", *(uint8_t*)data);
}

int uint8_hex_print(void const* const data, FILE* stream)
{
  return fprintf(stream, "0x%02X", *(uint8_t*)data);
}

int uint8_bin_fmt(void const* const data, char* str, const size_t len)
{
  return snprintf(str, len, "%08b", *(uint8_t*)data);
}

int uint8_bin_print(void const* const data, FILE* stream)
{
  return fprintf(stream, "%08b", *(uint8_t*)data);
}

int uint16_set(uint8_t* const regs, void const* const data)
{
  *(uint16_t*)regs = htole16(*(uint16_t const*)data);
  return 0;
}

void uint16_get(uint8_t const* const regs, void* const data)
{
  *(uint16_t*)data = le16toh(*(uint16_t const*)regs);
}

int uint16_fmt(void const* const data, char* str, const size_t len)
{
  return snprintf(str, len, "%"PRIu16, *(const uint16_t*)data);
}

int uint16_print(void const* const data, FILE* stream)
{
  return fprintf(stream, "%"PRIu16, *(const uint16_t*)data);
}

int fan_control_set(uint8_t* const regs, void const* const data)
{
  struct fan_control_data const* const fcd = (struct fan_control_data const*)data;
  CHECK_FAN_CONTROL_SLOPE(fcd->slope);
  *regs = (((fcd->low_temp)>>2)<<3)|fcd->slope;
  return 0;
}

void fan_control_get(uint8_t const* const regs, void* const data)
{
  struct fan_control_data* const fcd = (struct fan_control_data* const)data;
  fcd->low_temp = ((*regs)>>3)<<2;
  fcd->slope = (*regs)&0b111;
}

int fan_control_fmt(void const* const data, char* str, const size_t len)
{
  struct fan_control_data const* const fcd = (struct fan_control_data const*)data;

  switch(fcd->slope) {

    case fan_control_slope_2_dcy_per_C:
      return snprintf(str, len, "%u C low temperature, 2 dcy/C slope", fcd->low_temp);

    case fan_control_slope_4_dcy_per_C:
      return snprintf(str, len, "%u C low temperature, 4 dcy/C slope", fcd->low_temp);

    case fan_control_slope_8_dcy_per_C:
      return snprintf(str, len, "%u C low temperature, 8 dcy/C slope", fcd->low_temp);

    case fan_control_slope_16_dcy_per_C:
      return snprintf(str, len, "%u C low temperature, 16 dcy/C slope", fcd->low_temp);

    case fan_control_slope_32_dcy_per_C:
      return snprintf(str, len, "%u C low temperature, 32 dcy/C slope", fcd->low_temp);
  }
  return 0;
}

int fan_control_print(void const* const data, FILE* stream)
{
  struct fan_control_data const* const fcd = (struct fan_control_data const*)data;

  switch(fcd->slope) {

    case fan_control_slope_2_dcy_per_C:
      return fprintf(stream, "%u C low temperature, 2 dcy/C slope", fcd->low_temp);

    case fan_control_slope_4_dcy_per_C:
      return fprintf(stream, "%u C low temperature, 4 dcy/C slope", fcd->low_temp);

    case fan_control_slope_8_dcy_per_C:
      return fprintf(stream, "%u C low temperature, 8 dcy/C slope", fcd->low_temp);

    case fan_control_slope_16_dcy_per_C:
      return fprintf(stream, "%u C low temperature, 16 dcy/C slope", fcd->low_temp);

    case fan_control_slope_32_dcy_per_C:
      return fprintf(stream, "%u C low temperature, 32 dcy/C slope", fcd->low_temp);
  }
  return 0;
}

int dcy_ramp_set(uint8_t* const regs, void const* const data)
{
  struct dcy_ramp_data const* const drd = (struct dcy_ramp_data const*)data;
  CHECK_DCY_RAMP_STEP(drd->step);
  CHECK_DCY_RAMP_RATE(drd->rate);
  CHECK_DCY_RAMP_THRESHOLD(drd->threshold);
  *regs = ((drd->enabled)<<7)|((drd->step)<<5)|((drd->rate)<<2)|drd->threshold;
  return 0;
}

void dcy_ramp_get(uint8_t const* const regs, void* const data)
{
  struct dcy_ramp_data* const drd = (struct dcy_ramp_data* const)data;
  drd->enabled = (*regs)>>7;
  drd->step = ((*regs)>>5)&0b11;
  drd->rate = ((*regs)>>2)&0b111;
  drd->threshold = (*regs)&0b11;
}

int dcy_ramp_fmt(void const* const data, char* str, const size_t len)
{
  struct dcy_ramp_data const* const drd = (struct dcy_ramp_data const*)data;
  int ret;

  switch(drd->enabled) {
    case true:
      ret=snprintf(str, len, "enabled, ");
      break;
    case false:
      ret=snprintf(str, len, "disabled, ");
  }

  if(ret<len) {

    switch(drd->step) {

      case dcy_ramp_step_1_dcy:
	ret+=snprintf(str+ret, len-ret, "1 dcy step, ");
	break;

      case dcy_ramp_step_2_dcy:
	ret+=snprintf(str+ret, len-ret, "2 dcy step, ");
	break;

      case dcy_ramp_step_4_dcy:
	ret+=snprintf(str+ret, len-ret, "4 dcy step, ");
	break;

      case dcy_ramp_step_8_dcy:
	ret+=snprintf(str+ret, len-ret, "8 dcy step, ");
	break;
    }
  }

  if(ret<len) {

    switch(drd->rate) {

      case dcy_ramp_rate_1_16_s:
	ret+=snprintf(str+ret, len-ret, "0.0625 s rate, ");
	break;

      case dcy_ramp_rate_1_8_s:
	ret+=snprintf(str+ret, len-ret, "0.125 s rate, ");
	break;

      case dcy_ramp_rate_1_4_s:
	ret+=snprintf(str+ret, len-ret, "0.25 s rate, ");
	break;

      case dcy_ramp_rate_1_2_s:
	ret+=snprintf(str+ret, len-ret, "0.5 s rate, ");
	break;

      case dcy_ramp_rate_1_s:
	ret+=snprintf(str+ret, len-ret, "1 s rate, ");
	break;

      case dcy_ramp_rate_2_s:
	ret+=snprintf(str+ret, len-ret, "2 s rate, ");
	break;

      case dcy_ramp_rate_4_s:
	ret+=snprintf(str+ret, len-ret, "4 s rate, ");
	break;

      case dcy_ramp_rate_8_s:
	ret+=snprintf(str+ret, len-ret, "8 s rate, ");
	break;
    }
  }

  if(ret<len) {

    switch(drd->threshold) {

      case dcy_ramp_thresh_1_dcy:
	ret+=snprintf(str+ret, len-ret, "1 dcy threshold");
	break;

      case dcy_ramp_thresh_2_dcy:
	ret+=snprintf(str+ret, len-ret, "2 dcy threshold");
	break;

      case dcy_ramp_thresh_3_dcy:
	ret+=snprintf(str+ret, len-ret, "3 dcy threshold");
	break;

      case dcy_ramp_thresh_4_dcy:
	ret+=snprintf(str+ret, len-ret, "4 dcy threshold");
	break;
    }
  }
  return ret;
}

int dcy_ramp_print(void const* const data, FILE* stream)
{
  struct dcy_ramp_data const* const drd = (struct dcy_ramp_data const*)data;
  int ret;

  switch(drd->enabled) {
    case true:
      ret=fprintf(stream, "enabled, ");
      break;
    case false:
      ret=fprintf(stream, "disabled, ");
  }

  switch(drd->step) {

    case dcy_ramp_step_1_dcy:
      ret+=fprintf(stream, "1 dcy step, ");
      break;

    case dcy_ramp_step_2_dcy:
      ret+=fprintf(stream, "2 dcy step, ");
      break;

    case dcy_ramp_step_4_dcy:
      ret+=fprintf(stream, "4 dcy step, ");
      break;

    case dcy_ramp_step_8_dcy:
      ret+=fprintf(stream, "8 dcy step, ");
      break;
  }

  switch(drd->rate) {

    case dcy_ramp_rate_1_16_s:
      ret+=fprintf(stream, "0.0625 s rate, ");
      break;

    case dcy_ramp_rate_1_8_s:
      ret+=fprintf(stream, "0.125 s rate, ");
      break;

    case dcy_ramp_rate_1_4_s:
      ret+=fprintf(stream, "0.25 s rate, ");
      break;

    case dcy_ramp_rate_1_2_s:
      ret+=fprintf(stream, "0.5 s rate, ");
      break;

    case dcy_ramp_rate_1_s:
      ret+=fprintf(stream, "1 s rate, ");
      break;

    case dcy_ramp_rate_2_s:
      ret+=fprintf(stream, "2 s rate, ");
      break;

    case dcy_ramp_rate_4_s:
      ret+=fprintf(stream, "4 s rate, ");
      break;

    case dcy_ramp_rate_8_s:
      ret+=fprintf(stream, "8 s rate, ");
      break;
  }

  switch(drd->threshold) {

    case dcy_ramp_thresh_1_dcy:
      ret+=fprintf(stream, "1 dcy threshold");
      break;

    case dcy_ramp_thresh_2_dcy:
      ret+=fprintf(stream, "2 dcy threshold");
      break;

    case dcy_ramp_thresh_3_dcy:
      ret+=fprintf(stream, "3 dcy threshold");
      break;

    case dcy_ramp_thresh_4_dcy:
      ret+=fprintf(stream, "4 dcy threshold");
      break;
  }
  return ret;
}

int tach_set(uint8_t* const regs, void const* const data)
{
  *(uint16_t*)regs = htole16((uint16_t)(6e6/ *(float const*)data + 0.5));
  return 0;
}

void tach_get(uint8_t const* const regs, void* const data)
{
  *(uint16_t*)data = le16toh(*(uint16_t const*)regs);
}

int tach_fmt(void const* const data, char* str, const size_t len)
{
  return snprintf(str, len, "%i", (int)(6e6/ *(const uint16_t*)data + 0.5));
}

int tach_print(void const* const data, FILE* stream)
{
  return fprintf(stream, "%i", (int)(6e6/ *(const uint16_t*)data + 0.5));
}

int temp_low_res_fmt(void const* const data, char* str, const size_t len)
{
  return snprintf(str, len, "%"PRIi8" C", *(int8_t const*)data);
}

int temp_low_res_print(void const* const data, FILE* stream)
{
  return fprintf(stream, "%"PRIi8" C", *(int8_t const*)data);
}

const struct amc_dtype amc_dtypes[AMC_NDTYPES]={
  AMC_DTYPE_DEFINE(uint8),  			//AMC_UINT8_DTYPE
  AMC_DTYPE_DEFINE(uint8), 			//AMC_UINT8_HEX_DTYPE
  AMC_DTYPE_DEFINE(uint8), 			//AMC_UINT8_BIN_DTYPE
  AMC_DTYPE_DEFINE(uint16),			//AMC_UINT16_DTYPE
  AMC_DTYPE_DEFINE(fan_control),		//AMC_FAN_CONTROL_DTYPE
  AMC_DTYPE_DEFINE(dcy_ramp),			//AMC_DCY_RAMP_DTYPE
  AMC_DTYPE_DEFINE(uint16),			//AMC_TACH_DTYPE
  AMC_DTYPE_DEFINE(uint8),			//AMC_TEMP_LOW_RES_DTYPE
};
