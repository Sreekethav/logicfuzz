# Project layout (adjust if needed)
INC_DIR := include
SRC_DIR := src
BUILD_DIR := build

# Toolchain
CC := gcc
AR := ar
ARFLAGS := rcs

# Flags
CPPFLAGS := -I$(INC_DIR)
CFLAGS   := -Wall -Wextra -O2 -g -fPIC
LDFLAGS  := 
LDLIBS   := 

# Outputs
LIB_STATIC := $(BUILD_DIR)/liba.a
LIB_SHARED := $(BUILD_DIR)/liba.so

# Sources / Objects
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean distclean print

all: $(LIB_STATIC) $(LIB_SHARED)

# Build dirs
$(BUILD_DIR):
	@mkdir -p $@

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

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
	@$(RM) -r $(BUILD_DIR)/*.o

distclean: clean
	@$(RM) -r $(BUILD_DIR)
