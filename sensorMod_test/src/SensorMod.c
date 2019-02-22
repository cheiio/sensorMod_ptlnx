#include "SensorMod.h"

//***********************************************  Constructor

// Constructor (without allocation)
void sensorMod__init(sensorMod* self, const char *_uio_dev, const uint32_t _uio_size){
    self->sensorMod_handler = xil_uio__create(_uio_dev, _uio_size);

    // Center Current
    uint32_t *cenCurr = (uint32_t *)malloc(sizeof(uint32_t *));
	*cenCurr = xil_uio__read32(self->sensorMod_handler,	OFFSET_CEN_CURR);
    self->center_current._uint32 = cenCurr;
    // dt
    uint32_t *dt = (uint32_t *)malloc(sizeof(uint32_t *));
    *dt = xil_uio__read32(self->sensorMod_handler,	OFFSET_DT);
    self->dt._uint32 = dt;
    // R
    uint32_t *R = (uint32_t *)malloc(sizeof(uint32_t *));
    *R = xil_uio__read32(self->sensorMod_handler, OFFSET_R);
    self->R._uint32 = R;
    // Q
    uint32_t* Q = (uint32_t*)malloc(4*3);
    Q[0] = xil_uio__read32(self->sensorMod_handler, OFFSET_Q0);
    Q[1] = xil_uio__read32(self->sensorMod_handler, OFFSET_Q12);
    Q[2] = xil_uio__read32(self->sensorMod_handler, OFFSET_Q3);
    self->Q._uint32 = Q;
    // ab
    uint32_t* ab = (uint32_t*)malloc(4*2);
    ab[0] = xil_uio__read32(self->sensorMod_handler, OFFSET_a1);
    ab[1] = xil_uio__read32(self->sensorMod_handler, OFFSET_b01);
    self->ab._uint32 = ab;

    // Filter
    uint32_t *pos = (uint32_t*)malloc(4*Nmodules);
    uint32_t *vel = (uint32_t*)malloc(4*Nmodules);
    uint32_t *curr = (uint32_t*)malloc(4*Nmodules);
    self->filt_pos._uint32 = pos;
    self->filt_vel._uint32 = vel;
    self->filt_curr._uint32 = curr;
}

// Allocation + initialization
sensorMod* sensorMod__create(const char *_uio_dev, const uint32_t _uio_size){
    sensorMod* result = (sensorMod*) malloc(sizeof(sensorMod));
    sensorMod__init(result, _uio_dev, _uio_size);
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
void sensorMod__start(sensorMod *self,
		sensorMod_data_t posIn, sensorMod_data_t currIn, sensorMod_data_t voltIn){

	uint32_t data_n = 0;
	for(uint32_t mod=1; mod <= Nmodules; mod++){
		posIn._uint32[data_n] = (posIn._uint32[data_n] & MASK_MOD_IN) | mod;
		xil_uio__write32(self->sensorMod_handler, OFFSET_POS, posIn._uint32[data_n]);

		currIn._uint32[data_n] = (currIn._uint32[data_n] & MASK_MOD_IN) | mod;
		xil_uio__write32(self->sensorMod_handler, OFFSET_VEL_CURR, currIn._uint32[data_n]);

		voltIn._uint32[data_n] = (voltIn._uint32[data_n] & MASK_MOD_IN) | mod;
		xil_uio__write32(self->sensorMod_handler, OFFSET_CURR_VOLT, voltIn._uint32[data_n]);

		data_n++;
	}
}

void sensorMod__start_float(sensorMod* self,
		float* posIn_f, float* currIn_f, float* voltIn_f){
	sensorMod_data_t posIn, currIn, voltIn;
    posIn._float32 = posIn_f;   currIn._float32 = currIn_f;   voltIn._float32 = voltIn_f;   
    sensorMod__start(self, posIn, currIn, voltIn);
}

void sensorMod_get_filteredData(sensorMod* self){
	for (uint32_t mod=1; mod<Nmodules; mod++){
		xil_uio__write32(self->sensorMod_handler, STATUS_R_MOD_W, mod);
		self->filt_pos._uint32[mod] = xil_uio__read32( self->sensorMod_handler, OFFSET_POS);
		self->filt_vel._uint32[mod] = xil_uio__read32( self->sensorMod_handler, OFFSET_VEL_CURR);
		self->filt_curr._uint32[mod] = xil_uio__read32( self->sensorMod_handler, OFFSET_CURR_VOLT);
	}
}

uint32_t sensorMod__isReady(sensorMod* self){
    uint32_t status = xil_uio__read32(self->sensorMod_handler, STATUS_R_MOD_W);
    if ( (status & READY_MASK) == ISREADY ){
    	sensorMod_get_filteredData(self);
    	return 1;
    }else{
    	return 0;
    }
}

void sensorMod__get_modFilt_Data(sensorMod* self, uint32_t mod, float* mod_data){
    mod_data[0] = self->filt_pos._float32[mod];
    mod_data[1] = self->filt_vel._float32[mod];
    mod_data[2] = self->filt_curr._float32[mod];
}

//***********************************************  Configuration Functions
void sensorMod__set_centerCurr(sensorMod* self, float centerCurr){
    self->center_current._float32 = &centerCurr;
    xil_uio__write32(self->sensorMod_handler, OFFSET_CEN_CURR,
    		*self->center_current._uint32);
}

void sensorMod__set_Kalman(sensorMod* self, float dt, float R, float nQ){
    self->dt._float32 = &dt;
    xil_uio__write32(self->sensorMod_handler, OFFSET_DT, *self->dt._uint32);
    self->R._float32 = &R;
    xil_uio__write32(self->sensorMod_handler, OFFSET_R, *self->R._uint32);
    self->Q._float32[0] = (nQ*dt*dt*dt*dt)/4.0f;    // (nQ*dt^4)/4
    xil_uio__write32(self->sensorMod_handler, OFFSET_Q0, self->Q._uint32[0]);
    self->Q._float32[1] = (nQ*dt*dt*dt)/2.0f;       // (nQ*dt^3)/2 
    xil_uio__write32(self->sensorMod_handler, OFFSET_Q12, self->Q._uint32[1]);
    self->Q._float32[2] = nQ*dt;                    // nQ*dt 
    xil_uio__write32(self->sensorMod_handler, OFFSET_Q3, self->Q._uint32[2]);
}

void sensorMod__set_butter(sensorMod* self, float* ab){
    self->ab._float32[0] = ab[0];
    xil_uio__write32(self->sensorMod_handler->virtAddr, OFFSET_a1, self->ab._uint32[0]);
    self->ab._float32[1] = ab[1];
    xil_uio__write32(self->sensorMod_handler->virtAddr, OFFSET_b01, self->ab._uint32[1]);
}

//***********************************************  Variable Functions
float sensorMod__get_centerCurr(sensorMod* self){return *self->center_current._float32;}
float sensorMod__get_dt(sensorMod* self){return *self->dt._float32;};
float sensorMod__get_R(sensorMod* self){return *self->R._float32;};
float* sensorMod__get_Q(sensorMod* self){return self->Q._float32;};
float* sensorMod__get_ab(sensorMod* self){return self->ab._float32;};

void sensorMod__printConfig(sensorMod*  self){
	float dt, cenCurr, R, *Q, *ab;

	dt = sensorMod__get_dt(self);
	cenCurr = sensorMod__get_centerCurr(self);
	R = sensorMod__get_R(self);
	Q = sensorMod__get_Q(self);
	ab = sensorMod__get_ab(self);

	printf("\n\nSensor Module Configuration Set \n");
	printf("\tdt = %.10e \n", dt);
	printf("\tR = %.10e \n", R);
	printf("\tQ = { %.10e , %.10e , %.10e } \n", Q[0], Q[1], Q[2]);

	printf("\tCenterCurr = %.10e \n", cenCurr);
	printf("\tab = { %.10e , %.10e } \n", ab[0], ab[1] );
}

void sensorMod__printFiltData(sensorMod*  self){

	float *sensor_filt = (float *)malloc(4*3);
	printf("\n\nFiltered Data\n");
	printf("----------------------------------------\n");
	printf( "Mod | position | velocity | current \n");
	for (uint32_t mod=0; mod<Nmodules; mod++){
		sensorMod__get_modFilt_Data(self, mod, sensor_filt);
		printf(" %d  | %f | %f | %f \n", mod, sensor_filt[0], sensor_filt[1], sensor_filt[2]);
	}
}
