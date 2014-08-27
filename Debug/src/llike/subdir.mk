################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/llike/consrv.c \
../src/llike/constp.c \
../src/llike/setcoe.c \
../src/llike/setcoe_calc_scaling.c \
../src/llike/setcoe_obs_loop.c \
../src/llike/setcoe_setup_indices.c \
../src/llike/setlik.c \
../src/llike/setobj.c 

OBJS += \
./src/llike/consrv.o \
./src/llike/constp.o \
./src/llike/setcoe.o \
./src/llike/setcoe_calc_scaling.o \
./src/llike/setcoe_obs_loop.o \
./src/llike/setcoe_setup_indices.o \
./src/llike/setlik.o \
./src/llike/setobj.o 

C_DEPS += \
./src/llike/consrv.d \
./src/llike/constp.d \
./src/llike/setcoe.d \
./src/llike/setcoe_calc_scaling.d \
./src/llike/setcoe_obs_loop.d \
./src/llike/setcoe_setup_indices.d \
./src/llike/setlik.d \
./src/llike/setobj.d 


# Each subdirectory must supply rules for building sources it contributes
src/llike/%.o: ../src/llike/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


