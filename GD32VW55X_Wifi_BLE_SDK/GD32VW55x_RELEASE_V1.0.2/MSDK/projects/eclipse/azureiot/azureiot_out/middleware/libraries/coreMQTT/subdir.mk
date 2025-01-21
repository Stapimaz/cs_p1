################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/azureiot/libs/azure-iot-middleware-freertos/libraries/coreMQTT/source/core_mqtt.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/azureiot/libs/azure-iot-middleware-freertos/libraries/coreMQTT/source/core_mqtt_serializer.c \
C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/azureiot/libs/azure-iot-middleware-freertos/libraries/coreMQTT/source/core_mqtt_state.c 

C_DEPS += \
./middleware/libraries/coreMQTT/core_mqtt.d \
./middleware/libraries/coreMQTT/core_mqtt_serializer.d \
./middleware/libraries/coreMQTT/core_mqtt_state.d 

OBJS += \
./middleware/libraries/coreMQTT/core_mqtt.o \
./middleware/libraries/coreMQTT/core_mqtt_serializer.o \
./middleware/libraries/coreMQTT/core_mqtt_state.o 


# Each subdirectory must supply rules for building sources it contributes
middleware/libraries/coreMQTT/core_mqtt.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/azureiot/libs/azure-iot-middleware-freertos/libraries/coreMQTT/source/core_mqtt.c middleware/libraries/coreMQTT/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -std=c99 -DCONFIG_AZURE_F527_DEMO_SUPPORT -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure" -I"..\..\..\..\cloud\azureiot\freertos-src\FreeRTOS-Plus\Source\Utilities\logging" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\iot" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\iot\internal" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\src\azure\core" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\src\azure\iot" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreMQTT\source\include" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\include" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\interface" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\http_parser" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\dependency\3rdparty\http_parser" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreMQTT\source\interface" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\core" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\core\internal" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\source" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\source\include" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\source\interface" -I"..\..\..\..\cloud\azureiot\demos\projects\GD32VW553\config" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\ports\coreMQTT" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

middleware/libraries/coreMQTT/core_mqtt_serializer.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/azureiot/libs/azure-iot-middleware-freertos/libraries/coreMQTT/source/core_mqtt_serializer.c middleware/libraries/coreMQTT/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -std=c99 -DCONFIG_AZURE_F527_DEMO_SUPPORT -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure" -I"..\..\..\..\cloud\azureiot\freertos-src\FreeRTOS-Plus\Source\Utilities\logging" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\iot" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\iot\internal" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\src\azure\core" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\src\azure\iot" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreMQTT\source\include" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\include" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\interface" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\http_parser" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\dependency\3rdparty\http_parser" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreMQTT\source\interface" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\core" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\core\internal" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\source" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\source\include" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\source\interface" -I"..\..\..\..\cloud\azureiot\demos\projects\GD32VW553\config" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\ports\coreMQTT" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

middleware/libraries/coreMQTT/core_mqtt_state.o: C:/Users/stapi/OneDrive/Desktop/GD32VW553/V1_MQTT_OK/GD32VW55X_Wifi_BLE_SDK/GD32VW55x_RELEASE_V1.0.2/MSDK/cloud/azureiot/libs/azure-iot-middleware-freertos/libraries/coreMQTT/source/core_mqtt_state.c middleware/libraries/coreMQTT/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD RISC-V MCU C Compiler'
	riscv-nuclei-elf-gcc -march=rv32imafcbp -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -std=c99 -DCONFIG_AZURE_F527_DEMO_SUPPORT -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure" -I"..\..\..\..\cloud\azureiot\freertos-src\FreeRTOS-Plus\Source\Utilities\logging" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\iot" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\iot\internal" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\src\azure\core" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\src\azure\iot" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreMQTT\source\include" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\include" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\interface" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\http_parser" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreHTTP\source\dependency\3rdparty\http_parser" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\coreMQTT\source\interface" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\core" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\libraries\azure-sdk-for-c\sdk\inc\azure\core\internal" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\source" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\source\include" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\source\interface" -I"..\..\..\..\cloud\azureiot\demos\projects\GD32VW553\config" -I"..\..\..\..\cloud\azureiot\libs\azure-iot-middleware-freertos\ports\coreMQTT" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns=$@.lst   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-middleware-2f-libraries-2f-coreMQTT

clean-middleware-2f-libraries-2f-coreMQTT:
	-$(RM) ./middleware/libraries/coreMQTT/core_mqtt.d ./middleware/libraries/coreMQTT/core_mqtt.o ./middleware/libraries/coreMQTT/core_mqtt_serializer.d ./middleware/libraries/coreMQTT/core_mqtt_serializer.o ./middleware/libraries/coreMQTT/core_mqtt_state.d ./middleware/libraries/coreMQTT/core_mqtt_state.o

.PHONY: clean-middleware-2f-libraries-2f-coreMQTT

