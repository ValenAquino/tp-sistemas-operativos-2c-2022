################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comunicacion.c \
../src/consola.c \
../src/pseudocodigo.c 

C_DEPS += \
./src/comunicacion.d \
./src/consola.d \
./src/pseudocodigo.d 

OBJS += \
./src/comunicacion.o \
./src/consola.o \
./src/pseudocodigo.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/media/sf_tp/tp-2022-2c-Compilant/GECK/shared/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/comunicacion.d ./src/comunicacion.o ./src/consola.d ./src/consola.o ./src/pseudocodigo.d ./src/pseudocodigo.o

.PHONY: clean-src

