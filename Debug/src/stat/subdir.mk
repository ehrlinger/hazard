################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/stat/normal.c \
../src/stat/pdlike.c 

OBJS += \
./src/stat/normal.o \
./src/stat/pdlike.o 

C_DEPS += \
./src/stat/normal.d \
./src/stat/pdlike.d 


# Each subdirectory must supply rules for building sources it contributes
src/stat/%.o: ../src/stat/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


