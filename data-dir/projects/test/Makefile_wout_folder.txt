BUILD_DIR := build

CC := gcc
AR := ar
ARFLAGS := rcs

CFLAGS := -Wall -Wextra -O2 -g -fPIC
LDFLAGS :=
LDLIBS :=

LIB_STATIC := $(BUILD_DIR)/liba.a
LIB_SHARED := $(BUILD_DIR)/liba.so

# Source files are all .c files in current directory
SRCS := $(wildcard *.c)

# Object files in build/ with same base names
OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

.PHONY: all clean distclean print

all: $(LIB_STATIC) $(LIB_SHARED)

# Make sure build directory exists before compiling
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Static library
$(LIB_STATIC): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

# Shared library
$(LIB_SHARED): $(OBJS)
	$(CC) -shared $(LDFLAGS) -o $@ $^ $(LDLIBS)

print:
	@echo "Sources: $(SRCS)"
	@echo "Objects: $(OBJS)"

clean:
	@rm -rf $(BUILD_DIR)

distclean: clean
