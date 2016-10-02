################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
/home/josh/stm/STM32Cube_FW_F4_V1.13.0/Projects/STM32F412ZG-Nucleo/Examples/GPIO/GPIO_IOToggle/SW4STM32/startup_stm32f412zx.s 

OBJS += \
./Example/SW4STM32/startup_stm32f412zx.o 


# Each subdirectory must supply rules for building sources it contributes
Example/SW4STM32/startup_stm32f412zx.o: /home/josh/stm/STM32Cube_FW_F4_V1.13.0/Projects/STM32F412ZG-Nucleo/Examples/GPIO/GPIO_IOToggle/SW4STM32/startup_stm32f412zx.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


