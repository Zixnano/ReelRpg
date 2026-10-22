# Reel RPG - GBA homebrew prototype
ifeq ($(strip $(DEVKITPRO)),)
$(error DEVKITPRO is not set. Install devkitPro and set DEVKITPRO.)
endif
ifeq ($(strip $(DEVKITARM)),)
$(error DEVKITARM is not set. Set DEVKITARM to $(DEVKITPRO)/devkitARM.)
endif
include $(DEVKITARM)/gba_rules

TARGET := reel-rpg
BUILD := build
SOURCES := src
CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
OFILES := $(CFILES:.c=.o)
CFLAGS := -g -Wall -Wextra -O2 -mcpu=arm7tdmi -mtune=arm7tdmi -mthumb-interwork -I$(DEVKITPRO)/libgba/include
LDFLAGS := -g $(ARCH)
LIBS := -L$(DEVKITPRO)/libgba/lib -lgba
export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)

.PHONY: all clean
all: $(BUILD) $(OUTPUT).gba

$(BUILD):
	@mkdir -p $@

$(OUTPUT).gba: $(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) $(LIBS) -o $(OUTPUT).elf
	$(OBJCOPY) -O binary $(OUTPUT).elf $(OUTPUT).gba
	@$(NM) -CSn $(OUTPUT).elf > $(OUTPUT).map
	@echo Built $(OUTPUT).gba

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -MF $(DEPSDIR)/$*.d -c $< -o $@

clean:
	@rm -rf $(BUILD) $(OUTPUT).elf $(OUTPUT).gba $(OUTPUT).map

-include $(wildcard $(DEPSDIR)/*.d)
