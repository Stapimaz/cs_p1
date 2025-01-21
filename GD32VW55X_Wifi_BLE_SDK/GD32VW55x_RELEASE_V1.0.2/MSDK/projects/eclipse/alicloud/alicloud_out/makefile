################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include util/subdir.mk
-include shadow/subdir.mk
-include sdk_impl/subdir.mk
-include ota/subdir.mk
-include mqtt/subdir.mk
-include linkkit/subdir.mk
-include infra/subdir.mk
-include dm/subdir.mk
-include dev_reset/subdir.mk
-include dev_diagnosis/subdir.mk
-include dev_bind/subdir.mk
-include coap/subdir.mk
-include awss/subdir.mk
-include alcs/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := libalicloud
BUILD_ARTIFACT_EXTENSION := a
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: libalicloud.a secondary-outputs

# Tool invocations
libalicloud.a: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GD RISC-V MCU Archiver'
	riscv-nuclei-elf-ar -r   "libalicloud.a" $(OBJS) $(USER_OBJS) $(LIBS) ./awss/awss_security.o
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) libalicloud.a
	-@echo ' '

post-build:
	-cp "libalicloud.a" "../../../../lib"
	-@echo ' '

secondary-outputs:

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
