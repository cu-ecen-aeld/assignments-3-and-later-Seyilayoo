# Define compiler variable
CC = $(CROSS_COMPILE)gcc

# Define flags (adjust for your specific needs)
CFLAGS = -Wall 

# Target to build the writer application
writer: writer.o
	$(CC) $(CFLAGS) -o writer writer.o

# Object file from source code
writer.o: writer.c
	$(CC) $(CFLAGS) -c writer.c

# Clean target to remove object files and executable
clean:
	rm -f writer writer.o

# Target for cross-compilation (optional, can be commented out)
cross-compile:
	$(MAKE) CROSS_COMPILE=aarch64-none-linux-gnu-

# Help target to display available options
help:
	@echo "Available targets:"
	@echo "  writer      - Build the writer application"
	@echo "  clean       - Remove object files and executable"
	@echo "  cross-compile - Build for the target architecture (requires CROSS_COMPILE set)"
	@echo "  help        - Display this help message"
