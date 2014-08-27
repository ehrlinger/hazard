################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/libhazp/hzd_calc_norinv.c \
../src/libhazp/hzp_calc_XsX.c \
../src/libhazp/hzp_calc_fn.c \
../src/libhazp/hzp_calc_haz_CL.c \
../src/libhazp/hzp_calc_hazard.c \
../src/libhazp/hzp_calc_intcp_drv.c \
../src/libhazp/hzp_calc_intcp_subr.c \
../src/libhazp/hzp_calc_log_fn.c \
../src/libhazp/hzp_calc_parm_drv.c \
../src/libhazp/hzp_calc_scale.c \
../src/libhazp/hzp_calc_srv_CL.c \
../src/libhazp/hzp_calc_survival.c \
../src/libhazp/hzp_init_CL.c \
../src/libhazp/hzp_init_preds.c \
../src/libhazp/hzp_pred_time_0.c \
../src/libhazp/hzp_refmt_cov.c \
../src/libhazp/hzp_time_parm_drv.c \
../src/libhazp/hzpe.c \
../src/libhazp/hzpi.c \
../src/libhazp/hzpm.c \
../src/libhazp/hzpo.c \
../src/libhazp/hzpp.c 

OBJS += \
./src/libhazp/hzd_calc_norinv.o \
./src/libhazp/hzp_calc_XsX.o \
./src/libhazp/hzp_calc_fn.o \
./src/libhazp/hzp_calc_haz_CL.o \
./src/libhazp/hzp_calc_hazard.o \
./src/libhazp/hzp_calc_intcp_drv.o \
./src/libhazp/hzp_calc_intcp_subr.o \
./src/libhazp/hzp_calc_log_fn.o \
./src/libhazp/hzp_calc_parm_drv.o \
./src/libhazp/hzp_calc_scale.o \
./src/libhazp/hzp_calc_srv_CL.o \
./src/libhazp/hzp_calc_survival.o \
./src/libhazp/hzp_init_CL.o \
./src/libhazp/hzp_init_preds.o \
./src/libhazp/hzp_pred_time_0.o \
./src/libhazp/hzp_refmt_cov.o \
./src/libhazp/hzp_time_parm_drv.o \
./src/libhazp/hzpe.o \
./src/libhazp/hzpi.o \
./src/libhazp/hzpm.o \
./src/libhazp/hzpo.o \
./src/libhazp/hzpp.o 

C_DEPS += \
./src/libhazp/hzd_calc_norinv.d \
./src/libhazp/hzp_calc_XsX.d \
./src/libhazp/hzp_calc_fn.d \
./src/libhazp/hzp_calc_haz_CL.d \
./src/libhazp/hzp_calc_hazard.d \
./src/libhazp/hzp_calc_intcp_drv.d \
./src/libhazp/hzp_calc_intcp_subr.d \
./src/libhazp/hzp_calc_log_fn.d \
./src/libhazp/hzp_calc_parm_drv.d \
./src/libhazp/hzp_calc_scale.d \
./src/libhazp/hzp_calc_srv_CL.d \
./src/libhazp/hzp_calc_survival.d \
./src/libhazp/hzp_init_CL.d \
./src/libhazp/hzp_init_preds.d \
./src/libhazp/hzp_pred_time_0.d \
./src/libhazp/hzp_refmt_cov.d \
./src/libhazp/hzp_time_parm_drv.d \
./src/libhazp/hzpe.d \
./src/libhazp/hzpi.d \
./src/libhazp/hzpm.d \
./src/libhazp/hzpo.d \
./src/libhazp/hzpp.d 


# Each subdirectory must supply rules for building sources it contributes
src/libhazp/%.o: ../src/libhazp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


