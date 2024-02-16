#include "amc6821_config.h"

#define CFUNCTSTR (__func__+7)

#define HT_SET_FUNCT(funct) (ht_set(config->ht, #funct, config_ ## funct))

#define HTC1_SET(flag) (ht_set(config->htcs[0], #flag, (void*)O_ ## flag))
#define HTC2_SET(flag) (ht_set(config->htcs[1], #flag, (void*)O_ ## flag))
#define HTC3_SET(flag) (ht_set(config->htcs[2], #flag, (void*)O_ ## flag))
#define HTC4_SET(flag) (ht_set(config->htcs[3], #flag, (void*)O_ ## flag))

#define HT_GET_SET_FUNCTS(funct_id) \
    ht_set(config->ht, "get_" #funct_id, config_get_ ## funct_id);\
    ht_set(config->ht, "set_" #funct_id, config_set_ ## funct_id);\
    ht_set(config->ht, "restore_" #funct_id, config_restore_ ## funct_id);

#define HT_GET_MOD_FUNCTS(funct_id) \
    ht_set(config->ht, "get_" #funct_id, config_get_ ## funct_id);\
    ht_set(config->ht, "modify_" #funct_id, config_modify_ ## funct_id);\
    ht_set(config->ht, "restore_" #funct_id, config_restore_ ## funct_id);

#define HT_GET_FUNCT(funct_id) \
    ht_set(config->ht, "get_" #funct_id, config_get_ ## funct_id);\

#define CONFIG_RW_GET_RESTORE_FUNCTS(funct_id, dtype, print_id) \
int config_get_ ## funct_id(struct amc6821_config* config) {\
  int ret;\
  dtype data;\
  if((ret=amc6821_load_ ## funct_id(&config->dev, &data))) return ret;\
  printf("%s: ",CFUNCTSTR);\
  print_id ## _print(&data, stdout);\
  printf("\n");\
  return 0;\
}\
int config_restore_ ## funct_id(struct amc6821_config* config) {\
  int ret;\
  if((ret=amc6821_restore_ ## funct_id(&config->dev))) return ret;\
  return 0;\
}

#define CONFIG_RO_GET_FUNCT(funct_id, dtype, print_id) \
int config_get_ ## funct_id(struct amc6821_config* config) {\
  int ret;\
  dtype data;\
  if((ret=amc6821_load_ ## funct_id(&config->dev, &data))) return ret;\
  printf("%s: ",CFUNCTSTR);\
  print_id ## _print(&data, stdout);\
  printf("\n");\
  return 0;\
}\

#define BSTR "\e[1m"
#define UBSTR "\e[m"

int config_help(struct amc6821_config* config)
{
  printf(BSTR "config" UBSTR " configuration_file\n"); 
  printf(BSTR "olog" UBSTR " output_log_file\n"); 
  printf(BSTR "elog" UBSTR " error_log_file\n"); 
  printf(BSTR "prompt" UBSTR "\n"); 

  printf(BSTR "exit" UBSTR "\n"); 
  return 0;
}

int config_config(struct amc6821_config* config)
{
  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Missing configuration file name!\n",CFUNCTSTR);
    return -1;
  }

  if(args_open_conf_file(&config->a_h, config->pbuf)) {
    fprintf(stderr,"%s: Error: Error while trying to open configuration file!\n",CFUNCTSTR);
    return -1;
  }

  //If we switch to interactive mode (reading instructions from standard
  //input)
  if(args_reading_from_stdin(&config->a_h)) {

      config->interactive=true;
      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wunused-result"
      write(config->oldstderr,"@INTERACTIVE\n",13);
      #pragma GCC diagnostic push

    //Otherwise, if reading from a configuration file
  } else {
    printf("Reading from config file %s\n",config->pbuf);
  }
  return 0;
}

//Output file log, redirecting the standard output
int config_olog(struct amc6821_config* config)
{
  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Missing output log file name!\n",CFUNCTSTR);
    return -1;
  }
  fflush(stdout);
  close(config->oout);

  if((config->oout=open(config->pbuf,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0) {
    fprintf(stderr,"%s: Error: Cannot open file '%s' in write mode\n",CFUNCTSTR,config->pbuf);
    return 1;
  }
  dup2(config->oout,1);
  return 0;
}

//Output error file log, redirecting the standard error
int config_elog(struct amc6821_config* config)
{
  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Missing error log file name!\n",CFUNCTSTR);
    return -1;
  }
  fflush(stderr);
  close(config->eout);

  if((config->eout=open(config->pbuf,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0) {
    fprintf(stderr,"%s: Error: cannot open file '%s' in write mode\n",CFUNCTSTR,config->pbuf);
    return 1;
  }
  dup2(config->eout,2);
  return 0;
}

int prompt_callback(const char* param, void* ptr) {
  struct amc6821_config* config=(struct amc6821_config*)ptr;

  int (*cfunct)(struct amc6821_config* config)=(int (*)(struct amc6821_config* config))ht_get(config->ht, param);
  int cret;

  if(cfunct) {
    cret=cfunct(config);

    if(cret == CONFIG_EXIT_RET) return ARGS_CB_EXIT;

  } else {
    fprintf(stderr,"%s: Error: Unknown command: '%s'\n",CFUNCTSTR,param);
    return -1;
  }
  return ARGS_CB_OK;
}

int config_prompt(struct amc6821_config* config)
{
  return args_prompt(&config->a_h, "<amc6821> ", config->pbuf, AMC_PBUF_SIZE, prompt_callback, config);
}

int config_open(struct amc6821_config* config)
{
  char* device;
  uint8_t dev_addr;
  int ret;

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Missing device path!\n",CFUNCTSTR);
    return -1;
  }
  device=strdup(config->pbuf);

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    free(device);
    fprintf(stderr,"%s: Error: Missing device address!\n",CFUNCTSTR);
    return -2;
  }

  if(sscanf(config->pbuf, "%hhx", &dev_addr) != 1) {
    free(device);
    fprintf(stderr,"%s: Error: Device address '%s' is invalid!\n",CFUNCTSTR,config->pbuf);
    return -3;
  }

  ret=amc6821_open(&config->dev, device, dev_addr);
  free(device);

  if(ret) {
    fprintf(stderr, "Error: amc6821_open (%i)\n", ret);
    return -10;
  }
  return 0;
}

int config_mux_open(struct amc6821_config* config)
{
  char* device;
  uint8_t dev_addr;
  uint8_t mux_addr;
  uint8_t mux_reg_value;
  uint8_t mux_reg_mask;
  int ret;

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Missing device path!\n",CFUNCTSTR);
    return -1;
  }
  device=strdup(config->pbuf);

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    free(device);
    fprintf(stderr,"%s: Error: Missing device address!\n",CFUNCTSTR);
    return -2;
  }

  if(sscanf(config->pbuf, "%hhx", &dev_addr) != 1) {
    free(device);
    fprintf(stderr,"%s: Error: Device address '%s' is invalid!\n",CFUNCTSTR,config->pbuf);
    return -3;
  }

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    free(device);
    fprintf(stderr,"%s: Error: Missing mux address!\n",CFUNCTSTR);
    return -4;
  }

  if(sscanf(config->pbuf, "%hhx", &mux_addr) != 1) {
    free(device);
    fprintf(stderr,"%s: Error: Mux address '%s' is invalid!\n",CFUNCTSTR,config->pbuf);
    return -5;
  }

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    free(device);
    fprintf(stderr,"%s: Error: Missing mux register value!\n",CFUNCTSTR);
    return -6;
  }

  if(sscanf(config->pbuf, "%hhx", &mux_reg_value) != 1) {
    free(device);
    fprintf(stderr,"%s: Error: Mux register value '%s' is invalid!\n",CFUNCTSTR,config->pbuf);
    return -7;
  }

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    free(device);
    fprintf(stderr,"%s: Error: Missing mux register mask!\n",CFUNCTSTR);
    return -8;
  }

  if(sscanf(config->pbuf, "%hhx", &mux_reg_mask) != 1) {
    free(device);
    fprintf(stderr,"%s: Error: Mux register mask '%s' is invalid!\n",CFUNCTSTR,config->pbuf);
    return -9;
  }

  ret=amc6821_mux_open(&config->dev, device, dev_addr, mux_addr, mux_reg_value, mux_reg_mask);
  free(device);

  if(ret) {
    fprintf(stderr, "Error: amc6821_open (%i)\n", ret);
    return -10;
  }
  return 0;
}

int _config_modify_conf(struct amc6821_config* config, const uint8_t htcidx, int (*load_funct)(struct amc6821_device*, uint8_t *const), int (*store_funct)(struct amc6821_device*, uint8_t *const))
{
  int ret;
  uint8_t data;
  uint16_t flag, flags=0;

  if((ret=load_funct(&config->dev, &data))) return ret;

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Missing configuration flags!\n",CFUNCTSTR);
    return -1;
  }
  char* tok=strtok(config->pbuf, ",");

  while(tok) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    flag=(uint16_t)ht_get(config->htcs[htcidx], tok);
    #pragma GCC diagnostic pop

    if(flag==0) {
      fprintf(stderr,"%s: Error: Configuration flag '%s' is unknown!\n",CFUNCTSTR,tok);
      return -1;
    }
    flags|=flag;
    tok=strtok(NULL, ",");
  }
  uint8_t newdata=MOD_FLAGS(data, flags);

  if(newdata != data) {

    if((ret=store_funct(&config->dev, &newdata))) {
      fprintf(stderr, "Error: While storing register value (%i)\n", ret);
      return -1;
    }
  }
  return 0;
}

CONFIG_RW_GET_RESTORE_FUNCTS(conf1, uint8_t, uint8_bin);

int config_modify_conf1(struct amc6821_config* config)
{
  return _config_modify_conf(config, 0, amc6821_load_conf1, amc6821_store_conf1);
}

CONFIG_RW_GET_RESTORE_FUNCTS(conf2, uint8_t, uint8_bin);

int config_modify_conf2(struct amc6821_config* config)
{
  return _config_modify_conf(config, 1, amc6821_load_conf2, amc6821_store_conf2);
}

CONFIG_RW_GET_RESTORE_FUNCTS(conf3, uint8_t, uint8_bin);

int config_modify_conf3(struct amc6821_config* config)
{
  return _config_modify_conf(config, 2, amc6821_load_conf3, amc6821_store_conf3);
}

CONFIG_RW_GET_RESTORE_FUNCTS(conf4, uint8_t, uint8_bin);

int config_modify_conf4(struct amc6821_config* config)
{
  return _config_modify_conf(config, 3, amc6821_load_conf4, amc6821_store_conf4);
}

CONFIG_RW_GET_RESTORE_FUNCTS(dcy, uint8_t, uint8);

int _config_set_dcy(struct amc6821_config* config, const char* functname, int (*store_funct)(struct amc6821_device*, uint8_t *const))
{
  uint8_t dcy;
  int ret;

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Missing DCY value!\n",functname);
    return -1;
  }

  if(sscanf(config->pbuf, "%" SCNu8, &dcy) != 1) {
    fprintf(stderr,"%s: Error: DCY value '%s' is invalid!\n",functname,config->pbuf);
    return -2;
  }

  if((ret=store_funct(&config->dev, &dcy))) {
    fprintf(stderr, "Error: amc6821_store (%i)\n", ret);
    return -3;
  }

  return 0;
}

int config_set_dcy(struct amc6821_config* config)
{
  return _config_set_dcy(config, CFUNCTSTR, amc6821_store_dcy);
}

CONFIG_RW_GET_RESTORE_FUNCTS(dcy_low_temp, uint8_t, uint8);

int config_set_dcy_low_temp(struct amc6821_config* config)
{
  return _config_set_dcy(config, CFUNCTSTR, amc6821_store_dcy_low_temp);
}

int _config_set_fan_control(struct amc6821_config* config, const char* functname, int (*store_funct)(struct amc6821_device*, struct fan_control_data *const))
{
  struct fan_control_data fcd;
  uint8_t slope;
  int ret;

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Missing low temperature value!\n",functname);
    return -1;
  }

  if(sscanf(config->pbuf, "%" SCNu8, &fcd.low_temp) != 1) {
    fprintf(stderr,"%s: Error: Low temperature value '%s' is invalid!\n",functname,config->pbuf);
    return -2;
  }

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Missing slope value!\n",functname);
    return -1;
  }

  if(sscanf(config->pbuf, "%" SCNu8, &slope) != 1) {
    fprintf(stderr,"%s: Error: Slope value '%s' is invalid!\n",functname,config->pbuf);
    return -2;
  }

  switch(slope) {
    case 2:
      fcd.slope=fan_control_slope_2_dcy_per_C;
      break;
    case 4:
      fcd.slope=fan_control_slope_4_dcy_per_C;
      break;
    case 8:
      fcd.slope=fan_control_slope_8_dcy_per_C;
      break;
    case 16:
      fcd.slope=fan_control_slope_16_dcy_per_C;
      break;
    case 32:
      fcd.slope=fan_control_slope_32_dcy_per_C;
      break;
    default:
      fprintf(stderr,"%s: Error: Slope value '%u' is invalid!\n",functname,slope);
      return -3;
  }

  if((ret=store_funct(&config->dev, &fcd))) {
    fprintf(stderr, "Error: amc6821_store (%i)\n", ret);
    return -3;
  }

  return 0;
}

CONFIG_RW_GET_RESTORE_FUNCTS(local_temp_fan_control, struct fan_control_data, fan_control);

int config_set_local_temp_fan_control(struct amc6821_config* config)
{
  return _config_set_fan_control(config, CFUNCTSTR, amc6821_store_local_temp_fan_control);
}

CONFIG_RW_GET_RESTORE_FUNCTS(remote_temp_fan_control, struct fan_control_data, fan_control);

int config_set_remote_temp_fan_control(struct amc6821_config* config)
{
  return _config_set_fan_control(config, CFUNCTSTR, amc6821_store_remote_temp_fan_control);
}

CONFIG_RW_GET_RESTORE_FUNCTS(dcy_ramp, struct dcy_ramp_data, dcy_ramp);

int config_set_dcy_ramp(struct amc6821_config* config)
{
  struct dcy_ramp_data drd;
  uint8_t ubuf;
  float fbuf;
  int ret;

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: enabled/disabled status is missing!\n",CFUNCTSTR);
    return -1;
  }

  if(!argsdiffer(config->pbuf, "enabled")) drd.enabled=true;

  else if(!argsdiffer(config->pbuf, "disabled")) drd.enabled=false;

  else {
    fprintf(stderr,"%s: Error: Status '%s' is invalid!\n",CFUNCTSTR,config->pbuf);
    return -2;
  }

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Step value is missing!\n",CFUNCTSTR);
    return -3;
  }

  if(sscanf(config->pbuf, "%" SCNu8, &ubuf) != 1) {
    fprintf(stderr,"%s: Error: Step value '%s' is invalid!\n",CFUNCTSTR,config->pbuf);
    return -4;
  }

  switch(ubuf) {
    case 1:
      drd.step=dcy_ramp_step_1_dcy;
      break;
    case 2:
      drd.step=dcy_ramp_step_2_dcy;
      break;
    case 4:
      drd.step=dcy_ramp_step_4_dcy;
      break;
    case 8:
      drd.step=dcy_ramp_step_8_dcy;
      break;
    default:
      fprintf(stderr,"%s: Error: Step value '%u' is invalid!\n",CFUNCTSTR,ubuf);
      return -5;
  }

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Rate value is missing!\n",CFUNCTSTR);
    return -6;
  }

  if(sscanf(config->pbuf, "%f", &fbuf) != 1 || (int)(fbuf*16) != fbuf*16) {
    fprintf(stderr,"%s: Error: Rate value '%s' is invalid!\n",CFUNCTSTR,config->pbuf);
    return -7;
  }

  switch((int)(fbuf*16)) {
    case 1:
      drd.rate=dcy_ramp_rate_1_16_s;
      break;
    case 2:
      drd.rate=dcy_ramp_rate_1_8_s;
      break;
    case 4:
      drd.rate= dcy_ramp_rate_1_4_s;
      break;
    case 8:
      drd.rate=dcy_ramp_rate_1_2_s;
      break;
    case 16:
      drd.rate=dcy_ramp_rate_1_s;
      break;
    case 32:
      drd.rate=dcy_ramp_rate_2_s;
      break;
    case 64:
      drd.rate=dcy_ramp_rate_4_s;
      break;
    case 128:
      drd.rate=dcy_ramp_rate_8_s;
      break;
    default:
      fprintf(stderr,"%s: Error: Rate value '%f' is invalid!\n",CFUNCTSTR,fbuf);
      return -8;
  }

  if(getnextparam(&config->a_h,true,config->pbuf,AMC_PBUF_SIZE)<0) {
    fprintf(stderr,"%s: Error: Threshold value is missing!\n",CFUNCTSTR);
    return -9;
  }

  if(sscanf(config->pbuf, "%" SCNu8, &ubuf) != 1) {
    fprintf(stderr,"%s: Error: Threshold value '%s' is invalid!\n",CFUNCTSTR,config->pbuf);
    return -10;
  }

  switch(ubuf) {
    case 1:
      drd.threshold=dcy_ramp_thresh_1_dcy;
      break;
    case 2:
      drd.threshold=dcy_ramp_thresh_2_dcy;
      break;
    case 3:
      drd.threshold=dcy_ramp_thresh_3_dcy;
      break;
    case 4:
      drd.threshold=dcy_ramp_thresh_4_dcy;
      break;
    default:
      fprintf(stderr,"%s: Error: Threshold value '%u' is invalid!\n",CFUNCTSTR,ubuf);
      return -11;
  }

  if((ret=amc6821_store_dcy_ramp(&config->dev, &drd))) {
    fprintf(stderr, "Error: amc6821_store (%i)\n", ret);
    return -12;
  }

  return 0;
}

CONFIG_RO_GET_FUNCT(tach, uint16_t, tach);
CONFIG_RO_GET_FUNCT(local_temp_low_res, int8_t, temp_low_res);
CONFIG_RO_GET_FUNCT(remote_temp_low_res, int8_t, temp_low_res);

int config_exit(struct amc6821_config* config)
{
  return CONFIG_EXIT_RET;
}

void config_ht_populate(struct amc6821_config* config)
{
  HT_SET_FUNCT(config);

  HT_SET_FUNCT(help);
  HT_SET_FUNCT(olog);
  HT_SET_FUNCT(elog);
  HT_SET_FUNCT(prompt);

  HT_SET_FUNCT(open);
  HT_SET_FUNCT(mux_open);

  HT_GET_MOD_FUNCTS(conf1);
  HT_GET_MOD_FUNCTS(conf2);
  HT_GET_MOD_FUNCTS(conf3);
  HT_GET_MOD_FUNCTS(conf4);

  HT_GET_SET_FUNCTS(dcy);
  HT_GET_SET_FUNCTS(dcy_low_temp);
  HT_GET_SET_FUNCTS(local_temp_fan_control);
  HT_GET_SET_FUNCTS(remote_temp_fan_control);
  HT_GET_SET_FUNCTS(dcy_ramp);

  HT_GET_FUNCT(tach);
  HT_GET_FUNCT(local_temp_low_res);
  HT_GET_FUNCT(remote_temp_low_res);

  HT_SET_FUNCT(exit);
}

void config_htc1_populate(struct amc6821_config* config)
{
  HTC1_SET(THERM_INT_EN);
  HTC1_SET(FDRC1);
  HTC1_SET(FDRC0);
  HTC1_SET(FAN_FAULT_EN);
  HTC1_SET(PWM_INVERT);
  HTC1_SET(RPM_INT_EN);
  HTC1_SET(GLOBAL_INT_EN);
  HTC1_SET(START_MONITOR);
  
  HTC1_SET(NO_THERM_INT_EN);
  HTC1_SET(NO_FDRC1);
  HTC1_SET(NO_FDRC0);
  HTC1_SET(NO_FAN_FAULT_EN);
  HTC1_SET(NO_PWM_INVERT);
  HTC1_SET(NO_RPM_INT_EN);
  HTC1_SET(NO_GLOBAL_INT_EN);
  HTC1_SET(NO_START_MONITOR);
}

void config_htc2_populate(struct amc6821_config* config)
{
  HTC2_SET(RESET);
  HTC2_SET(LPSV_INT_EN);
  HTC2_SET(RT_INT_EN);
  HTC2_SET(LT_INT_EN);
  HTC2_SET(REMOTE_FAIL_INT_EN);
  HTC2_SET(TACH_EN);
  HTC2_SET(TACH_MODE);
  HTC2_SET(PWM_OUT_EN);

  HTC2_SET(NO_RESET);
  HTC2_SET(NO_LPSV_INT_EN);
  HTC2_SET(NO_RT_INT_EN);
  HTC2_SET(NO_LT_INT_EN);
  HTC2_SET(NO_REMOTE_FAIL_INT_EN);
  HTC2_SET(NO_TACH_EN);
  HTC2_SET(NO_TACH_MODE);
  HTC2_SET(NO_PWM_OUT_EN);
}

void config_htc3_populate(struct amc6821_config* config)
{
  HTC3_SET(THERM_FAN_EN);

  HTC3_SET(NO_THERM_FAN_EN);
}

void config_htc4_populate(struct amc6821_config* config)
{
  HTC4_SET(PULSE_NUMBER);
  HTC4_SET(TACH_FAST);
  HTC4_SET(OVR_PIN_EN);

  HTC4_SET(NO_PULSE_NUMBER);
  HTC4_SET(NO_TACH_FAST);
  HTC4_SET(NO_OVR_PIN_EN);
}

void config_clear(struct amc6821_config* config)
{
  close(config->oout);
  close(config->eout);
  dup2(config->oldstdout,1);
  dup2(config->oldstderr,2);
  close(config->oldstdout);
  close(config->oldstderr);

  args_terminate(&config->a_h);
  ht_destroy(config->ht);
  ht_destroy(config->htcs[0]);
  ht_destroy(config->htcs[1]);
  ht_destroy(config->htcs[2]);
  ht_destroy(config->htcs[3]);
}

int amc6821_config(struct amc6821_config* config, int nargs, const char** args)
{
  args_init(&config->a_h, nargs, args);

  config->plength=1;

  config->oout=1; //stdout==1
  config->eout=2; //stderr==2
  
  config->interactive=false;
 
  config->ht=ht_create();
  config->htcs[0]=ht_create();
  config->htcs[1]=ht_create();
  config->htcs[2]=ht_create();
  config->htcs[3]=ht_create();
  config_ht_populate(config);
  config_htc1_populate(config);
  config_htc2_populate(config);
  config_htc3_populate(config);
  config_htc4_populate(config);

  config->oldstdout=dup(1);
  config->oldstderr=dup(2);

  int (*cfunct)(struct amc6821_config* config);

  while(config->plength>0) {

    while((config->plength=getnextparam(&config->a_h,false,config->pbuf,AMC_PBUF_SIZE))>0) {
      cfunct=(int (*)(struct amc6821_config* config))ht_get(config->ht, config->pbuf);

      if(cfunct) {

	if(cfunct(config)) {
	  fprintf(stderr,"%s: Error: Invalid command!\n",CFUNCTSTR);
	  config_clear(config);
	  return 1;
	}

      } else {
	fprintf(stderr,"%s: Error: Unknown parameter: '%s'\n",CFUNCTSTR,config->pbuf);
	config_clear(config);
	return 1;
      }
    }

    //Assertion: We are ready to perform a task whether a break statement has
    //been reached in the inner loop or plength<=0. Otherwise the value of
    //plength can be assessed and appropriate actions can be taken

    //All the parameters for the current job have been read, the job can now be
    //performed. Do it here

    //If reading argument from the standard input (interactive mode), we
    //indicate that the job is completed through the standard output
    if(config->interactive) {
      write(config->oldstderr,"@DONE\n",6);
      config->interactive=false;
    }
    fflush(stdout);
    fflush(stderr);
    //Continue reading other commands for the next job
  }

  config_clear(config);
  return 0;
}

