################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Impedance_v1.c \
../src/MotorDriver.c \
../src/SensorMod.c \
../src/Time_Meas.c \
../src/main.c \
../src/xil_uio.c 

OBJS += \
./src/Impedance_v1.o \
./src/MotorDriver.o \
./src/SensorMod.o \
./src/Time_Meas.o \
./src/main.o \
./src/xil_uio.o 

C_DEPS += \
./src/Impedance_v1.d \
./src/MotorDriver.d \
./src/SensorMod.d \
./src/Time_Meas.d \
./src/main.d \
./src/xil_uio.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 Linux gcc compiler'
	arm-linux-gnueabihf-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


