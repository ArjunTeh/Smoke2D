SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = build
LIB_DIR = lib
TARGET = smoke2d

LIBS = -lglfw3 -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lpthread -ldl -lGLEW -lGLU -lGL -I./$(INCLUDE_DIR)/
CFLAGS = -g -std=c++11 -MMD $(LIBS) 
CC = g++

CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
H_FILES := $(wildcard $(INCLUDE_DIR)/*.hpp)
OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o)))

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:  
	rm -f $(OBJ_FILES) $(TARGET)

.PHONY: all
all:  
	make clean && make -j8

.PHONY: run
run:
	make all && ./$(TARGET)
