#ifndef BUTTER_FILT_
#define BUTTER_FILT_

#include <stdint.h>

#include "Time_Meas.h"
#include "definitions.h"

#include "definitions.h"

class butter{
public:
	butter();
	butter(_real*, _real*);

	void Initialize(_real*, _real*);
	bool Compute();

	void Configure(const uint32_t, const _real, const _real);

	uint64_t lastTime, SampleTime;
private:

	void Reset();

	_real a0, b01;
	_real y_ant, x_ant;
	_real *my_y, *my_x;



};

#endif /* SRC_DATA_MEM_H_ */
