################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comunicacion.c \
../src/configuracion.c \
../src/kernel.c \
../src/planificadorCortoPlazo.c \
../src/planificadorLargoPlazo.c \
../src/suspendidos.c 

C_DEPS += \
./src/comunicacion.d \
./src/configuracion.d \
./src/kernel.d \
./src/planificadorCortoPlazo.d \
./src/planificadorLargoPlazo.d \
./src/suspendidos.d 

OBJS += \
./src/comunicacion.o \
./src/configuracion.o \
./src/kernel.o \
./src/planificadorCortoPlazo.o \
./src/planificadorLargoPlazo.o \
./src/suspendidos.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/media/sf_tp/tp-2022-2c-Compilant/GECK/shared/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/comunicacion.d ./src/comunicacion.o ./src/configuracion.d ./src/configuracion.o ./src/kernel.d ./src/kernel.o ./src/planificadorCortoPlazo.d ./src/planificadorCortoPlazo.o ./src/planificadorLargoPlazo.d ./src/planificadorLargoPlazo.o ./src/suspendidos.d ./src/suspendidos.o

.PHONY: clean-src

