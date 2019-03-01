#ifndef XIL_UIO_H           /* prevent circular inclusions */
#define XIL_UIO_H           /* by using protection macros */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>

// Global Variables

class xil_uio {
	public:
		xil_uio(const char *_uio_dev, const uint32_t _uio_size);	// Creates UIO handler for /dev/uio* device

		uint32_t read32(const uint32_t offset); 	// reads data from virtual mem address with offset
		void write32 (const uint32_t offset, const uint32_t writeval); 
                                                                // writes data from virtual mem address with offset
	private:
		uint32_t mapSize;
		uint32_t fd;
		void*   virtAddr;

};
   

#endif /* end of protection macro */
