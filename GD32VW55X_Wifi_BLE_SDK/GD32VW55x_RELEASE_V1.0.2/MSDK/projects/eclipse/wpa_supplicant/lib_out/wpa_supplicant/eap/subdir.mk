################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_peer/eap.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_common/eap_common.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_peer/eap_methods.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_server/eap_server.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_peer/eap_wsc.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_common/eap_wsc_common.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eapol_auth/eapol_auth_sm.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eapol_supp/eapol_supp_sm.c 

C_DEPS += \
./wpa_supplicant/eap/eap.d \
./wpa_supplicant/eap/eap_common.d \
./wpa_supplicant/eap/eap_methods.d \
./wpa_supplicant/eap/eap_server.d \
./wpa_supplicant/eap/eap_wsc.d \
./wpa_supplicant/eap/eap_wsc_common.d \
./wpa_supplicant/eap/eapol_auth_sm.d \
./wpa_supplicant/eap/eapol_supp_sm.d 

OBJS += \
./wpa_supplicant/eap/eap.o \
./wpa_supplicant/eap/eap_common.o \
./wpa_supplicant/eap/eap_methods.o \
./wpa_supplicant/eap/eap_server.o \
./wpa_supplicant/eap/eap_wsc.o \
./wpa_supplicant/eap/eap_wsc_common.o \
./wpa_supplicant/eap/eapol_auth_sm.o \
./wpa_supplicant/eap/eapol_supp_sm.o 


# Each subdirectory must supply rules for building sources it contributes
wpa_supplicant/eap/eap.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_peer/eap.c wpa_supplicant/eap/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label  -g -std=c99 -DCFG_RTOS -DCONFIG_WPA_SUPPLICANT -I"..\..\..\..\..\config" -I"..\..\..\..\app" -I"..\..\..\..\wpa_supplicant\wpa_supplicant" -I"..\..\..\..\wpa_supplicant\src" -I"..\..\..\..\wpa_supplicant\src\utils" -I"..\..\..\..\wpa_supplicant\src\crypto" -I"..\..\..\..\wpa_supplicant\src\wps" -I"..\..\..\..\wpa_supplicant\src\eap_common" -I"..\..\..\..\wpa_supplicant\src\eap_peer" -I"..\..\..\..\wpa_supplicant\src\eapol_supp" -I"..\..\..\..\wifi_manager" -I"..\..\..\..\macsw\export" -I"..\..\..\..\macsw\import" -I"..\..\..\..\rtos\rtos_wrapper" -I"..\..\..\..\..\ROM-EXPORT\mbedtls-2.17.0-rom\include" -I"..\..\..\..\..\ROM-EXPORT\bootloader" -I"..\..\..\..\plf\src" -I"..\..\..\..\plf\riscv\arch" -I"..\..\..\..\plf\riscv\arch\boot" -I"..\..\..\..\plf\riscv\arch\ll" -I"..\..\..\..\plf\riscv\arch\compiler" -I"..\..\..\..\plf\src\time" -I"..\..\..\..\plf\src\trng" -I"..\..\..\..\plf\riscv\gd32vw55x" -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -I"..\..\..\..\util\include" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral\Include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\compat\posix" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip\apps" -I"..\..\..\..\lwip\lwip-2.1.2\port" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

wpa_supplicant/eap/eap_common.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_common/eap_common.c wpa_supplicant/eap/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label  -g -std=c99 -DCFG_RTOS -DCONFIG_WPA_SUPPLICANT -I"..\..\..\..\..\config" -I"..\..\..\..\app" -I"..\..\..\..\wpa_supplicant\wpa_supplicant" -I"..\..\..\..\wpa_supplicant\src" -I"..\..\..\..\wpa_supplicant\src\utils" -I"..\..\..\..\wpa_supplicant\src\crypto" -I"..\..\..\..\wpa_supplicant\src\wps" -I"..\..\..\..\wpa_supplicant\src\eap_common" -I"..\..\..\..\wpa_supplicant\src\eap_peer" -I"..\..\..\..\wpa_supplicant\src\eapol_supp" -I"..\..\..\..\wifi_manager" -I"..\..\..\..\macsw\export" -I"..\..\..\..\macsw\import" -I"..\..\..\..\rtos\rtos_wrapper" -I"..\..\..\..\..\ROM-EXPORT\mbedtls-2.17.0-rom\include" -I"..\..\..\..\..\ROM-EXPORT\bootloader" -I"..\..\..\..\plf\src" -I"..\..\..\..\plf\riscv\arch" -I"..\..\..\..\plf\riscv\arch\boot" -I"..\..\..\..\plf\riscv\arch\ll" -I"..\..\..\..\plf\riscv\arch\compiler" -I"..\..\..\..\plf\src\time" -I"..\..\..\..\plf\src\trng" -I"..\..\..\..\plf\riscv\gd32vw55x" -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -I"..\..\..\..\util\include" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral\Include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\compat\posix" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip\apps" -I"..\..\..\..\lwip\lwip-2.1.2\port" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

wpa_supplicant/eap/eap_methods.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_peer/eap_methods.c wpa_supplicant/eap/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label  -g -std=c99 -DCFG_RTOS -DCONFIG_WPA_SUPPLICANT -I"..\..\..\..\..\config" -I"..\..\..\..\app" -I"..\..\..\..\wpa_supplicant\wpa_supplicant" -I"..\..\..\..\wpa_supplicant\src" -I"..\..\..\..\wpa_supplicant\src\utils" -I"..\..\..\..\wpa_supplicant\src\crypto" -I"..\..\..\..\wpa_supplicant\src\wps" -I"..\..\..\..\wpa_supplicant\src\eap_common" -I"..\..\..\..\wpa_supplicant\src\eap_peer" -I"..\..\..\..\wpa_supplicant\src\eapol_supp" -I"..\..\..\..\wifi_manager" -I"..\..\..\..\macsw\export" -I"..\..\..\..\macsw\import" -I"..\..\..\..\rtos\rtos_wrapper" -I"..\..\..\..\..\ROM-EXPORT\mbedtls-2.17.0-rom\include" -I"..\..\..\..\..\ROM-EXPORT\bootloader" -I"..\..\..\..\plf\src" -I"..\..\..\..\plf\riscv\arch" -I"..\..\..\..\plf\riscv\arch\boot" -I"..\..\..\..\plf\riscv\arch\ll" -I"..\..\..\..\plf\riscv\arch\compiler" -I"..\..\..\..\plf\src\time" -I"..\..\..\..\plf\src\trng" -I"..\..\..\..\plf\riscv\gd32vw55x" -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -I"..\..\..\..\util\include" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral\Include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\compat\posix" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip\apps" -I"..\..\..\..\lwip\lwip-2.1.2\port" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

wpa_supplicant/eap/eap_server.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_server/eap_server.c wpa_supplicant/eap/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label  -g -std=c99 -DCFG_RTOS -DCONFIG_WPA_SUPPLICANT -I"..\..\..\..\..\config" -I"..\..\..\..\app" -I"..\..\..\..\wpa_supplicant\wpa_supplicant" -I"..\..\..\..\wpa_supplicant\src" -I"..\..\..\..\wpa_supplicant\src\utils" -I"..\..\..\..\wpa_supplicant\src\crypto" -I"..\..\..\..\wpa_supplicant\src\wps" -I"..\..\..\..\wpa_supplicant\src\eap_common" -I"..\..\..\..\wpa_supplicant\src\eap_peer" -I"..\..\..\..\wpa_supplicant\src\eapol_supp" -I"..\..\..\..\wifi_manager" -I"..\..\..\..\macsw\export" -I"..\..\..\..\macsw\import" -I"..\..\..\..\rtos\rtos_wrapper" -I"..\..\..\..\..\ROM-EXPORT\mbedtls-2.17.0-rom\include" -I"..\..\..\..\..\ROM-EXPORT\bootloader" -I"..\..\..\..\plf\src" -I"..\..\..\..\plf\riscv\arch" -I"..\..\..\..\plf\riscv\arch\boot" -I"..\..\..\..\plf\riscv\arch\ll" -I"..\..\..\..\plf\riscv\arch\compiler" -I"..\..\..\..\plf\src\time" -I"..\..\..\..\plf\src\trng" -I"..\..\..\..\plf\riscv\gd32vw55x" -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -I"..\..\..\..\util\include" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral\Include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\compat\posix" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip\apps" -I"..\..\..\..\lwip\lwip-2.1.2\port" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

wpa_supplicant/eap/eap_wsc.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_peer/eap_wsc.c wpa_supplicant/eap/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label  -g -std=c99 -DCFG_RTOS -DCONFIG_WPA_SUPPLICANT -I"..\..\..\..\..\config" -I"..\..\..\..\app" -I"..\..\..\..\wpa_supplicant\wpa_supplicant" -I"..\..\..\..\wpa_supplicant\src" -I"..\..\..\..\wpa_supplicant\src\utils" -I"..\..\..\..\wpa_supplicant\src\crypto" -I"..\..\..\..\wpa_supplicant\src\wps" -I"..\..\..\..\wpa_supplicant\src\eap_common" -I"..\..\..\..\wpa_supplicant\src\eap_peer" -I"..\..\..\..\wpa_supplicant\src\eapol_supp" -I"..\..\..\..\wifi_manager" -I"..\..\..\..\macsw\export" -I"..\..\..\..\macsw\import" -I"..\..\..\..\rtos\rtos_wrapper" -I"..\..\..\..\..\ROM-EXPORT\mbedtls-2.17.0-rom\include" -I"..\..\..\..\..\ROM-EXPORT\bootloader" -I"..\..\..\..\plf\src" -I"..\..\..\..\plf\riscv\arch" -I"..\..\..\..\plf\riscv\arch\boot" -I"..\..\..\..\plf\riscv\arch\ll" -I"..\..\..\..\plf\riscv\arch\compiler" -I"..\..\..\..\plf\src\time" -I"..\..\..\..\plf\src\trng" -I"..\..\..\..\plf\riscv\gd32vw55x" -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -I"..\..\..\..\util\include" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral\Include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\compat\posix" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip\apps" -I"..\..\..\..\lwip\lwip-2.1.2\port" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

wpa_supplicant/eap/eap_wsc_common.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eap_common/eap_wsc_common.c wpa_supplicant/eap/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label  -g -std=c99 -DCFG_RTOS -DCONFIG_WPA_SUPPLICANT -I"..\..\..\..\..\config" -I"..\..\..\..\app" -I"..\..\..\..\wpa_supplicant\wpa_supplicant" -I"..\..\..\..\wpa_supplicant\src" -I"..\..\..\..\wpa_supplicant\src\utils" -I"..\..\..\..\wpa_supplicant\src\crypto" -I"..\..\..\..\wpa_supplicant\src\wps" -I"..\..\..\..\wpa_supplicant\src\eap_common" -I"..\..\..\..\wpa_supplicant\src\eap_peer" -I"..\..\..\..\wpa_supplicant\src\eapol_supp" -I"..\..\..\..\wifi_manager" -I"..\..\..\..\macsw\export" -I"..\..\..\..\macsw\import" -I"..\..\..\..\rtos\rtos_wrapper" -I"..\..\..\..\..\ROM-EXPORT\mbedtls-2.17.0-rom\include" -I"..\..\..\..\..\ROM-EXPORT\bootloader" -I"..\..\..\..\plf\src" -I"..\..\..\..\plf\riscv\arch" -I"..\..\..\..\plf\riscv\arch\boot" -I"..\..\..\..\plf\riscv\arch\ll" -I"..\..\..\..\plf\riscv\arch\compiler" -I"..\..\..\..\plf\src\time" -I"..\..\..\..\plf\src\trng" -I"..\..\..\..\plf\riscv\gd32vw55x" -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -I"..\..\..\..\util\include" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral\Include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\compat\posix" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip\apps" -I"..\..\..\..\lwip\lwip-2.1.2\port" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

wpa_supplicant/eap/eapol_auth_sm.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eapol_auth/eapol_auth_sm.c wpa_supplicant/eap/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label  -g -std=c99 -DCFG_RTOS -DCONFIG_WPA_SUPPLICANT -I"..\..\..\..\..\config" -I"..\..\..\..\app" -I"..\..\..\..\wpa_supplicant\wpa_supplicant" -I"..\..\..\..\wpa_supplicant\src" -I"..\..\..\..\wpa_supplicant\src\utils" -I"..\..\..\..\wpa_supplicant\src\crypto" -I"..\..\..\..\wpa_supplicant\src\wps" -I"..\..\..\..\wpa_supplicant\src\eap_common" -I"..\..\..\..\wpa_supplicant\src\eap_peer" -I"..\..\..\..\wpa_supplicant\src\eapol_supp" -I"..\..\..\..\wifi_manager" -I"..\..\..\..\macsw\export" -I"..\..\..\..\macsw\import" -I"..\..\..\..\rtos\rtos_wrapper" -I"..\..\..\..\..\ROM-EXPORT\mbedtls-2.17.0-rom\include" -I"..\..\..\..\..\ROM-EXPORT\bootloader" -I"..\..\..\..\plf\src" -I"..\..\..\..\plf\riscv\arch" -I"..\..\..\..\plf\riscv\arch\boot" -I"..\..\..\..\plf\riscv\arch\ll" -I"..\..\..\..\plf\riscv\arch\compiler" -I"..\..\..\..\plf\src\time" -I"..\..\..\..\plf\src\trng" -I"..\..\..\..\plf\riscv\gd32vw55x" -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -I"..\..\..\..\util\include" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral\Include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\compat\posix" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip\apps" -I"..\..\..\..\lwip\lwip-2.1.2\port" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

wpa_supplicant/eap/eapol_supp_sm.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/wpa_supplicant/src/eapol_supp/eapol_supp_sm.c wpa_supplicant/eap/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-unroll-loops -Werror -Wunused -Wuninitialized -Wall -Wno-format -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label  -g -std=c99 -DCFG_RTOS -DCONFIG_WPA_SUPPLICANT -I"..\..\..\..\..\config" -I"..\..\..\..\app" -I"..\..\..\..\wpa_supplicant\wpa_supplicant" -I"..\..\..\..\wpa_supplicant\src" -I"..\..\..\..\wpa_supplicant\src\utils" -I"..\..\..\..\wpa_supplicant\src\crypto" -I"..\..\..\..\wpa_supplicant\src\wps" -I"..\..\..\..\wpa_supplicant\src\eap_common" -I"..\..\..\..\wpa_supplicant\src\eap_peer" -I"..\..\..\..\wpa_supplicant\src\eapol_supp" -I"..\..\..\..\wifi_manager" -I"..\..\..\..\macsw\export" -I"..\..\..\..\macsw\import" -I"..\..\..\..\rtos\rtos_wrapper" -I"..\..\..\..\..\ROM-EXPORT\mbedtls-2.17.0-rom\include" -I"..\..\..\..\..\ROM-EXPORT\bootloader" -I"..\..\..\..\plf\src" -I"..\..\..\..\plf\riscv\arch" -I"..\..\..\..\plf\riscv\arch\boot" -I"..\..\..\..\plf\riscv\arch\ll" -I"..\..\..\..\plf\riscv\arch\compiler" -I"..\..\..\..\plf\src\time" -I"..\..\..\..\plf\src\trng" -I"..\..\..\..\plf\riscv\gd32vw55x" -I"..\..\..\..\plf\riscv\NMSIS\Core\Include" -I"..\..\..\..\util\include" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral" -I"..\..\..\..\plf\GD32VW55x_standard_peripheral\Include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\compat\posix" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip" -I"..\..\..\..\lwip\lwip-2.1.2\src\include\lwip\apps" -I"..\..\..\..\lwip\lwip-2.1.2\port" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-wpa_supplicant-2f-eap

clean-wpa_supplicant-2f-eap:
	-$(RM) ./wpa_supplicant/eap/eap.d ./wpa_supplicant/eap/eap.o ./wpa_supplicant/eap/eap_common.d ./wpa_supplicant/eap/eap_common.o ./wpa_supplicant/eap/eap_methods.d ./wpa_supplicant/eap/eap_methods.o ./wpa_supplicant/eap/eap_server.d ./wpa_supplicant/eap/eap_server.o ./wpa_supplicant/eap/eap_wsc.d ./wpa_supplicant/eap/eap_wsc.o ./wpa_supplicant/eap/eap_wsc_common.d ./wpa_supplicant/eap/eap_wsc_common.o ./wpa_supplicant/eap/eapol_auth_sm.d ./wpa_supplicant/eap/eapol_auth_sm.o ./wpa_supplicant/eap/eapol_supp_sm.d ./wpa_supplicant/eap/eapol_supp_sm.o

.PHONY: clean-wpa_supplicant-2f-eap

