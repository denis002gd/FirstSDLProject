# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`

# SDL2 Libraries
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lm

# Source files
SRCS = main.c list.c render.c buttons.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = program

# Default rule
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile .c files into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

