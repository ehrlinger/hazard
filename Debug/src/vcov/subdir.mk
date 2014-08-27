################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/vcov/chlskm.c \
../src/vcov/choslv.c \
../src/vcov/dcovar.c \
../src/vcov/setdll.c 

OBJS += \
./src/vcov/chlskm.o \
./src/vcov/choslv.o \
./src/vcov/dcovar.o \
./src/vcov/setdll.o 

C_DEPS += \
./src/vcov/chlskm.d \
./src/vcov/choslv.d \
./src/vcov/dcovar.d \
./src/vcov/setdll.d 


# Each subdirectory must supply rules for building sources it contributes
src/vcov/%.o: ../src/vcov/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


