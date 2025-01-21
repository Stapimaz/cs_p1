################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include wpa_supplicant/wps/subdir.mk
-include wpa_supplicant/tls/subdir.mk
-include wpa_supplicant/eap/subdir.mk
-include wpa_supplicant/ap/subdir.mk
-include wpa_supplicant/subdir.mk
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


BUILD_ARTIFACT_NAME := libwpa_supplicant
BUILD_ARTIFACT_EXTENSION := a
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: libwpa_supplicant.a

# Tool invocations
libwpa_supplicant.a: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GD RISC-V MCU Archiver'
	riscv-nuclei-elf-ar -r  "libwpa_supplicant.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) libwpa_supplicant.a
	-@echo ' '

post-build:
	-cp "libwpa_supplicant.a" "../../../../lib"
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
