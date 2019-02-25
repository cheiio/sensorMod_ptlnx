#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H


/****************** Include Files ********************/
#include "xil_uio.h"

#define MOTORDRIVER_S00AXI_SLV_REG0_OFFSET 0
#define MOTORDRIVER_S00_AXI_SLV_REG1_OFFSET 4
#define MOTORDRIVER_S00_AXI_SLV_REG2_OFFSET 8
#define MOTORDRIVER_S00_AXI_SLV_REG3_OFFSET 12

#define MAX_DUTY 1023
#define PWM_V 127.875			// PWM/V ->		(ant 1023/8)
#define PWM_OUT_LSB 4

void setMotor(xil_uio *motorMod, uint8_t pwmPin, double pid_Out);

#endif // MOTORDRIVER_H
