
/***************************** Include Files *******************************/
#include "MotorDriver.h"

void setMotor(xil_uio *motorMod, uint8_t pwmPin, double pid_Out){

	int32_t pwm_Out = (int32_t)(PWM_V * pid_Out);
	if (pwm_Out > MAX_DUTY){
		pwm_Out = MAX_DUTY;
	}else{
		if (pwm_Out < -MAX_DUTY){
			pwm_Out = -MAX_DUTY;
		}
	}
	uint32_t pwm_AXI = ((uint32_t)pwm_Out << PWM_OUT_LSB) | ((uint32_t)pwmPin & PWMPIN_MASK);
	xil_uio__write32( motorMod , 0, pwm_AXI );

}

void disableMotors(xil_uio *motorMod){
	for (uint8_t i=0; i<NMOTORS; i++){
		setMotor(motorMod, i, (uint32_t)0);
	}
}

/************************** Function Definitions ***************************/ 
