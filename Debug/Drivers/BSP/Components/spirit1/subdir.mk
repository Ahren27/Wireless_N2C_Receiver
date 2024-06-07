################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/spirit1/radio_target.c 

OBJS += \
./Drivers/BSP/Components/spirit1/radio_target.o 

C_DEPS += \
./Drivers/BSP/Components/spirit1/radio_target.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/spirit1/%.o Drivers/BSP/Components/spirit1/%.su Drivers/BSP/Components/spirit1/%.cyclo: ../Drivers/BSP/Components/spirit1/%.c Drivers/BSP/Components/spirit1/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -IC:/Users/ahren/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/STM32L4xx_HAL_Driver/Inc -IC:/Users/ahren/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -IC:/Users/ahren/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include -IC:/Users/ahren/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/CMSIS/Include -IC:/Users/ahren/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Middlewares/Third_Party/FreeRTOS/Source/include -IC:/Users/ahren/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -IC:/Users/ahren/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/ahren/STM32CubeIDE/workspace_1.15.0/Wireless_N2C_Receiver/Drivers/BSP/Components/spirit1/SPIRIT1_Library/Inc" -I"C:/Users/ahren/STM32CubeIDE/workspace_1.15.0/Wireless_N2C_Receiver/Drivers/BSP/Components/spirit1" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-spirit1

clean-Drivers-2f-BSP-2f-Components-2f-spirit1:
	-$(RM) ./Drivers/BSP/Components/spirit1/radio_target.cyclo ./Drivers/BSP/Components/spirit1/radio_target.d ./Drivers/BSP/Components/spirit1/radio_target.o ./Drivers/BSP/Components/spirit1/radio_target.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-spirit1

