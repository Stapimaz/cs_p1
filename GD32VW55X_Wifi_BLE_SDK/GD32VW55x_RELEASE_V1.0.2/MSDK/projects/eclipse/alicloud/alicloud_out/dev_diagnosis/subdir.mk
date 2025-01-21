################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/alicloud/src/services/dev_diagnosis/dev_diagnosis.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/alicloud/src/services/dev_diagnosis/dev_errcode.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/alicloud/src/services/dev_diagnosis/dev_offline_ota.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/alicloud/src/services/dev_diagnosis/dev_state_machine.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/alicloud/src/services/dev_diagnosis/diagnosis_offline_log.c 

C_DEPS += \
./dev_diagnosis/dev_diagnosis.d \
./dev_diagnosis/dev_errcode.d \
./dev_diagnosis/dev_offline_ota.d \
./dev_diagnosis/dev_state_machine.d \
./dev_diagnosis/diagnosis_offline_log.d 

OBJS += \
./dev_diagnosis/dev_diagnosis.o \
./dev_diagnosis/dev_errcode.o \
./dev_diagnosis/dev_offline_ota.o \
./dev_diagnosis/dev_state_machine.o \
./dev_diagnosis/diagnosis_offline_log.o 


# Each subdirectory must supply rules for building sources it contributes
dev_diagnosis/dev_diagnosis.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/alicloud/src/services/dev_diagnosis/dev_diagnosis.c dev_diagnosis/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -std=c99 -DSUPPORT_TLS -DCOMPATIBLE_LK_KV -DCOAP_SERV_MULTITHREAD -DAWSS_SUPPORT_APLIST -DMQTT_AUTO_SUBSCRIBE -DMQTT_COMM_ENABLED -DDEVICE_MODEL_ENABLED -DDEV_BIND_ENABLED -DOTA_ENABLED -DWIFI_PROVISION_ENABLED -DAWSS_SUPPORT_DEV_AP -DAWSS_SUPPORT_SMARTCONFIG -I"..\..\..\..\cloud\alicloud\src\infra\log" -I"..\..\..\..\util\include" -I"..\..\..\..\cloud\alicloud\include" -I"..\..\..\..\cloud\alicloud\include\imports" -I"..\..\..\..\cloud\alicloud\include\exports" -I"..\..\..\..\cloud\alicloud\src\services\dev_bind" -I"..\..\..\..\cloud\alicloud\src\services\ota" -I"..\..\..\..\cloud\alicloud\src\infra\system" -I"..\..\..\..\cloud\alicloud\src\infra\utils" -I"..\..\..\..\cloud\alicloud\src\infra\utils\digest" -I"..\..\..\..\cloud\alicloud\src\infra\utils\kv" -I"..\..\..\..\cloud\alicloud\src\infra\utils\misc" -I"..\..\..\..\cloud\alicloud\src\protocol\alcs" -I"..\..\..\..\cloud\alicloud\src\protocol\coap\local" -I"..\..\..\..\cloud\alicloud\src\protocol\mqtt" -I"..\..\..\..\cloud\alicloud\src\protocol\mqtt\MQTTPacket" -I"..\..\..\..\cloud\alicloud\src\ref-impl\hal\os\freertos" -I"..\..\..\..\cloud\alicloud\src\sdk-impl" -I"..\..\..\..\cloud\alicloud\src\services\awss" -I"..\..\..\..\cloud\alicloud\src\services\shadow" -I"..\..\..\..\cloud\alicloud\src\services\dev_diagnosis" -I"..\..\..\..\cloud\alicloud\src\services\dev_bind\os" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\cm" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\cm\include" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dev_reset" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm\client" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm\server" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\ota" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

dev_diagnosis/dev_errcode.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/alicloud/src/services/dev_diagnosis/dev_errcode.c dev_diagnosis/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -std=c99 -DSUPPORT_TLS -DCOMPATIBLE_LK_KV -DCOAP_SERV_MULTITHREAD -DAWSS_SUPPORT_APLIST -DMQTT_AUTO_SUBSCRIBE -DMQTT_COMM_ENABLED -DDEVICE_MODEL_ENABLED -DDEV_BIND_ENABLED -DOTA_ENABLED -DWIFI_PROVISION_ENABLED -DAWSS_SUPPORT_DEV_AP -DAWSS_SUPPORT_SMARTCONFIG -I"..\..\..\..\cloud\alicloud\src\infra\log" -I"..\..\..\..\util\include" -I"..\..\..\..\cloud\alicloud\include" -I"..\..\..\..\cloud\alicloud\include\imports" -I"..\..\..\..\cloud\alicloud\include\exports" -I"..\..\..\..\cloud\alicloud\src\services\dev_bind" -I"..\..\..\..\cloud\alicloud\src\services\ota" -I"..\..\..\..\cloud\alicloud\src\infra\system" -I"..\..\..\..\cloud\alicloud\src\infra\utils" -I"..\..\..\..\cloud\alicloud\src\infra\utils\digest" -I"..\..\..\..\cloud\alicloud\src\infra\utils\kv" -I"..\..\..\..\cloud\alicloud\src\infra\utils\misc" -I"..\..\..\..\cloud\alicloud\src\protocol\alcs" -I"..\..\..\..\cloud\alicloud\src\protocol\coap\local" -I"..\..\..\..\cloud\alicloud\src\protocol\mqtt" -I"..\..\..\..\cloud\alicloud\src\protocol\mqtt\MQTTPacket" -I"..\..\..\..\cloud\alicloud\src\ref-impl\hal\os\freertos" -I"..\..\..\..\cloud\alicloud\src\sdk-impl" -I"..\..\..\..\cloud\alicloud\src\services\awss" -I"..\..\..\..\cloud\alicloud\src\services\shadow" -I"..\..\..\..\cloud\alicloud\src\services\dev_diagnosis" -I"..\..\..\..\cloud\alicloud\src\services\dev_bind\os" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\cm" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\cm\include" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dev_reset" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm\client" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm\server" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\ota" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

dev_diagnosis/dev_offline_ota.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/alicloud/src/services/dev_diagnosis/dev_offline_ota.c dev_diagnosis/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -std=c99 -DSUPPORT_TLS -DCOMPATIBLE_LK_KV -DCOAP_SERV_MULTITHREAD -DAWSS_SUPPORT_APLIST -DMQTT_AUTO_SUBSCRIBE -DMQTT_COMM_ENABLED -DDEVICE_MODEL_ENABLED -DDEV_BIND_ENABLED -DOTA_ENABLED -DWIFI_PROVISION_ENABLED -DAWSS_SUPPORT_DEV_AP -DAWSS_SUPPORT_SMARTCONFIG -I"..\..\..\..\cloud\alicloud\src\infra\log" -I"..\..\..\..\util\include" -I"..\..\..\..\cloud\alicloud\include" -I"..\..\..\..\cloud\alicloud\include\imports" -I"..\..\..\..\cloud\alicloud\include\exports" -I"..\..\..\..\cloud\alicloud\src\services\dev_bind" -I"..\..\..\..\cloud\alicloud\src\services\ota" -I"..\..\..\..\cloud\alicloud\src\infra\system" -I"..\..\..\..\cloud\alicloud\src\infra\utils" -I"..\..\..\..\cloud\alicloud\src\infra\utils\digest" -I"..\..\..\..\cloud\alicloud\src\infra\utils\kv" -I"..\..\..\..\cloud\alicloud\src\infra\utils\misc" -I"..\..\..\..\cloud\alicloud\src\protocol\alcs" -I"..\..\..\..\cloud\alicloud\src\protocol\coap\local" -I"..\..\..\..\cloud\alicloud\src\protocol\mqtt" -I"..\..\..\..\cloud\alicloud\src\protocol\mqtt\MQTTPacket" -I"..\..\..\..\cloud\alicloud\src\ref-impl\hal\os\freertos" -I"..\..\..\..\cloud\alicloud\src\sdk-impl" -I"..\..\..\..\cloud\alicloud\src\services\awss" -I"..\..\..\..\cloud\alicloud\src\services\shadow" -I"..\..\..\..\cloud\alicloud\src\services\dev_diagnosis" -I"..\..\..\..\cloud\alicloud\src\services\dev_bind\os" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\cm" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\cm\include" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dev_reset" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm\client" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm\server" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\ota" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

dev_diagnosis/dev_state_machine.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/alicloud/src/services/dev_diagnosis/dev_state_machine.c dev_diagnosis/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -std=c99 -DSUPPORT_TLS -DCOMPATIBLE_LK_KV -DCOAP_SERV_MULTITHREAD -DAWSS_SUPPORT_APLIST -DMQTT_AUTO_SUBSCRIBE -DMQTT_COMM_ENABLED -DDEVICE_MODEL_ENABLED -DDEV_BIND_ENABLED -DOTA_ENABLED -DWIFI_PROVISION_ENABLED -DAWSS_SUPPORT_DEV_AP -DAWSS_SUPPORT_SMARTCONFIG -I"..\..\..\..\cloud\alicloud\src\infra\log" -I"..\..\..\..\util\include" -I"..\..\..\..\cloud\alicloud\include" -I"..\..\..\..\cloud\alicloud\include\imports" -I"..\..\..\..\cloud\alicloud\include\exports" -I"..\..\..\..\cloud\alicloud\src\services\dev_bind" -I"..\..\..\..\cloud\alicloud\src\services\ota" -I"..\..\..\..\cloud\alicloud\src\infra\system" -I"..\..\..\..\cloud\alicloud\src\infra\utils" -I"..\..\..\..\cloud\alicloud\src\infra\utils\digest" -I"..\..\..\..\cloud\alicloud\src\infra\utils\kv" -I"..\..\..\..\cloud\alicloud\src\infra\utils\misc" -I"..\..\..\..\cloud\alicloud\src\protocol\alcs" -I"..\..\..\..\cloud\alicloud\src\protocol\coap\local" -I"..\..\..\..\cloud\alicloud\src\protocol\mqtt" -I"..\..\..\..\cloud\alicloud\src\protocol\mqtt\MQTTPacket" -I"..\..\..\..\cloud\alicloud\src\ref-impl\hal\os\freertos" -I"..\..\..\..\cloud\alicloud\src\sdk-impl" -I"..\..\..\..\cloud\alicloud\src\services\awss" -I"..\..\..\..\cloud\alicloud\src\services\shadow" -I"..\..\..\..\cloud\alicloud\src\services\dev_diagnosis" -I"..\..\..\..\cloud\alicloud\src\services\dev_bind\os" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\cm" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\cm\include" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dev_reset" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm\client" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm\server" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\ota" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

dev_diagnosis/diagnosis_offline_log.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/alicloud/src/services/dev_diagnosis/diagnosis_offline_log.c dev_diagnosis/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -std=c99 -DSUPPORT_TLS -DCOMPATIBLE_LK_KV -DCOAP_SERV_MULTITHREAD -DAWSS_SUPPORT_APLIST -DMQTT_AUTO_SUBSCRIBE -DMQTT_COMM_ENABLED -DDEVICE_MODEL_ENABLED -DDEV_BIND_ENABLED -DOTA_ENABLED -DWIFI_PROVISION_ENABLED -DAWSS_SUPPORT_DEV_AP -DAWSS_SUPPORT_SMARTCONFIG -I"..\..\..\..\cloud\alicloud\src\infra\log" -I"..\..\..\..\util\include" -I"..\..\..\..\cloud\alicloud\include" -I"..\..\..\..\cloud\alicloud\include\imports" -I"..\..\..\..\cloud\alicloud\include\exports" -I"..\..\..\..\cloud\alicloud\src\services\dev_bind" -I"..\..\..\..\cloud\alicloud\src\services\ota" -I"..\..\..\..\cloud\alicloud\src\infra\system" -I"..\..\..\..\cloud\alicloud\src\infra\utils" -I"..\..\..\..\cloud\alicloud\src\infra\utils\digest" -I"..\..\..\..\cloud\alicloud\src\infra\utils\kv" -I"..\..\..\..\cloud\alicloud\src\infra\utils\misc" -I"..\..\..\..\cloud\alicloud\src\protocol\alcs" -I"..\..\..\..\cloud\alicloud\src\protocol\coap\local" -I"..\..\..\..\cloud\alicloud\src\protocol\mqtt" -I"..\..\..\..\cloud\alicloud\src\protocol\mqtt\MQTTPacket" -I"..\..\..\..\cloud\alicloud\src\ref-impl\hal\os\freertos" -I"..\..\..\..\cloud\alicloud\src\sdk-impl" -I"..\..\..\..\cloud\alicloud\src\services\awss" -I"..\..\..\..\cloud\alicloud\src\services\shadow" -I"..\..\..\..\cloud\alicloud\src\services\dev_diagnosis" -I"..\..\..\..\cloud\alicloud\src\services\dev_bind\os" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\cm" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\cm\include" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dev_reset" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm\client" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\dm\server" -I"..\..\..\..\cloud\alicloud\src\services\linkkit\ota" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-dev_diagnosis

clean-dev_diagnosis:
	-$(RM) ./dev_diagnosis/dev_diagnosis.d ./dev_diagnosis/dev_diagnosis.o ./dev_diagnosis/dev_errcode.d ./dev_diagnosis/dev_errcode.o ./dev_diagnosis/dev_offline_ota.d ./dev_diagnosis/dev_offline_ota.o ./dev_diagnosis/dev_state_machine.d ./dev_diagnosis/dev_state_machine.o ./dev_diagnosis/diagnosis_offline_log.d ./dev_diagnosis/diagnosis_offline_log.o

.PHONY: clean-dev_diagnosis

