# Reel RPG - standard devkitPro GBA project layout
.SUFFIXES:

ifeq ($(strip $(DEVKITPRO)),)
$(error DEVKITPRO is not set)
endif
ifeq ($(strip $(DEVKITARM)),)
$(error DEVKITARM is not set)
endif

include $(DEVKITARM)/gba_rules

TARGET := reel-rpg
BUILD := build
SOURCES := src
LIBDIRS := $(LIBGBA)
GAME_TITLE := REEL RPG
GAME_CODE := RERP
MAKER_CODE := 01
ARCH := -mthumb -mthumb-interwork -mcpu=arm7tdmi
CFLAGS := -g -O2 -Wall -Wextra $(ARCH)
# devkitARM base_rules compiles with CFLAGS/CPPFLAGS; it does not consume INCLUDE by itself.
# Keep this recursive so the exported INCLUDE from the parent make is expanded in build/.
CFLAGS += $(INCLUDE)
LIBS := -lgba

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)
export PATH := $(DEVKITARM)/bin:$(PATH)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
export OFILES := $(CFILES:.c=.o)
export INCLUDE := $(foreach dir,$(LIBDIRS),-I$(dir)/include) -I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
export LIBS
export CFLAGS
export ARCH
export GAME_TITLE
export GAME_CODE
export MAKER_CODE

.PHONY: all clean
all: $(BUILD) $(OUTPUT).gba

$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).gba $(TARGET).map

else

DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).gba: $(OUTPUT).elf

$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

endif
