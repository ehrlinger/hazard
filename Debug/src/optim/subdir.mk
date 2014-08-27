################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/optim/bfgsfa.c \
../src/optim/cndest.c \
../src/optim/dnmr2.c \
../src/optim/dqdot.c \
../src/optim/fdgrad.c \
../src/optim/fdhess.c \
../src/optim/ihesfa.c \
../src/optim/jacrot.c \
../src/optim/linesr.c \
../src/optim/qrupdt.c \
../src/optim/rquad.c \
../src/optim/setopt.c \
../src/optim/setoptim.c \
../src/optim/uminck.c \
../src/optim/umstop.c \
../src/optim/umstp0.c 

OBJS += \
./src/optim/bfgsfa.o \
./src/optim/cndest.o \
./src/optim/dnmr2.o \
./src/optim/dqdot.o \
./src/optim/fdgrad.o \
./src/optim/fdhess.o \
./src/optim/ihesfa.o \
./src/optim/jacrot.o \
./src/optim/linesr.o \
./src/optim/qrupdt.o \
./src/optim/rquad.o \
./src/optim/setopt.o \
./src/optim/setoptim.o \
./src/optim/uminck.o \
./src/optim/umstop.o \
./src/optim/umstp0.o 

C_DEPS += \
./src/optim/bfgsfa.d \
./src/optim/cndest.d \
./src/optim/dnmr2.d \
./src/optim/dqdot.d \
./src/optim/fdgrad.d \
./src/optim/fdhess.d \
./src/optim/ihesfa.d \
./src/optim/jacrot.d \
./src/optim/linesr.d \
./src/optim/qrupdt.d \
./src/optim/rquad.d \
./src/optim/setopt.d \
./src/optim/setoptim.d \
./src/optim/uminck.d \
./src/optim/umstop.d \
./src/optim/umstp0.d 


# Each subdirectory must supply rules for building sources it contributes
src/optim/%.o: ../src/optim/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


