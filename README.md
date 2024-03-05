# amc6821_controller
Utility to interact with an AMC6821 Fan Controller through I2C

Depends on https://github.com/pldrouin/libht and
https://github.com/pldrouin/libargs .

To compile, the LIBHTDIR and LIBARGSDIR variables (through environment or make) must respectively
point to the installation directories of the two dependencies.

On a LX2160A system, an automatic mode can be configured using
examples/lx2160a_auto.conf . This automatic mode can be configured, and the fan
duty cycle, fan RPM as well as the local and remote temperatures can be queried.
For example, if the fan controller slave address is 0x18,
the mux slave's address is 0x77 and the fan controller is selected on the mux using
register value 0x9:

./bin/amc6821 --mux_open /dev/iic0 0x18 0x77 0x9 0xF --config examples/lx2160a_auto.conf --get_dcy --get_tach --get_local_temp_low_res --get_remote_temp_low_res
