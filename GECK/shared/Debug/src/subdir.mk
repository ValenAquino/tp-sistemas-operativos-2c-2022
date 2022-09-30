################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cliente.c \
../src/protocolo.c \
../src/servidor.c \
../src/shared.c 

C_DEPS += \
./src/cliente.d \
./src/protocolo.d \
./src/servidor.d \
./src/shared.d 

OBJS += \
./src/cliente.o \
./src/protocolo.o \
./src/servidor.o \
./src/shared.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/cliente.d ./src/cliente.o ./src/protocolo.d ./src/protocolo.o ./src/servidor.d ./src/servidor.o ./src/shared.d ./src/shared.o

.PHONY: clean-src

