################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/stm32f4xx_it.c \
../src/syscalls.c \
../src/system_stm32f4xx.c 

OBJS += \
./src/main.o \
./src/stm32f4xx_it.o \
./src/syscalls.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/main.d \
./src/stm32f4xx_it.d \
./src/syscalls.d \
./src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F412ZGTx -DSTM32F4 -DNUCLEO_F412ZG -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F412Zx -I"/mnt/C/Programing/Github/XPhone/micro/stm/XPhone/inc" -I"/mnt/C/Programing/Github/XPhone/micro/stm/XPhone/Utilities/STM32F4xx_Nucleo_144" -I"/mnt/C/Programing/Github/XPhone/micro/stm/XPhone/HAL_Driver/Inc" -I"/mnt/C/Programing/Github/XPhone/micro/stm/XPhone/HAL_Driver/Inc/Legacy" -I"/mnt/C/Programing/Github/XPhone/micro/stm/XPhone/CMSIS/core" -I"/mnt/C/Programing/Github/XPhone/micro/stm/XPhone/CMSIS/device" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

