#ifndef IMPEDANCE_V1_H
#define IMPEDANCE_V1_H

#include "Time_Meas.h"

	//Constants used in some of the functions below
#define AUTOMATIC	1
#define MANUAL		0
#define DIRECT		0
#define REVERSE		1
#define LIMITED		1
#define NONLIMITED	0

class Impedance
{

public:

	//commonly used functions **************************************************************************
	Impedance(double*, double*, double*, double*, double*,	// * Constructor.  links the Impedance controller to the Input,
		double, double, double, int);	//   Output, and Setpoint.  Initial tuning parameters M B and K*/

	void SetMode(int Mode);					// * Sets Impedance Controller to either Manual (0) or Auto (non-0)

	bool Compute();							// * Performs the Impedance Controller action calculation.
											//   it should be called every time loop() cycles. ON/OFF and
											//   calculation frequency can be set using SetMode
											//   SetSampleTime respectively

	//available but not commonly used functions ********************************************************
	void SetTunings(double, double,			// * While most users will set the tunings once in the 
		double);         					//   constructor, this function gives the user the option
											//   of changing tunings during runtime for Adaptive control

	void SetControllerDirection(int);		// * Sets the Direction, or "Action" of the controller. DIRECT
											//   means the output will increase when error is positive. REVERSE
											//   means the opposite.  it's very unlikely that this will be needed
											//   once it is set in the constructor.
	
	void SetSampleTime(int);				// * Sets the frequency, in Milliseconds, with which 
											//   the PID calculation is performed.  default is 100

	//Display functions ****************************************************************
	double GetM();							//	These functions query the Impedance Controller for interal values.
	double GetB();							
	double GetK();			
	double Get_xp();
	int GetMode();							
	int GetDirection();						

private:
	void Initialize();

	double dispM;				// * we'll hold on to the tuning parameters in user-entered 
	double dispB;				//   format for display purposes
	double dispK;				//

	double myM;                  // * M - Inertia Component
	double myB;                  // * B - Energy dissipation term from the viscous damper
	double myK;                  // * K - Stiffness of the equivalent system

	int controllerDirection;
	
	double x;
	double xp;
	double xpp;

	double *myInput;              // * Pointers to the Input, Output, and Setpoint variables
	double *myOutput;
	double *mySetpoint;

	double *my_x;
	double *my_xp;

	unsigned long lastTime;
	double last_xpp, last_xp, last_x;

	unsigned long SampleTime;
	double SampleTimeInSec;
	double outMin, outMax;
	bool inAuto;
};
#endif

