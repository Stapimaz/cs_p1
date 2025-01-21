################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/gd32vw55x/gd32vw55x_it.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/gd32vw55x/system_gd32vw55x.c 

C_DEPS += \
./plf/gd32vw55x/gd32vw55x_it.d \
./plf/gd32vw55x/system_gd32vw55x.d 

OBJS += \
./plf/gd32vw55x/gd32vw55x_it.o \
./plf/gd32vw55x/system_gd32vw55x.o 


# Each subdirectory must supply rules for building sources it contributes
plf/gd32vw55x/gd32vw55x_it.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/gd32vw55x/gd32vw55x_it.c plf/gd32vw55x/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

plf/gd32vw55x/system_gd32vw55x.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/gd32vw55x/system_gd32vw55x.c plf/gd32vw55x/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-plf-2f-gd32vw55x

clean-plf-2f-gd32vw55x:
	-$(RM) ./plf/gd32vw55x/gd32vw55x_it.d ./plf/gd32vw55x/gd32vw55x_it.o ./plf/gd32vw55x/system_gd32vw55x.d ./plf/gd32vw55x/system_gd32vw55x.o

.PHONY: clean-plf-2f-gd32vw55x

