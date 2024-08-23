# Define the cross-compiler prefix. Defaults to empty if not specified.
CROSS_COMPILE ?=

# Define the compiler to use based on the cross-compiler prefix.
CC = $(CROSS_COMPILE)gcc

# Compiler flags for generating debugging information and enabling all warnings.
CFLAGS = -g -Wall

# Name of the output executable.
TARGET = writer

# Object file required to build the target executable.
OBJ = writer.o

# Default target: builds the executable.
all: $(TARGET)

# Rule to link the object file and create the executable.
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule to compile the source file into an object file.
$(OBJ): writer.c
	$(CC) $(CFLAGS) -c writer.c -o writer.o

# Rule to clean up build artifacts (executable and object files).
clean:
	rm -f $(TARGET) $(OBJ)

# Custom target to provide help when nothing needs to be done.
help:
	@echo "Usage Examples:"
	@echo "1. Build the application natively:"
	@echo "   make"
	@echo ""
	@echo "2. Cross-compile the application:"
	@echo "   make CROSS_COMPILE=aarch64-none-linux-gnu-"
	@echo ""
	@echo "3. Clean the build artifacts:"
	@echo "   make clean"
	@echo ""
	@echo "4. Cross-compile with additional flags:"
	@echo "   make CROSS_COMPILE=aarch64-none-linux-gnu- CFLAGS=\"-g -Wall -O2\""
	@echo ""
	@echo "5. Build verbosely:"
	@echo "   make V=1 #see make -h"

# Declare phony targets to avoid conflicts with files of the same name.
.PHONY: all clean help
