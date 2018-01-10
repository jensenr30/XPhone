################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32f412zx.s 

OBJS += \
./startup/startup_stm32f412zx.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo %cd%
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -I"A:/Programming/XPhone/micro/stm/XPhone/inc" -I"A:/Programming/XPhone/micro/stm/XPhone/Utilities/STM32F4xx_Nucleo_144" -I"A:/Programming/XPhone/micro/stm/XPhone/HAL_Driver/Inc" -I"A:/Programming/XPhone/micro/stm/XPhone/HAL_Driver/Inc/Legacy" -I"A:/Programming/XPhone/micro/stm/XPhone/CMSIS/core" -I"A:/Programming/XPhone/micro/stm/XPhone/CMSIS/device" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


