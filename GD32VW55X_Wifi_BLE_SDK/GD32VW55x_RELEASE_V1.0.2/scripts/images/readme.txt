image-all-msdk.bin：		    SDK默认状态下使用GD32 Embedded-Builder编译出来的image，直接烧录进Flash即可使用。

image-all-msdk-full-app.bin：   SDK开启了功能：
							    (MQTT、COAP、OTA_DEMO、AT指令、
								 IPERF测试(默认开启)、FATFS文件系统、
							     ssl_demo(使用mbedtls实现的简单https的demo)、
								 socket_demo(调用lwip socket api的socket demo))
							     该image是使用使用GD32 Embedded-Builder编译出来的，直接烧录进flash即可使用；
							     使用uart输入命令help可查看所有命令，命令具体使用请参考文档
							    《AN153 GD32VW553 基本指令用户指南》和《AN151 GD32VW553 AT指令用户指南》
								
image-all-msdk-ffd-full-app.bin：该image是使用msdk_ffd的configuration编译的，msdk_ffd和msdk的区别请参考文档
								《AN154 GD32VW553 快速开发指南》的3.2.6小节；
								该image同样是使用使用GD32 Embedded-Builder编译出来的，直接烧录进flash即可使用。

以上三个image均为mbl.bin和image-ota.bin的结合，有关mbl.bin和image-ota.bin的说明请参考文档
《AN154 GD32VW553 快速开发指南》的3.1小节。