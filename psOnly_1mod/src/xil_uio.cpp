#include "xil_uio.hpp"

// Constructor (without allocation)
xil_uio::xil_uio(const char *_uio_dev, const uint32_t _uio_size) {
    this->mapSize = _uio_size;
	this->fd = open(_uio_dev, O_RDWR);
	if (this->fd < 1) {
		printf("Error opening : %s \n", _uio_dev );
	    return;
	}
	this->virtAddr = (void *)mmap(NULL, _uio_size,
			PROT_READ | PROT_WRITE,	MAP_SHARED, fd, 0);
    if (this->fd < 1) {
		printf("Error mapping : %s \n", _uio_dev );
	}
}

// Read data from uio device
uint32_t xil_uio::read32(const uint32_t offset){	
	uint32_t read_result = *((volatile uint32_t *) (this->virtAddr + offset));
	return read_result;
}

// Write data to  uio device
void 	xil_uio::write32 (const uint32_t offset, const uint32_t writeval){
	 *((volatile uint32_t *) (this->virtAddr + offset)) = writeval;
}