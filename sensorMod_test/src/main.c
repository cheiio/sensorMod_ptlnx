#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "data_mem.h"
#include "definitions.h"

#include "xil_uio.h"

#include "SensorMod.h"
#include "Impedance_v1.h"
#include "MotorDriver.h"

#define XIL_SIZE				0x10000

#define XIL_GPIO_1_OFFSET		0x0
#define XIL_GPIO_1_TRI_OFFSET	0x4
#define XIL_GPIO_2_OFFSET		0x8
#define XIL_GPIO_2_TRI_OFFSET	0xC

const char *uio_motorMod 	= "/dev/uio0"; // motor mod
const char *uio_sensorMod 	= "/dev/uio1"; // sensor mod
const char *uio_dev_0 		= "/dev/uio2"; // switches
const char *uio_dev_1 		= "/dev/uio3"; // shields leds

const uint32_t  nInd = 0, nInd2 = 5, nThumb = 4, nThumb2 = 6,
                nMid = 1, nRing = 2, nLit = 3;

int main(int argc, char *argv[]) {

	printf("Start \n");

    // --------------------------------------------- Create handlers
    xil_uio *gpioSw_handler, *gpioLED_handler, *motors_handler;
    gpioSw_handler = xil_uio__create(uio_dev_0, XIL_SIZE);
    gpioLED_handler = xil_uio__create(uio_dev_1, XIL_SIZE);
    motors_handler = xil_uio__create(uio_motorMod, XIL_SIZE);

    disableMotors(motors_handler);

	// --------------------------------------------- Switch Read
    xil_uio__write32(gpioSw_handler, XIL_GPIO_1_TRI_OFFSET, 0xFF); // set as input
    xil_uio__write32(gpioSw_handler, XIL_GPIO_2_TRI_OFFSET, 0xFF); // set as input
    unsigned data = xil_uio__read32(gpioSw_handler, XIL_GPIO_2_OFFSET);

	// --------------------------------------------- LED write
    xil_uio__write32(gpioLED_handler, XIL_GPIO_2_TRI_OFFSET, 0x00); // set as output
    xil_uio__write32(gpioLED_handler, XIL_GPIO_2_OFFSET, data);

    // -------------------------------------------------------
    printf("Value on Switch-LED : %d", data);

	// --------------------------------------------- Create & Configure handlers for sensorMod
    sensorMod *sensor_handler;
    _real *posIn_f, *currIn_f, *voltIn_f;
    _real *pos_filt, *vel_filt, *curr_filt;
/*
	pos_filt = (float*)malloc(4*Nmodules);
    vel_filt = (float*)malloc(4*Nmodules);
    curr_filt = (float*)malloc(4*Nmodules);
    posIn_f = (float*)malloc(4*Nmodules);
    currIn_f = (float*)malloc(4*Nmodules);
    voltIn_f = (float*)malloc(4*Nmodules);
*/
    
    sensor_handler = sensorMod__create(uio_sensorMod, XIL_SIZE, posIn_f, currIn_f, voltIn_f,
    		pos_filt, vel_filt, curr_filt);
        
    //_real modInd[3], modInd2[3], modThumb[3], modThumb2[3], modMid[3], modRing[3], modLit[3];

    float dt = 5e-3;
    float R = 8.2673e-04;
    float nQ = 3;
    float *ab = (float *)malloc(4*2);
    ab[0] = -0.9450; ab[1] = 0.0275;
    float cenCurr = 36486.206;
    //sensorMod__set_Kalman(sensor_handler, dt, R, nQ);
    sensorMod__printConfig(sensor_handler);
    //float Q[3];

    // --------------------------------------------- Create & COnfigure Impedance Controllers
    const _real 	M = 0.528404343800586,
				    K = 88.6173474407601,
                    B = 9.58011226419688,
                    P = 516.001357364368,
                    outMax = 1023.00,
                    outMin = -1023.00;
    _real Setpoint[Nmodules] = {-200,-200,-200,-200,-200,-200,-200};
    
    uint32_t impMod;

    impMod = nInd;          // Index controller
    impedance *impInd = impedance__create((_real *)(curr_filt+impMod), (_real *)(pos_filt+impMod), 
        (_real *)(vel_filt+impMod), (_real *)(voltIn_f+impMod), (_real *)(Setpoint+impMod), M, B, K, DIRECT);
    
    impMod = nMid;          // Middle finger controller
    impedance *impMid = impedance__create((_real *)(curr_filt+impMod), (_real *)(pos_filt+impMod), 
        (_real *)(vel_filt+impMod), (_real *)(voltIn_f+impMod), (_real *)(Setpoint+impMod), M, B, K, DIRECT);

    impMod = nRing;         // Ring finger controller
    impedance *impRing = impedance__create((_real *)(curr_filt+impMod), (_real *)(pos_filt+impMod), 
        (_real *)(vel_filt+impMod), (_real *)(voltIn_f+impMod), (_real *)(Setpoint+impMod), M, B, K, DIRECT);

    impMod = nLit;          // Little finger controller
    impedance *impLit = impedance__create((_real *)(curr_filt+impMod), (_real *)(pos_filt+impMod), 
        (_real *)(vel_filt+impMod), (_real *)(voltIn_f+impMod), (_real *)(Setpoint+impMod), M, B, K, DIRECT);

    impMod = nThumb;        // Thumb finger controller
    impedance *impThumb = impedance__create((_real *)(curr_filt+impMod), (_real *)(pos_filt+impMod), 
        (_real *)(vel_filt+impMod), (_real *)(voltIn_f+impMod), (_real *)(Setpoint+impMod), M, B, K, DIRECT);

    impMod = nInd2;         // Index2 finger controller
    impedance *impInd2 = impedance__create((_real *)(curr_filt+impMod), (_real *)(pos_filt+impMod), 
        (_real *)(vel_filt+impMod), (_real *)(voltIn_f+impMod), (_real *)(Setpoint+impMod), M, B, K, DIRECT);

    impMod = nThumb2;       // Thumb2 finger controller
    impedance *impThumb2 = impedance__create((_real *)(curr_filt+impMod), (_real *)(pos_filt+impMod), 
        (_real *)(vel_filt+impMod), (_real *)(voltIn_f+impMod), (_real *)(Setpoint+impMod), M, B, K, DIRECT);

    // --------------------------------------------- Test Controller
    size_t n_data = sizeof(rawPos)/sizeof(*rawPos);
    printf("\nStart Filtering\n");
    printf("  |     Raw Data    ||        Filtered Data       |\n");
    printf("# |  Pos   |  Curr  ||   Pos   |   Vel   |  Curr  |\n");
	for (uint32_t i=0; i<n_data; i++){
    	for (uint32_t j=0; j<Nmodules; j++){
    		posIn_f[j]=rawPos[i];   currIn_f[j]=rawCurr[i];     voltIn_f[j]=rawVolt[i];
    	}
        // start filtering
    	sensorMod__start(sensor_handler);
    	// wait and receive data
    	sensorMod__wait(sensor_handler);

    	// print index
    	printf("%d | %.2f | %.2f || %.2f | %.2f | %.2f |\n", i, rawPos[i],rawCurr[i],
    			sensor_handler->filt_pos._float32[nInd]);
    			//pos_filt[nInd],vel_filt[nInd],curr_filt[nInd]);

        // 


    }

    // --------------------------------------------- Motor Driver Test
/*
    int32_t pwmPin = 0;
    float volt;
    printf("\nMotor Test");
    while(1){
    	printf("\n\tMotor ID Switches (-1 to get out): ");
    	scanf("%i", &pwmPin);
    	if ((pwmPin < 0) | (pwmPin > NMOTORS)) break;

		printf("\tVolt (-8 to 8) : ");
    	scanf("%f", &volt);

		setMotor(motors_handler, pwmPin, volt);
		printf("\t\tMotor %d set on %.2f", (uint8_t)pwmPin, volt);

    }
*/

    // --------------------------------------------- Destroy handlers
    xil_uio__destroy(gpioLED_handler);
    xil_uio__destroy(gpioSw_handler);
    xil_uio__destroy(motors_handler);
    sensorMod__destroy(sensor_handler);

    printf("\n End");

   return 0;
}

