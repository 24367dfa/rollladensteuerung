#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#define I2C_ADDR_CONTROLLER 0x20
#define I2C_ADDR_MANUEL     0x22

///// I2C stuff /////

/**
  * This record contains the I2C file descriptors we use in this program
  */
struct I2C_descriptors {
  int controller;
  int manuel;
} I2C_fd;

#define I2C_FD_CONTROLLER (I2C_fd.controller)
#define I2C_FD_MANUEL     (I2C_fd.manuel)

/**
  * Initialize an I2C channel to the specified address. Exits with an error
  * message if the initialization fails,
  *
  * @param addr The target address.
  * @return File Descriptor for the I2C channel
  */
int I2C_setup_fd(int addr) {
  const int fd = wiringPiI2CSetup(addr);
  if (!fd) {
    printf("Error %d on I2C initialization!", errno);
    exit(-1);
  }
  return fd;
}

/**
  * Initialize all I2C channels and store the file descriptors.
  */
void I2C_init(void) {
  I2C_fd.controller = I2C_setup_fd(I2C_ADDR_CONTROLLER);
  I2C_fd.manuel = I2C_setup_fd(I2C_ADDR_MANUEL);
}

#define I2C_ERR_INVALIDARGUMENT -2

int I2C_command(int fd, char command, char data) {
  // check parameter range
  if ((command < 0) || (command > 0x0f))
    return I2C_ERR_INVALIDARGUMENT;
  if ((data < 0) || (data > 0x0f))
    return I2C_ERR_INVALIDARGUMENT;
  
  // TODO check fd
  
    
  // build the I2C data byte
  // arguments have been checked, 
  // this cannot be negative or more than 8 bits
  const char send = (command << 4) + data; 
  
  // send command
  const char result = wiringPiI2CReadReg8(fd, send);

  // TODO check for transmission errors
  
  return result;
}

///// I3C stuff /////

void I3C_reset() {
  I2C_command(I2C_FD_MANUEL, 0x4, 0x0);
}

///// Switch states /////

#define SWITCH_UP      1
#define SWITCH_NEUTRAL 3
#define SWITCH_DOWN    2

#define SWITCH_ERR             -1
#define SWITCH_ERR_OUTOFBOUNDS -2

char read_switch_state(int idx) {
  // check parameter range
  if ((idx < 1) || (idx > 3))
    return SWITCH_ERR_OUTOFBOUNDS;

  // send the command    
  const char state = I2C_command(I2C_FD_MANUEL, 0x3, idx);
  
  // return result
  return state;  
}

int main(int argc, char *argv[]) {
  I2C_init();
    
  int run=1;
  while(run) {
    char sw = read_switch_state(0x1);
    printf("Switch status: %d\n", sw);
    I3C_reset();

    if (sleep(1)) 
      exit(1);
      
//    wiringPiI2CWrite(I2C_FD_MANUEL, 0x20+(sw&0x0f));
  }
  
  return 0;
}