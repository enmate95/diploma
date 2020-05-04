################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/freertos.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_hal_timebase_tim.c \
../Src/stm32f4xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f4xx.c 

CPP_SRCS += \
../Src/main.cpp 

OBJS += \
./Src/freertos.o \
./Src/main.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_hal_timebase_tim.o \
./Src/stm32f4xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f4xx.o 

C_DEPS += \
./Src/freertos.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_hal_timebase_tim.d \
./Src/stm32f4xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f4xx.d 

CPP_DEPS += \
./Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_HAL_DRIVER -DSTM32F405xx -I"D:/dokumentumok/bme/Dipterv1/project/repo/Inc" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/Third_Party/FreeRTOS/Source/include" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Src/%.o: ../Src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_HAL_DRIVER -DSTM32F405xx -I"D:/dokumentumok/bme/Dipterv1/project/repo/Inc" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/Third_Party/FreeRTOS/Source/include" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


