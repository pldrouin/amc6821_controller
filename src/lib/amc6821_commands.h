#ifndef _AMC6821_COMMANDS_
#define _AMC6821_COMMANDS_

#include<string.h>

#include "smbus.h"
#include "amc6821_map.h"
#include "amc6821_data_types.h"

struct amc6821_device
{
  struct amc6821_smbus smbus;
  uint8_t amc_rw_orig_regs[AMC_N_RW_REGS];
  uint8_t amc_rw_regs_initialized[(AMC_N_RW_REGS+7)>>3];
};

#define AMC_INITIALIZED_RW_REG(reg_rw_idx) ((dev->amc_rw_regs_initialized[(reg_rw_idx)>>3]>>((reg_rw_idx)%8))&1)

int _amc6821_rw_single_reg_init(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx);

#define AMC_RW_REG_INIT(reg_id) (_amc6821_rw_single_reg_init(dev, reg_id ## _REG, reg_id ## _RW_IDX)

int _amc6821_rw_single_reg_load(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx, void* const data, const uint8_t dtype_idx);
int _amc6821_rw_multiple_regs_load(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx, const size_t nbytes, void* const data, const uint8_t dtype_idx);
int _amc6821_rw_single_reg_load_original(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx, void* const data, const uint8_t dtype_idx);
int _amc6821_rw_multiple_regs_load_original(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx, const size_t nbytes, void* const data, const uint8_t dtype_idx);
int _amc6821_w_single_reg_store(struct amc6821_device* dev, const uint8_t reg, void const* const data, const uint8_t dtype_idx);
int _amc6821_w_multiple_regs_store(struct amc6821_device* dev, const uint8_t reg, const size_t nbytes, void const* const data, const uint8_t dtype_idx);
int _amc6821_rw_single_reg_restore(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx);
int _amc6821_rw_multiple_regs_restore(struct amc6821_device* dev, const uint8_t reg, const uint8_t reg_rw_idx, const size_t nbytes);

#define AMC_RW_REG_LOAD_FUNCT_1(reg_id, data, dtype) _amc6821_rw_single_reg_load(dev, reg_id ## _REG, reg_id ## _RW_IDX, data, dtype ## _IDX)
#define AMC_RW_REG_LOAD_FUNCT_2(reg_id, data, dtype) _amc6821_rw_multiple_regs_load(dev, reg_id ## _REG, reg_id ## _RW_IDX, 2, data, dtype ## _IDX)
#define AMC_RW_REG_LOAD_ORIGINAL_FUNCT_1(reg_id, data, dtype) _amc6821_rw_single_reg_load_original(dev, reg_id ## _REG, reg_id ## _RW_IDX, data, dtype ## _IDX)
#define AMC_RW_REG_LOAD_ORIGINAL_FUNCT_2(reg_id, data, dtype) _amc6821_rw_multiple_regs_load_original(dev, reg_id ## _REG, reg_id ## _RW_IDX, 2, data, dtype ## _IDX)
#define AMC_RW_REG_STORE_FUNCT_1(reg_id, data, dtype) _amc6821_w_single_reg_store(dev, reg_id ## _REG, data, dtype ## _IDX)
#define AMC_RW_REG_STORE_FUNCT_2(reg_id, data, dtype) _amc6821_w_multiple_regs_store(dev, reg_id ## _REG, 2, data, dtype ## _IDX)
#define AMC_RW_REG_RESTORE_FUNCT_1(reg_id) _amc6821_rw_single_reg_restore(dev, reg_id ## _REG, reg_id ## _RW_IDX)
#define AMC_RW_REG_RESTORE_FUNCT_2(reg_id) _amc6821_rw_multiple_regs_restore(dev, reg_id ## _REG, reg_id ## _RW_IDX, 2)
#define AMC_RW_REG_FUNCTS(funct_id, reg_id, nbytes, dtype) \
inline static int amc6821_load_ ## funct_id(struct amc6821_device* dev, dtype ## _DECODED *const data) {\
      return AMC_RW_REG_LOAD_FUNCT_ ## nbytes(reg_id, data, dtype);\
}\
inline static int amc6821_load_original ## funct_id(struct amc6821_device* dev, dtype ## _DECODED *const data) {\
      return AMC_RW_REG_LOAD_ORIGINAL_FUNCT_ ## nbytes(reg_id, data, dtype);\
}\
inline static int amc6821_store_ ## funct_id(struct amc6821_device* dev, dtype ## _DECODED *const data) {\
      return AMC_RW_REG_STORE_FUNCT_ ## nbytes(reg_id, data, dtype);\
}\
inline static int amc6821_restore_ ## funct_id(struct amc6821_device* dev) {\
      return AMC_RW_REG_RESTORE_FUNCT_ ## nbytes(reg_id);\
}

int _amc6821_ro_single_reg_load(struct amc6821_device* dev, const uint8_t reg, void* const data, const uint8_t dtype_idx);
int _amc6821_ro_multiple_regs_load(struct amc6821_device* dev, const uint8_t reg, const size_t nbytes, void* const data, const uint8_t dtype_idx);

#define AMC_RO_REG_LOAD_FUNCT_1(reg_id, data, dtype) _amc6821_ro_single_reg_load(dev, reg_id ## _REG, data, dtype ## _IDX)
#define AMC_RO_REG_LOAD_FUNCT_2(reg_id, data, dtype) _amc6821_ro_multiple_regs_load(dev, reg_id ## _REG, 2, data, dtype ## _IDX)
#define AMC_RO_REG_FUNCTS(funct_id, reg_id, nbytes, dtype) \
inline static int amc6821_load_ ## funct_id(struct amc6821_device* dev, dtype ## _DECODED *const data) {\
      return AMC_RO_REG_LOAD_FUNCT_ ## nbytes(reg_id, data, dtype);\
}

int amc6821_init(struct amc6821_device* dev);
int amc6821_open(struct amc6821_device* dev, const char* device, const uint8_t dev_addr);
int amc6821_mux_open(struct amc6821_device* dev, const char* device, const uint8_t dev_addr, const uint8_t mux_addr, const uint8_t mux_reg_value, const uint8_t mux_reg_mask);
int amc6821_close(struct amc6821_device* dev);
int amc6821_terminate(struct amc6821_device* dev);

AMC_RW_REG_FUNCTS(conf1				,AMC_CONF1			,1,	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(conf2				,AMC_CONF2			,1, 	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(conf3				,AMC_CONF3			,1, 	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(conf4				,AMC_CONF4			,1, 	AMC_UINT8_DTYPE)

AMC_RW_REG_FUNCTS(local_high_temp_limit		,AMC_LOCAL_HIGH_TEMP_LIMIT	,1,	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(local_low_temp_limit		,AMC_LOCAL_LOW_TEMP_LIMIT	,1,	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(local_therm_limit		,AMC_LOCAL_THERM_LIMIT		,1,	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(local_crit_limit		,AMC_LOCAL_CRIT_LIMIT		,1,	AMC_UINT8_DTYPE)

AMC_RW_REG_FUNCTS(remote_high_temp_limit	,AMC_REMOTE_HIGH_TEMP_LIMIT	,1,	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(remote_low_temp_limit		,AMC_REMOTE_LOW_TEMP_LIMIT	,1,	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(remote_therm_limit		,AMC_REMOTE_THERM_LIMIT		,1,	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(remote_crit_limit		,AMC_REMOTE_CRIT_LIMIT		,1,	AMC_UINT8_DTYPE)

AMC_RW_REG_FUNCTS(psv_temp			,AMC_PSV_TEMP			,1,	AMC_UINT5_DTYPE)

AMC_RW_REG_FUNCTS(fan_characteristics		,AMC_FAN_CHARACTERISTICS	,1,	AMC_FAN_CHARACTERISTICS_DTYPE)
AMC_RW_REG_FUNCTS(dcy_low_temp			,AMC_DCY_LOW_TEMP		,1,	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(dcy				,AMC_DCY			,1,	AMC_UINT8_DTYPE)
AMC_RW_REG_FUNCTS(dcy_ramp			,AMC_DCY_RAMP			,1,	AMC_DCY_RAMP_DTYPE)
AMC_RW_REG_FUNCTS(local_temp_fan_control	,AMC_LOCAL_TEMP_FAN_CONTROL	,1,	AMC_FAN_CONTROL_DTYPE)
AMC_RW_REG_FUNCTS(remote_temp_fan_control	,AMC_REMOTE_TEMP_FAN_CONTROL	,1,	AMC_FAN_CONTROL_DTYPE)

AMC_RW_REG_FUNCTS(tach_low_limit		,AMC_TACH_LOW_LIMIT_LBYTE	,2,	AMC_TACH_DTYPE)
AMC_RW_REG_FUNCTS(tach_high_limit		,AMC_TACH_HIGH_LIMIT_LBYTE	,2,	AMC_TACH_DTYPE)
AMC_RW_REG_FUNCTS(tach_setting			,AMC_TACH_SETTING_LBYTE		,2,	AMC_TACH_DTYPE)

AMC_RO_REG_FUNCTS(status1			,AMC_STATUS1			,1,	AMC_UINT8_DTYPE)
AMC_RO_REG_FUNCTS(status2			,AMC_STATUS2			,1, 	AMC_UINT8_DTYPE)

AMC_RO_REG_FUNCTS(local_temp_low_res		,AMC_LOCAL_TEMP_DATA_HBYTE	,1,	AMC_TEMP_LOW_RES_DTYPE)
AMC_RO_REG_FUNCTS(remote_temp_low_res		,AMC_REMOTE_TEMP_DATA_HBYTE	,1,	AMC_TEMP_LOW_RES_DTYPE)
AMC_RO_REG_FUNCTS(tach				,AMC_TACH_DATA_LBYTE		,2,	AMC_TACH_DTYPE)

int amc6821_load_remote_temp_high_res(struct amc6821_device* dev, uint16_t *const data);
int amc6821_load_local_remote_temp_high_res(struct amc6821_device* dev, uint16_t *const data);
#endif
