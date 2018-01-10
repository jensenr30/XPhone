################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
A:/Programming/XPhone/common/src/key.c 

OBJS += \
./common/src/key.o 

C_DEPS += \
./common/src/key.d 


# Each subdirectory must supply rules for building sources it contributes
common/src/key.o: A:/Programming/XPhone/common/src/key.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F412ZGTx -DSTM32F4 -DNUCLEO_F412ZG -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F412Zx -I"A:/Programming/XPhone/micro/stm/XPhone/inc" -I"A:/Programming/XPhone/micro/stm/XPhone/Utilities/STM32F4xx_Nucleo_144" -I"A:/Programming/XPhone/micro/stm/XPhone/HAL_Driver/Inc" -I"A:/Programming/XPhone/micro/stm/XPhone/HAL_Driver/Inc/Legacy" -I"A:/Programming/XPhone/micro/stm/XPhone/CMSIS/core" -I"A:/Programming/XPhone/micro/stm/XPhone/CMSIS/device" -I"A:/Programming/XPhone/micro/stm/XPhone/../../../common/inc/" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


