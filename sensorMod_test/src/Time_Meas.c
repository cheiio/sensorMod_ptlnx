/*
 * XTime_Meas.cpp
 *
 *  Created on: 19 de dez de 2017
 *      Author: sapmc
 */
#include "Time_Meas.h"

Time x_time;

float micros(){

	x_time = clock();

	float time_f = (float)(x_time)*(1.0f)/((float)CLOCKS_PER_SEC*1e-6f);

	return (time_f);
}

float millis(){	

	x_time = clock();

	float time_f = (float)(x_time)*(1.0f)/((float)CLOCKS_PER_SEC*1e-3f);

	return (time_f);
}

