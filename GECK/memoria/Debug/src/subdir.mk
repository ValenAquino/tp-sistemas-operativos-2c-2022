################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comunicacion.c \
../src/configuracion.c \
../src/interfaz.c \
../src/memoria.c \
../src/memoria_principal.c \
../src/reemplazos.c \
../src/swap.c 

C_DEPS += \
./src/comunicacion.d \
./src/configuracion.d \
./src/interfaz.d \
./src/memoria.d \
./src/memoria_principal.d \
./src/reemplazos.d \
./src/swap.d 

OBJS += \
./src/comunicacion.o \
./src/configuracion.o \
./src/interfaz.o \
./src/memoria.o \
./src/memoria_principal.o \
./src/reemplazos.o \
./src/swap.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-2c-Compilant/GECK/shared/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/comunicacion.d ./src/comunicacion.o ./src/configuracion.d ./src/configuracion.o ./src/interfaz.d ./src/interfaz.o ./src/memoria.d ./src/memoria.o ./src/memoria_principal.d ./src/memoria_principal.o ./src/reemplazos.d ./src/reemplazos.o ./src/swap.d ./src/swap.o

.PHONY: clean-src

