################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/model/dtrsmu.c \
../src/model/restor.c \
../src/model/setg1.c \
../src/model/setg3.c 

OBJS += \
./src/model/dtrsmu.o \
./src/model/restor.o \
./src/model/setg1.o \
./src/model/setg3.o 

C_DEPS += \
./src/model/dtrsmu.d \
./src/model/restor.d \
./src/model/setg1.d \
./src/model/setg3.d 


# Each subdirectory must supply rules for building sources it contributes
src/model/%.o: ../src/model/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


