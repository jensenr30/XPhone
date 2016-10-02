################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/BSP/STM32F4xx_Nucleo_144/stm32f4xx_nucleo_144.c 

OBJS += \
./Drivers/BSP/STM32412ZG_Nucleo/stm32f4xx_nucleo_144.o 

C_DEPS += \
./Drivers/BSP/STM32412ZG_Nucleo/stm32f4xx_nucleo_144.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32412ZG_Nucleo/stm32f4xx_nucleo_144.o: /home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/BSP/STM32F4xx_Nucleo_144/stm32f4xx_nucleo_144.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F412Zx -DUSE_HAL_DRIVER -DUSE_STM32F4XX_NUCLEO_144 -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Projects/STM32F412ZG-Nucleo/Examples/GPIO/GPIO_IOToggle/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/BSP/STM32F4xx_Nucleo_144"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


