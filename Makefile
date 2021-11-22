SOURCE_DIR := src
BUILD_DIR := build
RE2C ?= : re2c

TARGET := server

RE_SOURCES := $(shell find $(SOURCE_DIR)/ -type f -name "*.re")
C_SOURCES := $(shell find $(SOURCE_DIR)/ -type f -name "*.c") $(RE_SOURCES:%.re=%.c)
RE_GENERATED := $(RE_SOURCES:%.re=%.c) $(RE_SOURCES:%.re=%.h) 
OBJS := $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)

DEP_FLAGS := -MMD -MP
CFLAGS += -g -std=gnu11 -fsanitize=address -Wall -I$(SOURCE_DIR) $(DEP_FLAGS)
REFLAGS += -W -i --no-generation-date

all: lexer c

c: $(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

clean-all: clean
	rm -f $(RE_GENERATED)

rebuild: clean c

rebuild-all: clean-all lexer c

lexer: $(RE_GENERATED)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SOURCE_DIR)/parsers/management.c: $(SOURCE_DIR)/parsers/management.re
	$(RE2C) $(REFLAGS) -c -o $@ -t $*.h $<

-include $(OBJS:%.o=%.d)

MKDIR_P ?= mkdir -p

.PHONY: all clean rebuild c lexer clean-all rebuild-all