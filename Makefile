SOURCE_DIR := src
BUILD_DIR := build

TARGET := pop3filter

RE_SOURCES := $(shell find $(SOURCE_DIR)/ -type f -name "*.re")
C_SOURCES := $(shell find $(SOURCE_DIR)/ -type f -name "*.c") $(RE_SOURCES:%.re=%.c)
RE_GENERATED := $(RE_SOURCES:%.re=%.c) $(RE_SOURCES:%.re=%.h) 
OBJS := $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)

DEP_FLAGS := -MMD -MP
CFLAGS += -g -std=gnu11 -Wall -I$(SOURCE_DIR) $(DEP_FLAGS) -Iutilities/src -fsanitize=address
REFLAGS += -f -W -i -c --no-generation-date

all: lexer c

c: $(BUILD_DIR)/$(TARGET) utilities

utilities:
	$(MAKE) -C utilities native

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C utilities clean

clean-all: clean
	rm -f $(RE_GENERATED)

rebuild: clean c

rebuild-all: clean-all lexer c

lexer: $(RE_GENERATED)

$(BUILD_DIR)/$(TARGET): $(OBJS) utilities
	$(CC) $(CFLAGS) -o $@ $(OBJS) utilities/obj/libutilities.a

$(BUILD_DIR)/%.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<


-include $(OBJS:%.o=%.d)

MKDIR_P ?= mkdir -p

.PHONY: all clean rebuild c lexer clean-all rebuild-all utilities