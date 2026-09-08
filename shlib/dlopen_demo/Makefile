# Compiler variables
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Target names
LIB_NAME = libmyplugin.so
EXE_NAME = my_program

# Default rule: Build both the shared library and the executable
all: $(LIB_NAME) $(EXE_NAME)

# Compile the main program and link against the dynamic loading library (-ldl)
$(EXE_NAME): main.c
	$(CC) $(CFLAGS) main.c -ldl -o $(EXE_NAME)

# Build the shared library from source
$(LIB_NAME): my_plugin.c
	$(CC) $(CFLAGS) -fPIC -shared my_plugin.c -o $(LIB_NAME)

# Clean up built binaries
clean:
	rm -f $(LIB_NAME) $(EXE_NAME)
