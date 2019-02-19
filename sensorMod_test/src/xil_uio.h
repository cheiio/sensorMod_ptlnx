#ifndef XIL_UIO_H           /* prevent circular inclusions */
#define XIL_UIO_H           /* by using protection macros */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>

// Global Variables

typedef struct xil_uio_t{
    uint32_t mapSize;
	uint32_t fd;
	void*   virtAddr;
} xil_uio;

xil_uio* xil_uio__create(const char *_uio_dev,
		const uint32_t _uio_size);    			// Creates UIO handler for /dev/uio* device
void xil_uio__destroy(xil_uio* self);           // Destroys UIO handler for /dev/uio* device

uint32_t xil_uio__mapSize(xil_uio* self);     // Public method that contains mapsize
uint32_t xil_uio__fd(xil_uio* self);          // Public method that contains fd /dev/uio* file pointer
void*    xil_uio__virtAddr(xil_uio* self);    // Public method that contains virtual mem address for /dev/uio*

uint32_t xil_uio__read32(xil_uio* self, const uint32_t offset); // reads data from virtual mem address with offset
void xil_uio__write32 (xil_uio* self, const uint32_t offset, const uint32_t writeval); 
                                                                // writes data from virtual mem address with offset

#endif /* end of protection macro */
