################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c 

OBJS += \
./Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal.o \
./Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_cortex.o \
./Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_gpio.o \
./Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_rcc.o \
./Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_rcc_ex.o 

C_DEPS += \
./Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal.d \
./Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_cortex.d \
./Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_gpio.d \
./Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_rcc.d \
./Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_rcc_ex.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal.o: /home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F412Zx -DUSE_HAL_DRIVER -DUSE_STM32F4XX_NUCLEO_144 -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Projects/STM32F412ZG-Nucleo/Examples/GPIO/GPIO_IOToggle/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/BSP/STM32F4xx_Nucleo_144"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_cortex.o: /home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F412Zx -DUSE_HAL_DRIVER -DUSE_STM32F4XX_NUCLEO_144 -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Projects/STM32F412ZG-Nucleo/Examples/GPIO/GPIO_IOToggle/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/BSP/STM32F4xx_Nucleo_144"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_gpio.o: /home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F412Zx -DUSE_HAL_DRIVER -DUSE_STM32F4XX_NUCLEO_144 -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Projects/STM32F412ZG-Nucleo/Examples/GPIO/GPIO_IOToggle/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/BSP/STM32F4xx_Nucleo_144"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_rcc.o: /home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F412Zx -DUSE_HAL_DRIVER -DUSE_STM32F4XX_NUCLEO_144 -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Projects/STM32F412ZG-Nucleo/Examples/GPIO/GPIO_IOToggle/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/BSP/STM32F4xx_Nucleo_144"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/STM32F4xx_HAL_Driver/stm32f4xx_hal_rcc_ex.o: /home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F412Zx -DUSE_HAL_DRIVER -DUSE_STM32F4XX_NUCLEO_144 -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Projects/STM32F412ZG-Nucleo/Examples/GPIO/GPIO_IOToggle/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/CMSIS/Include" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Drivers/BSP/STM32F4xx_Nucleo_144"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


