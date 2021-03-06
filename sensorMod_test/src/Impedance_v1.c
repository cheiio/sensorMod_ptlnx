/**********************************************************************************************
* Arduino Impedance Controller Library - Version 1.0
* by Sergio Pertuz <sapertuz@gmail.com>
**********************************************************************************************/

#include "Impedance_v1.h"

/* restart()****************************************************************
*	does all the things that need to happen to ensure a bumpless transfer
*  from manual to automatic mode.
******************************************************************************/
void impedance__restart(impedance* self){

	self->xpp = 0;
	self->xp = 0;
	self->x = *self->my_x;

	self->last_xpp = 0;
	self->last_xp = 0;
	self->last_x = *self->my_x;
}

// Allocation + initialization
void impedance__init(impedance* self, _real* Input, _real* x_e, _real* x_ep, _real* Output, 
		_real* Setpoint, _real M, _real B, _real K, uint32_t ControllerDirection){
	
	self->myOutput = Output;
	self->myInput = Input;
	self->mySetpoint = Setpoint;

	self->my_x = x_e;
	self->my_xp = x_ep;

	self->inAuto = 0;

	self->SampleTime = 5;							//default Controller Sample Time is 0.005 seconds
	self->SampleTimeInSec = ((_real)self->SampleTime) * 0.001;

	impedance__restart(self);
	impedance__setControllerDirection(self, ControllerDirection);
	impedance__setTunings(self, M, B, K);

	self->lastTime = millis();

}

/*Constructor (...)*********************************************************
*    The Controller parameters are specified in here.
***************************************************************************/
impedance* impedance__create(_real* Input, _real* x_e, _real* x_ep, _real* Output, 
		_real* Setpoint, _real M, _real B, _real K, uint32_t ControllerDirection){

	impedance* result = (impedance*) malloc(sizeof(impedance));
    impedance__init(result, Input, x_e, x_ep, Output, Setpoint, M, B, K, ControllerDirection);
    return result;
}

// Destructor (without deallocation)
void  impedance__reset(impedance* self){
}

// Destructor + deallocation (equivalent to "delete xil_uio")
void impedance__destroy(impedance* _impedance){
    if (_impedance) {
        impedance__reset(_impedance);
        free(_impedance);
    }
}



/* Compute() **********************************************************************
*   This function should be called every time the loop executes.  the function will 
*   decide for itself whether a new Output needs to be computed. returns true when the 
*   output is computed, false when nothing has been done.
**********************************************************************************/
uint8_t impedance__compute(impedance* self){
	if (!self->inAuto) return 0;

	uint64_t now = millis();
	uint64_t timeChange = (now - self->lastTime);
	if (timeChange >= self->SampleTime){
		/*Compute all the working error variables*/
		_real input = *self->myInput;
		_real f_error = *self->mySetpoint - input;
		_real x_error = self->x - *self->my_x;
		_real xp_error = self->xp - *self->my_xp;

		/*Compute Impedance Controller Output*/

		self->xpp = self->myM*(f_error - self->myK*x_error - self->myB*xp_error);
		
		// Integration Method 1 (Filtered Integral with n=2)
		self->xp = (self->SampleTimeInSec*0.5)*(self->xpp + self->last_xpp) + self->last_xp;
		self->x = (self->SampleTimeInSec*0.5)*(self->xp + self->last_xp) + self->last_x;
		
		// Integration Method 2 (Forward Euler)
		//xp = (SampleTimeInSec)*(xpp) + last_xp;
		//x = (SampleTimeInSec)*(xp) + last_x;

		if (self->controllerDirection == DIRECT)
		*self->myOutput = self->x;
		else
		*self->myOutput = (0-self->x);
		
		/*Remember some variables for next time*/
		self->last_x = self->x;
		self->last_xp = self->xp;
		self->last_xpp = self->xpp;
		self->lastTime = now;

		return 1;
	}
	else return 0;
}

/* SetTunings(...)*************************************************************
* This function allows the controller's dynamic performance to be adjusted.
* it's called automatically from the constructor, but tunings can also
* be adjusted on the fly during normal operation
******************************************************************************/
void impedance__setTunings(impedance* self, _real M, _real B, _real K){

	if (M<0 || B<0 || K<0) return;

	self->myM = 1/M;
	self->myB = B;
	self->myK = K;

}

/* SetSampleTime(...) *********************************************************
* sets the period, in Milliseconds, at which the calculation is performed
******************************************************************************/
void impedance__setSampleTime(impedance* self, uint32_t NewSampleTime){

	if (NewSampleTime > 0){
		self->SampleTime = NewSampleTime;
		self->SampleTimeInSec = ((_real)self->SampleTime) / 1000.00;
	}
}

/* SetMode(...)****************************************************************
* Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
* when the transition from manual to auto occurs, the controller is
* automatically initialized
******************************************************************************/
void impedance__setMode(impedance* self, uint32_t Mode){

	uint8_t newAuto = (Mode == AUTOMATIC);
	if (newAuto == !self->inAuto){  /*we just went from manual to auto*/
		impedance__restart(self);
	}
	self->inAuto = newAuto;
}

/* SetControllerDirection(...)*************************************************
******************************************************************************/
void impedance__setControllerDirection(impedance* self, uint32_t Direction){

	self->controllerDirection = Direction;
}

/* Status Funcions*************************************************************
* Just because you set the Kp=-1 doesn't mean it actually happened.  these
* functions query the internal state of the PID.  they're here for display
* purposes.  this are the functions the PID Front-end uses for example
******************************************************************************/
_real impedance__getM(impedance* self) { return  1/self->myM; }
_real impedance__getB(impedance* self) { return  self->myB; }
_real impedance__getK(impedance* self) { return  self->myK; }
_real impedance__get_xp(impedance* self) { return  self->xp; }
uint32_t impedance__getMode(impedance* self) { return  self->inAuto ? AUTOMATIC : MANUAL; }
uint32_t impedance__getDirection(impedance* self) { return self->controllerDirection; }
