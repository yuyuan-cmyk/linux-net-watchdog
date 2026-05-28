CC ?= gcc
CFLAGS ?= -Wall -Wextra -Werror -std=c11 -O2 -g
CPPFLAGS ?= -Iinclude -D_POSIX_C_SOURCE=200809L
LDFLAGS ?=

BUILD_DIR := build
SRC_DIR := src
TARGET := $(BUILD_DIR)/net-watchdog

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET) -c config/watchdog.conf

clean:
	rm -rf $(BUILD_DIR)
