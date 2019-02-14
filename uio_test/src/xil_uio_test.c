#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "xil_uio.h"

#define XIL_GPIO_0_BASEADDR 	0x41210000
#define XIL_GPIO_LED_BASEADDR	0x41200000
#define XIL_SIZE				0x10000

#define XIL_GPIO_1_OFFSET		0x0
#define XIL_GPIO_1_TRI_OFFSET	0x4
#define XIL_GPIO_2_OFFSET		0x8
#define XIL_GPIO_2_TRI_OFFSET	0xC

#define MAP_SIZE 0x4000000

const char *uio_dev_0 = "/dev/uio2"; // switches
const char *uio_dev_1 = "/dev/uio3"; // shields leds

int main(int argc, char *argv[]) {

	printf("Start \n");

    // --------------------------------------------- Create handlers
    xil_uio *gpioSw_handler;
    gpioSw_handler = xil_uio__create(uio_dev_0, XIL_SIZE);

	// --------------------------------------------- Switch Read
    xil_uio__write32(gpioSw_handler, XIL_GPIO_2_TRI_OFFSET, 0xFF); // set as input
    unsigned data = xil_uio__read32(gpioSw_handler, XIL_GPIO_2_OFFSET);

	// --------------------------------------------- LED write
    xil_uio *gpioLED_handler;
    gpioLED_handler = xil_uio__create(uio_dev_1, XIL_SIZE);
//    xil_uio__write32(gpioLED_handler, XIL_GPIO_2_TRI_OFFSET, 0x00); // set as output
//    xil_uio__write32(gpioLED_handler, XIL_GPIO_2_OFFSET, data);

    // --------------------------------------------- Destroy handlers

	printf("Value on Switch-LED : %d", data);

    xil_uio__destroy(gpioLED_handler);
    xil_uio__destroy(gpioLED_handler);

    return 0;
}

