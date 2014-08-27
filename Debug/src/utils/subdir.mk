################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/utils/hzr_calc_loge_mu.c \
../src/utils/hzr_cum_haz_calc.c \
../src/utils/hzr_cum_haz_func.c \
../src/utils/hzr_parm.c \
../src/utils/hzr_qdiv.c \
../src/utils/hzr_qextd.c \
../src/utils/hzr_qinit.c \
../src/utils/hzr_qmul.c \
../src/utils/hzr_qneg.c \
../src/utils/hzr_qsum.c \
../src/utils/hzr_qtod.c \
../src/utils/hzr_set_error.c \
../src/utils/hzr_set_parm_err.c \
../src/utils/hzr_xmul.c 

OBJS += \
./src/utils/hzr_calc_loge_mu.o \
./src/utils/hzr_cum_haz_calc.o \
./src/utils/hzr_cum_haz_func.o \
./src/utils/hzr_parm.o \
./src/utils/hzr_qdiv.o \
./src/utils/hzr_qextd.o \
./src/utils/hzr_qinit.o \
./src/utils/hzr_qmul.o \
./src/utils/hzr_qneg.o \
./src/utils/hzr_qsum.o \
./src/utils/hzr_qtod.o \
./src/utils/hzr_set_error.o \
./src/utils/hzr_set_parm_err.o \
./src/utils/hzr_xmul.o 

C_DEPS += \
./src/utils/hzr_calc_loge_mu.d \
./src/utils/hzr_cum_haz_calc.d \
./src/utils/hzr_cum_haz_func.d \
./src/utils/hzr_parm.d \
./src/utils/hzr_qdiv.d \
./src/utils/hzr_qextd.d \
./src/utils/hzr_qinit.d \
./src/utils/hzr_qmul.d \
./src/utils/hzr_qneg.d \
./src/utils/hzr_qsum.d \
./src/utils/hzr_qtod.d \
./src/utils/hzr_set_error.d \
./src/utils/hzr_set_parm_err.d \
./src/utils/hzr_xmul.d 


# Each subdirectory must supply rules for building sources it contributes
src/utils/%.o: ../src/utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


