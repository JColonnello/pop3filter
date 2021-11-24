SOURCE_DIR := src
BUILD_DIR := build
RE2C ?= : re2c

TARGET := pop3filter
CLIENT := client

RE_SOURCES := $(shell find $(SOURCE_DIR)/ -type f -name "*.re")
C_SOURCES := $(shell find $(SOURCE_DIR)/ -type f -name "*.c") $(RE_SOURCES:%.re=%.c)
RE_GENERATED := $(RE_SOURCES:%.re=%.c) $(RE_SOURCES:%.re=%.h) 
OBJS := $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)

DEP_FLAGS := -MMD -MP
CFLAGS += -g -std=gnu11 -Wall -I$(SOURCE_DIR) $(DEP_FLAGS) -Iutilities/src -fsanitize=address
REFLAGS += -W -i --no-generation-date

all: lexer c client

c: $(BUILD_DIR)/$(TARGET) utilities

utilities:
	$(MAKE) -C utilities native

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C utilities clean

clean-all: clean
	rm -f $(RE_GENERATED)

client: $(BUILD_DIR)/$(CLIENT)

rebuild: clean c

rebuild-all: clean-all lexer c

lexer: $(RE_GENERATED)

utilities/obj/libutilities.a: utilities

$(BUILD_DIR)/$(TARGET): $(OBJS) utilities/obj/libutilities.a
	$(CC) $(CFLAGS) -o $@ $^ -lanl

$(BUILD_DIR)/$(CLIENT): Client/mgmtClient.c
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SOURCE_DIR)/parsers/management.c: $(SOURCE_DIR)/parsers/management.re
	$(RE2C) $(REFLAGS) -c -o $@ -t $*.h $<

$(SOURCE_DIR)/parsers/pop3.c: $(SOURCE_DIR)/parsers/pop3.re
	$(RE2C) $(REFLAGS) -c -f -o $@ -t $*.h $<

-include $(OBJS:%.o=%.d)

MKDIR_P ?= mkdir -p

.PHONY: all clean rebuild c lexer clean-all rebuild-all utilities client