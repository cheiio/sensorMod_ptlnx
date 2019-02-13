#ifndef SENSORMOD_H
#define SENSORMOD_H

/****************** Include Files ********************/

#include "xil_uio.h"

#define OFFSET_POS      0x02
#define OFFSET_CURR     0x04
#define OFFSET_VOLT     0x06

#define OFFSET_POS_F    0x02
#define OFFSET_VEL_F    0x04
#define OFFSET_CURR_F   0x06

#define OFFSET_DT       0x00
#define OFFSET_R        0x08
#define OFFSET_CEN_CURR 0x16
#define OFFSET_Q0       0x18
#define OFFSET_Q12      0x20
#define OFFSET_Q3       0x28
#define OFFSET_b01      0x30
#define OFFSET_a1       0x38

#define OFFSET_FILT     0x00
#define OFFSET_SEN      0x40
#define OFFSET_CONF     0x80


#define Nmodules		7

/**************************** Type Definitions *****************************/
union sensorMod_data_t {
	        uint32_t 	_uint32;
	        float 		_float32;
        };
typedef struct sensorMod_struct{
	xil_uio *sensorMod_handler;

	sensorMod_data_t 	filt_pos[Nmodules],
						filt_vel[Nmodules],
						filt_curr[Nmodules];

	sensorMod_data_t center_current;
	sensorMod_data_t dt, R, Q[3];
	sensorMod_data_t ab[2];
}sensorMod;

/************************** Driver Class Definition ************************/

//***********************************************  Constructor
sensorMod* sensorMod__create(const char *_uio_dev,
		const uint32_t _uio_size);
void sensorMod__destroy(sensorMod* self);

//***********************************************  Calculating Functions
void sensorMod_start(sensorMod* self, sensorMod_data_t* dataIn);
void sensorMod_start(sensorMod* self, float* dataIn);
uint32_t sensorMod_isReady(sensorMod* self);
sensorMod_data_t *sensorMod_get_filteredData(sensorMod* self);
float *sensorMod_get_filteredData(sensorMod* self);

//***********************************************  Configuration Functions
void sensorMod__set_centerCurr(sensorMod* self, float centerCurr);
void sensorMod__set_Kalman(sensorMod* self, float dt, float R,
											float* Q, float* ab);
void sensorMod__set_butter(sensorMod* self, float* ab);

//***********************************************  Variable Functions
float sensorMod__get_centerCurr(sensorMod* self);
float sensorMod__get_dt(sensorMod* self);
float sensorMod__get_R(sensorMod* self);
float* sensorMod__get_Q(sensorMod* self);
float* sensorMod__get_ab(sensorMod* self);

#endif // SENSORMOD_H
