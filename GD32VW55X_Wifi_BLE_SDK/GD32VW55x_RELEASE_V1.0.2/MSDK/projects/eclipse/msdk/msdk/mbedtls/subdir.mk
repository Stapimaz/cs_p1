################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/debug.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/mbedtls_patch.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/net_sockets.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_cache.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_ciphersuites.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_cli.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/demo/ssl_client.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_cookie.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_default_conf.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_srv.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_ticket.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_tls.c 

C_DEPS += \
./mbedtls/debug.d \
./mbedtls/mbedtls_patch.d \
./mbedtls/net_sockets.d \
./mbedtls/ssl_cache.d \
./mbedtls/ssl_ciphersuites.d \
./mbedtls/ssl_cli.d \
./mbedtls/ssl_client.d \
./mbedtls/ssl_cookie.d \
./mbedtls/ssl_default_conf.d \
./mbedtls/ssl_srv.d \
./mbedtls/ssl_ticket.d \
./mbedtls/ssl_tls.d 

OBJS += \
./mbedtls/debug.o \
./mbedtls/mbedtls_patch.o \
./mbedtls/net_sockets.o \
./mbedtls/ssl_cache.o \
./mbedtls/ssl_ciphersuites.o \
./mbedtls/ssl_cli.o \
./mbedtls/ssl_client.o \
./mbedtls/ssl_cookie.o \
./mbedtls/ssl_default_conf.o \
./mbedtls/ssl_srv.o \
./mbedtls/ssl_ticket.o \
./mbedtls/ssl_tls.o 


# Each subdirectory must supply rules for building sources it contributes
mbedtls/debug.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/debug.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/mbedtls_patch.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/mbedtls_patch.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/net_sockets.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/net_sockets.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/ssl_cache.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_cache.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/ssl_ciphersuites.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_ciphersuites.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/ssl_cli.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_cli.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/ssl_client.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/demo/ssl_client.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/ssl_cookie.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_cookie.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/ssl_default_conf.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_default_conf.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/ssl_srv.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_srv.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/ssl_ticket.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_ticket.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbedtls/ssl_tls.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/mbedtls/mbedtls-2.17.0-ssl/library/ssl_tls.c mbedtls/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-mbedtls

clean-mbedtls:
	-$(RM) ./mbedtls/debug.d ./mbedtls/debug.o ./mbedtls/mbedtls_patch.d ./mbedtls/mbedtls_patch.o ./mbedtls/net_sockets.d ./mbedtls/net_sockets.o ./mbedtls/ssl_cache.d ./mbedtls/ssl_cache.o ./mbedtls/ssl_ciphersuites.d ./mbedtls/ssl_ciphersuites.o ./mbedtls/ssl_cli.d ./mbedtls/ssl_cli.o ./mbedtls/ssl_client.d ./mbedtls/ssl_client.o ./mbedtls/ssl_cookie.d ./mbedtls/ssl_cookie.o ./mbedtls/ssl_default_conf.d ./mbedtls/ssl_default_conf.o ./mbedtls/ssl_srv.d ./mbedtls/ssl_srv.o ./mbedtls/ssl_ticket.d ./mbedtls/ssl_ticket.o ./mbedtls/ssl_tls.d ./mbedtls/ssl_tls.o

.PHONY: clean-mbedtls

