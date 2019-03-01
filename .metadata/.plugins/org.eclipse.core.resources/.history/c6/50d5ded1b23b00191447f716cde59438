#include "xil_uio.h"

// Constructor (without allocation)
void xil_uio__init(xil_uio* self, const char *_uio_dev, const uint32_t _uio_size) {
    self->mapSize = _uio_size;
	self->fd = open(_uio_dev, O_RDWR);
	if (self->fd < 1) {
		printf("Error opening : %s \n", _uio_dev );
	    return;
	}
	self->virtAddr = (void *)mmap(NULL, _uio_size,
			PROT_READ | PROT_WRITE,	MAP_SHARED, self->fd, 0);
    if (self->fd < 1) {
		printf("Error mapping : %s \n", _uio_dev );
	    return;
	}
    return;
}

// Allocation + initialization (equivalent to "new uio(arg )")
xil_uio* xil_uio__create(const char *_uio_dev, const uint32_t _uio_size){
    xil_uio* result = (xil_uio*) malloc(sizeof(xil_uio));
    xil_uio__init(result, _uio_dev, _uio_size);
    return result;
}

// Destructor (without deallocation)
void xil_uio__reset(xil_uio* self) {
    if (self->fd < 0 ) {
		printf("/dev/mem hasn't been openned");
    }else{
    	munmap(self->virtAddr, self->mapSize);
        close(self->fd);
    }
}

// Destructor + deallocation (equivalent to "delete xil_uio")
void xil_uio__destroy(xil_uio* _xil_uio) {
  if (_xil_uio) {
     xil_uio__reset(_xil_uio);
     free(_xil_uio);
  }
}

// Read data from uio device
uint32_t xil_uio__read32(xil_uio *self, const uint32_t offset){	
	uint32_t read_result = *((volatile uint32_t *) (self->virtAddr + offset));
	return read_result;
}

// Write data to  uio device
void 	xil_uio__write32 (xil_uio *self, const uint32_t offset, const uint32_t writeval){
	 *((volatile uint32_t *) (self->virtAddr + offset)) = writeval;
}

uint32_t xil_uio__mapSize(xil_uio* self){ return(self->mapSize);}
uint32_t xil_uio__fd(xil_uio* self){return(self->fd);}
void*    xil_uio__virtAddr(xil_uio* self){return(self->virtAddr);}
