################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/env_init.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/handlers.c 

S_UPPER_SRCS += \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/entry.S \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/start.S 

C_DEPS += \
./plf/env/env_init.d \
./plf/env/handlers.d 

OBJS += \
./plf/env/entry.o \
./plf/env/env_init.o \
./plf/env/handlers.o \
./plf/env/start.o 


# Each subdirectory must supply rules for building sources it contributes
plf/env/entry.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/entry.S plf/env/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU Assembler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g -Wa,-adhlns=$@.lst   -x assembler-with-cpp -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

plf/env/env_init.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/env_init.c plf/env/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g -std=c99 -DCFG_RTOS -DCONFIG_WPA_SUPPLICANT -DPLATFORM_OS_FREERTOS -I"..\..\..\..\cloud\alicloud\examples\linkkit\living_platform" -I"..\..\..\..\cloud\alicloud\src\ref-impl\hal\os\freertos" -I"..\..\..\..\cloud\alicloud\src\infra\utils\misc" -I"..\..\..\..\cloud\alicloud\src\infra\utils\kv" -I"..\..\..\..\cloud\alicloud\src\ref-impl\hal" -I"..\..\..\..\cloud\alicloud\src\infra\log" -I"..\..\..\..\cloud\alicloud\src\infra\utils\digest" -I"..\..\..\..\cloud\alicloud\include\exports" -I"..\..\..\..\cloud\alicloud\include\imports" -I"..\..\..\..\cloud\alicloud\include" -I"..\..\..\..\mbedtls\mbedtls-2.17.0-ssl\include" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\adc" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\base\include" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\bluetooth" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\flash" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\gpio" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\hci" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\i2c" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\init\include" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\network" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\pinmux" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\pwm" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\rtc" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\security" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\spi" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\system" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\timer" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\uart" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\utilities\include" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\watchdog" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\wifi" -I"..\..\..\..\cloud\tuya" -I"..\..\..\..\lwip\iperf3" -I"..\..\..\..\lwip\iperf" -I"..\..\..\..\macsw\export" -I"..\..\..\..\macsw\import" -I"..\..\..\..\plf\riscv\arch" -I"..\..\..\..\plf\riscv\arch\boot" -I"..\..\..\..\plf\riscv\arch\lib" -I"..\..\..\..\plf\riscv\arch\ll" -I"..\..\..\..\plf\riscv\arch\compiler" -I"..\..\..\..\plf\src" -I"..\..\..\..\plf\src\reg" -I"..\..\..\..\plf\src\dma" -I"..\..\..\..\plf\src\time" -I"..\..\..\..\plf\src\trng" -I"..\..\..\..\plf\src\uart" -I"..\..\..\..\plf\src\nvds" -I"..\..\..\..\plf\src\raw_flash" -I"..\..\..\..\plf\src\rf" -I"..\..\..\..\plf\riscv\gd32vw55x" -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -I"..\..\..\..\plf\riscv\NMSIS\DSP\Include" -I"..\..\..\..\plf\riscv\NMSIS\DSP\Include\dsp" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral\Include" -I"..\..\..\..\rtos\rtos_wrapper" -I"..\..\..\..\rtos\FreeRTOS\Source\include" -I"..\..\..\..\rtos\FreeRTOS\Source\portable\riscv32" -I"..\..\..\..\rtos\FreeRTOS\config" -I"..\..\..\..\lwip\lwip-2.1.2\src\include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\compat\posix" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip\apps" -I"..\..\..\..\lwip\lwip-2.1.2\port" -I"..\..\..\..\lwip\libcoap-4.3.4\port" -I"..\..\..\..\lwip\libcoap-4.3.4\include" -I"..\..\..\..\wpa_supplicant\wpa_supplicant" -I"..\..\..\..\wpa_supplicant\src" -I"..\..\..\..\wpa_supplicant\src\utils" -I"..\..\..\..\wpa_supplicant\src\crypto" -I"..\..\..\..\mbedtls\mbedtls-2.17.0-ssl\include" -I"..\..\..\..\..\ROM-EXPORT\bootloader" -I"..\..\..\..\..\ROM-EXPORT\halcomm" -I"..\..\..\..\..\ROM-EXPORT\symbol" -I"..\..\..\..\..\ROM-EXPORT\mbedtls-2.17.0-rom\include" -I"..\..\..\..\..\config" -I"..\..\..\..\app" -I"..\..\..\..\app\mqtt_app" -I"..\..\..\..\wifi_manager" -I"..\..\..\..\blesw\src\export" -I"..\..\..\..\blesw\src\export\config_max" -I"..\..\..\..\ble\app" -I"..\..\..\..\ble\profile" -I"..\..\..\..\ble\profile\datatrans" -I"..\..\..\..\util\include" -I"..\..\..\..\FatFS\port" -I"..\..\..\..\FatFS\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

plf/env/handlers.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/handlers.c plf/env/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g -std=c99 -DCFG_RTOS -DCONFIG_WPA_SUPPLICANT -DPLATFORM_OS_FREERTOS -I"..\..\..\..\cloud\alicloud\examples\linkkit\living_platform" -I"..\..\..\..\cloud\alicloud\src\ref-impl\hal\os\freertos" -I"..\..\..\..\cloud\alicloud\src\infra\utils\misc" -I"..\..\..\..\cloud\alicloud\src\infra\utils\kv" -I"..\..\..\..\cloud\alicloud\src\ref-impl\hal" -I"..\..\..\..\cloud\alicloud\src\infra\log" -I"..\..\..\..\cloud\alicloud\src\infra\utils\digest" -I"..\..\..\..\cloud\alicloud\include\exports" -I"..\..\..\..\cloud\alicloud\include\imports" -I"..\..\..\..\cloud\alicloud\include" -I"..\..\..\..\mbedtls\mbedtls-2.17.0-ssl\include" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\adc" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\base\include" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\bluetooth" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\flash" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\gpio" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\hci" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\i2c" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\init\include" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\network" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\pinmux" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\pwm" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\rtc" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\security" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\spi" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\system" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\timer" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\uart" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\utilities\include" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\watchdog" -I"..\..\..\..\cloud\tuya\tuyaos_adapter\include\wifi" -I"..\..\..\..\cloud\tuya" -I"..\..\..\..\lwip\iperf3" -I"..\..\..\..\lwip\iperf" -I"..\..\..\..\macsw\export" -I"..\..\..\..\macsw\import" -I"..\..\..\..\plf\riscv\arch" -I"..\..\..\..\plf\riscv\arch\boot" -I"..\..\..\..\plf\riscv\arch\lib" -I"..\..\..\..\plf\riscv\arch\ll" -I"..\..\..\..\plf\riscv\arch\compiler" -I"..\..\..\..\plf\src" -I"..\..\..\..\plf\src\reg" -I"..\..\..\..\plf\src\dma" -I"..\..\..\..\plf\src\time" -I"..\..\..\..\plf\src\trng" -I"..\..\..\..\plf\src\uart" -I"..\..\..\..\plf\src\nvds" -I"..\..\..\..\plf\src\raw_flash" -I"..\..\..\..\plf\src\rf" -I"..\..\..\..\plf\riscv\gd32vw55x" -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -I"..\..\..\..\plf\riscv\NMSIS\DSP\Include" -I"..\..\..\..\plf\riscv\NMSIS\DSP\Include\dsp" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral\Include" -I"..\..\..\..\rtos\rtos_wrapper" -I"..\..\..\..\rtos\FreeRTOS\Source\include" -I"..\..\..\..\rtos\FreeRTOS\Source\portable\riscv32" -I"..\..\..\..\rtos\FreeRTOS\config" -I"..\..\..\..\lwip\lwip-2.1.2\src\include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\compat\posix" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip\apps" -I"..\..\..\..\lwip\lwip-2.1.2\port" -I"..\..\..\..\lwip\libcoap-4.3.4\port" -I"..\..\..\..\lwip\libcoap-4.3.4\include" -I"..\..\..\..\wpa_supplicant\wpa_supplicant" -I"..\..\..\..\wpa_supplicant\src" -I"..\..\..\..\wpa_supplicant\src\utils" -I"..\..\..\..\wpa_supplicant\src\crypto" -I"..\..\..\..\mbedtls\mbedtls-2.17.0-ssl\include" -I"..\..\..\..\..\ROM-EXPORT\bootloader" -I"..\..\..\..\..\ROM-EXPORT\halcomm" -I"..\..\..\..\..\ROM-EXPORT\symbol" -I"..\..\..\..\..\ROM-EXPORT\mbedtls-2.17.0-rom\include" -I"..\..\..\..\..\config" -I"..\..\..\..\app" -I"..\..\..\..\app\mqtt_app" -I"..\..\..\..\wifi_manager" -I"..\..\..\..\blesw\src\export" -I"..\..\..\..\blesw\src\export\config_max" -I"..\..\..\..\ble\app" -I"..\..\..\..\ble\profile" -I"..\..\..\..\ble\profile\datatrans" -I"..\..\..\..\util\include" -I"..\..\..\..\FatFS\port" -I"..\..\..\..\FatFS\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

plf/env/start.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/plf/riscv/env/start.S plf/env/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU Assembler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g -Wa,-adhlns=$@.lst   -x assembler-with-cpp -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-plf-2f-env

clean-plf-2f-env:
	-$(RM) ./plf/env/entry.o ./plf/env/env_init.d ./plf/env/env_init.o ./plf/env/handlers.d ./plf/env/handlers.o ./plf/env/start.o

.PHONY: clean-plf-2f-env

