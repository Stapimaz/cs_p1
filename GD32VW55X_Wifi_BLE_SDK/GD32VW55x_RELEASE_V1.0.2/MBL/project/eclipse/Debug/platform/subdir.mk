################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_eclic.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_fmc.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_gpio.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_rcu.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_usart.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/handlers.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/env_init.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/src/init_rom_symbol.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/arch/lib/lib_hook_mbl.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/gd32vw55x/system_gd32vw55x.c 

S_UPPER_SRCS += \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/entry.S \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/start.S 

C_DEPS += \
./platform/gd32vw55x_eclic.d \
./platform/gd32vw55x_fmc.d \
./platform/gd32vw55x_gpio.d \
./platform/gd32vw55x_rcu.d \
./platform/gd32vw55x_usart.d \
./platform/handlers.d \
./platform/init.d \
./platform/init_rom_symbol.d \
./platform/lib_hook_mbl.d \
./platform/system_gd32vw55x.d 

OBJS += \
./platform/entry.o \
./platform/gd32vw55x_eclic.o \
./platform/gd32vw55x_fmc.o \
./platform/gd32vw55x_gpio.o \
./platform/gd32vw55x_rcu.o \
./platform/gd32vw55x_usart.o \
./platform/handlers.o \
./platform/env_init.o \
./platform/init_rom_symbol.o \
./platform/lib_hook_mbl.o \
./platform/start.o \
./platform/system_gd32vw55x.o 


# Each subdirectory must supply rules for building sources it contributes
platform/entry.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/entry.S platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU Assembler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -Wa,-adhlns=$@.lst   -x assembler-with-cpp -I../../../../config -I../../../mainboot/ -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/gd32vw55x_eclic.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_eclic.c platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -std=c99 -DEXEC_USING_STD_PRINTF -I../../../../config -I../../../../ROM-EXPORT/bootloader -I../../../../ROM-EXPORT/halcomm -I../../../../ROM-EXPORT/mbedtls-2.17.0-rom/include -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/src -I../../../../MSDK/macsw/export -I../../../../MSDK/app -I../../../mainboot/ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/gd32vw55x_fmc.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_fmc.c platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -std=c99 -DEXEC_USING_STD_PRINTF -I../../../../config -I../../../../ROM-EXPORT/bootloader -I../../../../ROM-EXPORT/halcomm -I../../../../ROM-EXPORT/mbedtls-2.17.0-rom/include -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/src -I../../../../MSDK/macsw/export -I../../../../MSDK/app -I../../../mainboot/ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/gd32vw55x_gpio.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_gpio.c platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -std=c99 -DEXEC_USING_STD_PRINTF -I../../../../config -I../../../../ROM-EXPORT/bootloader -I../../../../ROM-EXPORT/halcomm -I../../../../ROM-EXPORT/mbedtls-2.17.0-rom/include -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/src -I../../../../MSDK/macsw/export -I../../../../MSDK/app -I../../../mainboot/ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/gd32vw55x_rcu.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_rcu.c platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -std=c99 -DEXEC_USING_STD_PRINTF -I../../../../config -I../../../../ROM-EXPORT/bootloader -I../../../../ROM-EXPORT/halcomm -I../../../../ROM-EXPORT/mbedtls-2.17.0-rom/include -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/src -I../../../../MSDK/macsw/export -I../../../../MSDK/app -I../../../mainboot/ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/gd32vw55x_usart.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/GD32VW55x_standard_peripheral/Source/gd32vw55x_usart.c platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -std=c99 -DEXEC_USING_STD_PRINTF -I../../../../config -I../../../../ROM-EXPORT/bootloader -I../../../../ROM-EXPORT/halcomm -I../../../../ROM-EXPORT/mbedtls-2.17.0-rom/include -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/src -I../../../../MSDK/macsw/export -I../../../../MSDK/app -I../../../mainboot/ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/handlers.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/handlers.c platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -std=c99 -DEXEC_USING_STD_PRINTF -I../../../../config -I../../../../ROM-EXPORT/bootloader -I../../../../ROM-EXPORT/halcomm -I../../../../ROM-EXPORT/mbedtls-2.17.0-rom/include -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/src -I../../../../MSDK/macsw/export -I../../../../MSDK/app -I../../../mainboot/ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/env_init.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/env_init.c platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -std=c99 -DEXEC_USING_STD_PRINTF -I../../../../config -I../../../../ROM-EXPORT/bootloader -I../../../../ROM-EXPORT/halcomm -I../../../../ROM-EXPORT/mbedtls-2.17.0-rom/include -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/src -I../../../../MSDK/macsw/export -I../../../../MSDK/app -I../../../mainboot/ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/init_rom_symbol.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/src/init_rom_symbol.c platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -std=c99 -DEXEC_USING_STD_PRINTF -I../../../../config -I../../../../ROM-EXPORT/bootloader -I../../../../ROM-EXPORT/halcomm -I../../../../ROM-EXPORT/mbedtls-2.17.0-rom/include -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/src -I../../../../MSDK/macsw/export -I../../../../MSDK/app -I../../../mainboot/ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/lib_hook_mbl.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/arch/lib/lib_hook_mbl.c platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -std=c99 -DEXEC_USING_STD_PRINTF -I../../../../config -I../../../../ROM-EXPORT/bootloader -I../../../../ROM-EXPORT/halcomm -I../../../../ROM-EXPORT/mbedtls-2.17.0-rom/include -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/src -I../../../../MSDK/macsw/export -I../../../../MSDK/app -I../../../mainboot/ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/start.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/start.S platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU Assembler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -Wa,-adhlns=$@.lst   -x assembler-with-cpp -I../../../../config -I../../../mainboot/ -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/system_gd32vw55x.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/gd32vw55x/system_gd32vw55x.c platform/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wuninitialized  -g -std=c99 -DEXEC_USING_STD_PRINTF -I../../../../config -I../../../../ROM-EXPORT/bootloader -I../../../../ROM-EXPORT/halcomm -I../../../../ROM-EXPORT/mbedtls-2.17.0-rom/include -I../../../../MSDK/plf/riscv/gd32vw55x -I../../../../MSDK/plf/riscv/NMSIS/Core/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral/Include -I../../../../MSDK/plf/GD32VW55x_standard_peripheral -I../../../../MSDK/plf/src -I../../../../MSDK/macsw/export -I../../../../MSDK/app -I../../../mainboot/ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-platform

clean-platform:
	-$(RM) ./platform/entry.o ./platform/env_init.o ./platform/gd32vw55x_eclic.d ./platform/gd32vw55x_eclic.o ./platform/gd32vw55x_fmc.d ./platform/gd32vw55x_fmc.o ./platform/gd32vw55x_gpio.d ./platform/gd32vw55x_gpio.o ./platform/gd32vw55x_rcu.d ./platform/gd32vw55x_rcu.o ./platform/gd32vw55x_usart.d ./platform/gd32vw55x_usart.o ./platform/handlers.d ./platform/handlers.o ./platform/init.d ./platform/init_rom_symbol.d ./platform/init_rom_symbol.o ./platform/lib_hook_mbl.d ./platform/lib_hook_mbl.o ./platform/start.o ./platform/system_gd32vw55x.d ./platform/system_gd32vw55x.o

.PHONY: clean-platform

