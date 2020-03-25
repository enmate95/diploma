################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_IFR.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gap_aci.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gatt_aci.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_hal_aci.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_l2cap_aci.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_updater_aci.c \
../Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_utils_small.c 

OBJS += \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_IFR.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gap_aci.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gatt_aci.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_hal_aci.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_l2cap_aci.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_updater_aci.o \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_utils_small.o 

C_DEPS += \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_IFR.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gap_aci.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_gatt_aci.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_hal_aci.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_l2cap_aci.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_updater_aci.d \
./Middlewares/ST/BlueNRG-MS/hci/controller/bluenrg_utils_small.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/BlueNRG-MS/hci/controller/%.o: ../Middlewares/ST/BlueNRG-MS/hci/controller/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_HAL_DRIVER -DSTM32F405xx -I"D:/dokumentumok/bme/Dipterv1/project/repo/Inc" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/Third_Party/FreeRTOS/Source/include" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/CMSIS/Include" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/ST/BlueNRG-MS/includes" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Middlewares/ST/BlueNRG-MS/utils" -I../Middlewares/ST/Middlewares/ST/BlueNRG-MS/includes -I../Middlewares/ST/Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Template -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/CMSIS/Include" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/CMSIS/Include" -I"D:/dokumentumok/bme/Dipterv1/project/repo/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


