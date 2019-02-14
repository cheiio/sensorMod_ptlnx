#include "SensorMod.h"

//***********************************************  Constructor

// Constructor (without allocation)
void sensorMod__init(sensorMod* self, const char *_uio_dev, const uint32_t _uio_size){
    self->sensorMod_handler = xil_uio__create(uio_dev, SENMOD_SIZE);
    self->center_current._uint32 = xil_uio__read32(self->sensorMod_handler, 
                                OFFSET_CONF | OFFSET_CEN_CURR);
    self->dt._uint32 = xil_uio__read32(self->sensorMod_handler, 
                                OFFSET_CONF | OFFSET_DT);
    self->R._uint32 = xil_uio__read32(self->sensorMod_handler, 
                                OFFSET_CONF | OFFSET_R);
    self->Q[0]._uint32 = xil_uio__read32(self->sensorMod_handler, 
                                OFFSET_CONF | OFFSET_Q0);
    self->Q[1]._uint32 = xil_uio__read32(self->sensorMod_handler, 
                                OFFSET_CONF | OFFSET_Q12);
    self->Q[2]._uint32 = xil_uio__read32(self->sensorMod_handler, 
                                OFFSET_CONF | OFFSET_Q3);
    self->ab[0]._uint32 = xil_uio__read32(self->sensorMod_handler, 
                                OFFSET_CONF | OFFSET_a1);
    self->ab[1]._uint32 = xil_uio__read32(self->sensorMod_handler, 
                                OFFSET_CONF | OFFSET_b01);
}

// Allocation + initialization
sensorMod* sensorMod__create(const char *_uio_dev, const uint32_t _uio_size){
    sensorMod* result = (sensorMod*) malloc(sizeof(sensorMod));
    Point__init(result, _uio_dev, _uio_size);
    return result;
}

// Destructor (without deallocation)
void  sensorMod__reset(sensorMod* self){
    xil_uio__destroy(self->sensorMod_handler);
}

// Destructor + deallocation (equivalent to "delete xil_uio")
void sensorMod__destroy(sensorMod* _sensorMod){
    if (_sensorMod) {
        sensorMod__reset(_sensorMod);
        free(_sensorMod);
    }
}

//***********************************************  Calculating Functions
void sensorMod_start(sensorMod* self,
		sensorMod_data_t* posIn, sensorMod_data_t* currIn, sensorMod_data_t* voltIn){
	const uint8_t empty_8bit = 0;
	uint32_t data_n = 0;
	for(uint8_t mod=1; mod <= Nmodules; mod++){
		xil_uio__write32(self->sensorMod_handler->virtAddr,
			empty_8bit | OFFSET_POS | OFFSET_SEN | mod<<3, posIn[data_n]->_uint32);
		xil_uio__write32(self->sensorMod_handler->virtAddr,
			empty_8bit | OFFSET_CURR | OFFSET_SEN | mod<<3, currIn[data_n]->_uint32);
		xil_uio__write32(self->sensorMod_handler->virtAddr,
			empty_8bit | OFFSET_VOLT | OFFSET_SEN | mod<<3, voltIn[data_n]->_uint32);
		data_n++;
	}
}

void sensorMod_start(sensorMod* self, float* posIn_f, float* currIn_f, float* voltIn_f){
	sensorMod_data_t *posIn._float32 = *posIn_f;
	sensorMod_data_t *currIn._float32 = *currIn_f;
	sensorMod_data_t *voltIn._float32 = *voltIn_f;
	sensorMod_start(self, posIn, currIn, voltIn);
}

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
