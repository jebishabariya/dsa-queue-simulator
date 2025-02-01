# Compiler (Use gcc for C)
CC = gcc

# Directories (SDL3 headers and libs inside "src/")
SDL3_INC = -I src/include
SDL3_LIB = -L src/lib -lSDL3main -lSDL3

# Source file (inside "src/")
SRC = src/main.c
OUT = main.exe  # Executable will be in the project root

# Build target
all:
	$(CC) $(SDL3_INC) $(SDL3_LIB) $(SRC) -o $(OUT)

# Run the executable
run: all
	./$(OUT)

# Clean build files
clean:
	del /q main.exe
