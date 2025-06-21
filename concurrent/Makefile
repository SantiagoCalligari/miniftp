CC      := gcc
CFLAGS  := -Wall -Wextra -std=gnu11 -g
LDFLAGS :=

COPY_SRC_DIR := ../iterative
OVERRIDE_FILES := main.c signals.c signals.h

# Automatically detect files in shared folder
ALL_SHARED_C := $(notdir $(wildcard $(COPY_SRC_DIR)/*.c))
ALL_SHARED_H := $(notdir $(wildcard $(COPY_SRC_DIR)/*.h))

# Filter out overridden files
COPY_C_FILES := $(filter-out $(OVERRIDE_FILES),$(ALL_SHARED_C))
COPY_H_FILES := $(filter-out $(OVERRIDE_FILES),$(ALL_SHARED_H))

# Only include .c and .h files, and remove any accidental garbage
COPY_FILES := $(filter %.c %.h, $(COPY_C_FILES) $(COPY_H_FILES))

# Declare which files will be generated
GENERATED_SRCS := $(COPY_C_FILES)
GENERATED_HDRS := $(COPY_H_FILES)

# Build targets
LOCAL_SRCS := main.c session.c signals.c
SRCS       := $(LOCAL_SRCS) $(GENERATED_SRCS)
OBJS       := $(SRCS:.c=.o)
TARGET     := miniftp

.PHONY: all clean prepare

# Default build target
all: prepare $(TARGET)

# Build executable
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Rule to build all .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Prepare: generate needed .c and .h files before anything else
prepare: $(COPY_FILES)

# Rule to copy each needed source/header from shared dir
$(COPY_FILES):
	cp -u $(COPY_SRC_DIR)/$@ .

# Clean up everything
clean:
	rm -f $(OBJS) $(TARGET) $(COPY_FILES)
