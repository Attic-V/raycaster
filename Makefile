.PHONY: all clean

TARGET := raycaster

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
DEP_DIR := $(BUILD_DIR)/dep
SRC_DIR := src

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEP := $(SRC:$(SRC_DIR)/%.c=$(DEP_DIR)/%.d)

CC := gcc

CFLAGS   := -std=c23 -Wall -Werror -Wextra -pedantic
CPPFLAGS := -Iinclude -MMD -MP
LDLIBS   := -lm -lSDL2

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJ) | $(BUILD_DIR)
	@ echo "linking objects"
	@ $(CC) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(DEP_DIR)
	@ echo "compiling $*.c"
	@ $(CC) $(CPPFLAGS) $(CFLAGS) -MF $(DEP_DIR)/$*.d -c -o $@ $<

$(BUILD_DIR) $(OBJ_DIR) $(DEP_DIR):
	mkdir -p $@

clean:
	@ $(RM) -rv $(BUILD_DIR)

-include $(DEP)

