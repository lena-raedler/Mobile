CC = gcc
LD = gcc

OBJ = Mobile_Shader.o LoadShader.o Matrix.o StringExtra.o OBJParser.o List.o LoadTexture.o
TARGET = Mobile_Shader

CFLAGS = -g -Wall -Wextra -std=gnu99
LDLIBS = -lm -lglut -lGLEW -lGL -lGLU
INCLUDES = -Isource

SRC_DIR = source
BUILD_DIR = build
VPATH = source

# Rules
all: $(TARGET)

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

clean:
	rm -f $(BUILD_DIR)/*.o *.o $(TARGET) 

.PHONY:  all clean

# Dependencies
$(TARGET): $(BUILD_DIR)/LoadShader.o $(BUILD_DIR)/Matrix.o $(BUILD_DIR)/StringExtra.o $(BUILD_DIR)/OBJParser.o $(BUILD_DIR)/LoadTexture.o $(BUILD_DIR)/List.o | $(BUILD_DIR)



