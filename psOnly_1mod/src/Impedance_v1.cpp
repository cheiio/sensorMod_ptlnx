/**********************************************************************************************
* Arduino Impedance Controller Library - Version 1.0
* by Sergio Pertuz <sapertuz@gmail.com>
**********************************************************************************************/

#include "Impedance_v1.hpp"


/*Constructor (...)*********************************************************
*    The Controller parameters are specified in here.
***************************************************************************/
Impedance::Impedance(double* Input, double* x_e, double* x_ep, double* Output, double* Setpoint,
	double M, double B, double K, int ControllerDirection){

	myOutput = Output;
	myInput = Input;
	mySetpoint = Setpoint;

	my_x = x_e;
	my_xp = x_ep;

	inAuto = false;

	SampleTime = 5;							//default Controller Sample Time is 0.005 seconds
	SampleTimeInSec = ((double)SampleTime) * 0.001;

	xpp = 0;
	xp = 0;
	x = *my_x;

	last_xpp = 0;
	last_xp = 0;
	last_x = *my_x;

	Impedance::SetControllerDirection(ControllerDirection);
	Impedance::SetTunings(M, B, K);

	lastTime = millis() - SampleTime;
}


/* Compute() **********************************************************************
*   This function should be called every time "void loop()" executes.  the function will 
*   decide for itself whether a new Output needs to be computed. returns true when the 
*   output is computed, false when nothing has been done.
**********************************************************************************/
bool Impedance::Compute(){
	if (!inAuto) return false;

	unsigned long now = millis();
	unsigned long timeChange = (now - lastTime);
	if (timeChange >= SampleTime){
		/*Compute all the working error variables*/
		double input = *myInput;
		double f_error = *mySetpoint - input;
		double x_error = x - *my_x;
		double xp_error = xp - *my_xp;

		/*Compute Impedance Controller Output*/

		xpp = myM*(f_error - myK*x_error - myB*xp_error);
		
		// Integration Method 1 (Filtered Integral with n=2)
		xp = (SampleTimeInSec*0.5)*(xpp + last_xpp) + last_xp;
		x = (SampleTimeInSec*0.5)*(xp + last_xp) + last_x;
		
		// Integration Method 2 (Forward Euler)
		//xp = (SampleTimeInSec)*(xpp) + last_xp;
		//x = (SampleTimeInSec)*(xp) + last_x;

		if (controllerDirection == DIRECT)
		*myOutput = x;
		else
		*myOutput = (0-x);
		
		/*Remember some variables for next time*/
		last_x = x;
		last_xp = xp;
		last_xpp = xpp;
		lastTime = now;

		return true;
	}
	else return false;
}

/* SetTunings(...)*************************************************************
* This function allows the controller's dynamic performance to be adjusted.
* it's called automatically from the constructor, but tunings can also
* be adjusted on the fly during normal operation
******************************************************************************/
void Impedance::SetTunings(double M, double B, double K){

	if (M<0 || B<0 || K<0) return;

	dispM = M; dispB = B; dispK = K;

	myM = 1/M;
	myB = B;
	myK = K;

}

/* SetSampleTime(...) *********************************************************
* sets the period, in Milliseconds, at which the calculation is performed
******************************************************************************/
void Impedance::SetSampleTime(int NewSampleTime){

	if (NewSampleTime > 0){
		SampleTime = (unsigned long)NewSampleTime;
		SampleTimeInSec = ((double)SampleTime) / 1000;
	}
}

/* SetMode(...)****************************************************************
* Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
* when the transition from manual to auto occurs, the controller is
* automatically initialized
******************************************************************************/
void Impedance::SetMode(int Mode){

	bool newAuto = (Mode == AUTOMATIC);
	if (newAuto == !inAuto){  /*we just went from manual to auto*/
		Impedance::Initialize();
	}
	inAuto = newAuto;
}

/* Initialize()****************************************************************
*	does all the things that need to happen to ensure a bumpless transfer
*  from manual to automatic mode.
******************************************************************************/
void Impedance::Initialize(){

	xpp = 0;
	xp = 0;
	x = *my_x;

	last_xpp = 0;
	last_xp = 0;
	last_x = *my_x;
}

/* SetControllerDirection(...)*************************************************
******************************************************************************/
void Impedance::SetControllerDirection(int Direction){

	controllerDirection = Direction;
}

/* Status Funcions*************************************************************
* Just because you set the Kp=-1 doesn't mean it actually happened.  these
* functions query the internal state of the PID.  they're here for display
* purposes.  this are the functions the PID Front-end uses for example
******************************************************************************/
double Impedance::GetM() { return  dispM; }
double Impedance::GetB() { return  dispB; }
double Impedance::GetK() { return  dispK; }
double Impedance::Get_xp() { return  xp; }
int Impedance::GetMode() { return  inAuto ? AUTOMATIC : MANUAL; }
int Impedance::GetDirection() { return controllerDirection; }
