################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/blackbox/blackbox.c \
../src/blackbox/hzpexmpl.c \
../src/blackbox/ibmieee.c \
../src/blackbox/xportHandler.c \
../src/blackbox/xpt2ascii.c 

OBJS += \
./src/blackbox/blackbox.o \
./src/blackbox/hzpexmpl.o \
./src/blackbox/ibmieee.o \
./src/blackbox/xportHandler.o \
./src/blackbox/xpt2ascii.o 

C_DEPS += \
./src/blackbox/blackbox.d \
./src/blackbox/hzpexmpl.d \
./src/blackbox/ibmieee.d \
./src/blackbox/xportHandler.d \
./src/blackbox/xpt2ascii.d 


# Each subdirectory must supply rules for building sources it contributes
src/blackbox/%.o: ../src/blackbox/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


