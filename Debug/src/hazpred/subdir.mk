################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/hazpred/addvars.c \
../src/hazpred/alocmem.c \
../src/hazpred/cntobsh.c \
../src/hazpred/cntobsi.c \
../src/hazpred/defvars.c \
../src/hazpred/fmtverr.c \
../src/hazpred/getflag.c \
../src/hazpred/gethazr.c \
../src/hazpred/hazpprc.c \
../src/hazpred/hazpred.c \
../src/hazpred/hazpred_l.c \
../src/hazpred/hazpred_y.c \
../src/hazpred/hazverr.c \
../src/hazpred/idproc.c \
../src/hazpred/initprz.c \
../src/hazpred/obsloop.c \
../src/hazpred/obsverr.c \
../src/hazpred/opnfils.c \
../src/hazpred/prederr.c \
../src/hazpred/stmtprc.c \
../src/hazpred/timeprc.c 

OBJS += \
./src/hazpred/addvars.o \
./src/hazpred/alocmem.o \
./src/hazpred/cntobsh.o \
./src/hazpred/cntobsi.o \
./src/hazpred/defvars.o \
./src/hazpred/fmtverr.o \
./src/hazpred/getflag.o \
./src/hazpred/gethazr.o \
./src/hazpred/hazpprc.o \
./src/hazpred/hazpred.o \
./src/hazpred/hazpred_l.o \
./src/hazpred/hazpred_y.o \
./src/hazpred/hazverr.o \
./src/hazpred/idproc.o \
./src/hazpred/initprz.o \
./src/hazpred/obsloop.o \
./src/hazpred/obsverr.o \
./src/hazpred/opnfils.o \
./src/hazpred/prederr.o \
./src/hazpred/stmtprc.o \
./src/hazpred/timeprc.o 

C_DEPS += \
./src/hazpred/addvars.d \
./src/hazpred/alocmem.d \
./src/hazpred/cntobsh.d \
./src/hazpred/cntobsi.d \
./src/hazpred/defvars.d \
./src/hazpred/fmtverr.d \
./src/hazpred/getflag.d \
./src/hazpred/gethazr.d \
./src/hazpred/hazpprc.d \
./src/hazpred/hazpred.d \
./src/hazpred/hazpred_l.d \
./src/hazpred/hazpred_y.d \
./src/hazpred/hazverr.d \
./src/hazpred/idproc.d \
./src/hazpred/initprz.d \
./src/hazpred/obsloop.d \
./src/hazpred/obsverr.d \
./src/hazpred/opnfils.d \
./src/hazpred/prederr.d \
./src/hazpred/stmtprc.d \
./src/hazpred/timeprc.d 


# Each subdirectory must supply rules for building sources it contributes
src/hazpred/%.o: ../src/hazpred/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


