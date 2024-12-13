CC = gcc
CFLAGS = -g -Wall

TARGET_DIR = ../..
TARGET = $(TARGET_DIR)/player

SRC_DIRS = . ../common
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

OBJ_DIR = bin
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	rm -f $(TARGET) $(OBJS)
	rm -rf $(OBJ_DIR)
	rm -rf ./common
	rm -rf ./scores

.PHONY: clean
