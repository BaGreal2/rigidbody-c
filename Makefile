CXX = gcc
LFLAGS = -L./raylib/lib -lraylib -lm -lpthread -framework OpenGL -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
CXXFLAGS = -Wall -Wextra -O3 -I./raylib/include -I./include 

SRC_DIR = src
OBJ_DIR = obj

OUT = rigidbody

SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(LFLAGS) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(LFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OUT)

.PHONY: all clean
