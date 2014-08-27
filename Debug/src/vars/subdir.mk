################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/vars/backw.c \
../src/vars/dfast.c \
../src/vars/dqstat.c \
../src/vars/q1.c \
../src/vars/stepw.c \
../src/vars/swvari.c \
../src/vars/swvarq.c \
../src/vars/swvarx.c 

OBJS += \
./src/vars/backw.o \
./src/vars/dfast.o \
./src/vars/dqstat.o \
./src/vars/q1.o \
./src/vars/stepw.o \
./src/vars/swvari.o \
./src/vars/swvarq.o \
./src/vars/swvarx.o 

C_DEPS += \
./src/vars/backw.d \
./src/vars/dfast.d \
./src/vars/dqstat.d \
./src/vars/q1.d \
./src/vars/stepw.d \
./src/vars/swvari.d \
./src/vars/swvarq.d \
./src/vars/swvarx.d 


# Each subdirectory must supply rules for building sources it contributes
src/vars/%.o: ../src/vars/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


