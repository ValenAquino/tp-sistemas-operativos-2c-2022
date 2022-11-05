################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cicloDeInstruccion.c \
../src/comunicacion.c \
../src/configuracion.c \
../src/cpu.c 

C_DEPS += \
./src/cicloDeInstruccion.d \
./src/comunicacion.d \
./src/configuracion.d \
./src/cpu.d 

OBJS += \
./src/cicloDeInstruccion.o \
./src/comunicacion.o \
./src/configuracion.o \
./src/cpu.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/ubuntu/Documents/tp-2022-2c-Compilant/GECK/shared/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/cicloDeInstruccion.d ./src/cicloDeInstruccion.o ./src/comunicacion.d ./src/comunicacion.o ./src/configuracion.d ./src/configuracion.o ./src/cpu.d ./src/cpu.o

.PHONY: clean-src

