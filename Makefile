CC      = gcc
AR      = ar

SRC_DIR = src
INC_DIR = include
LIB_DIR = lib
EXA_DIR = examples

LIB_SRC = $(SRC_DIR)/rigidbodylib.c
LIB_HDR = $(INC_DIR)/rigidbodylib.h

LIB_OBJ = rigidbodylib.o
LIB_A   = $(LIB_DIR)/librigidbodylib.a

EXAMPLE_SRC = $(EXA_DIR)/double_pendulum.c $(EXA_DIR)/friction.c
EXAMPLE_BIN = double_pendulum friction

CFLAGS  = -Wall -Wextra -O3 -I./raylib/include -I./include
LFLAGS  = -L./raylib/lib -lraylib -lm -lpthread -framework OpenGL -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

all: library $(EXAMPLE_BIN)

library: $(LIB_A)

$(LIB_A): $(LIB_OBJ)
	mkdir -p $(LIB_DIR)
	$(AR) rcs $@ $(LIB_OBJ)
	cp $(LIB_HDR) $(LIB_DIR)/

$(LIB_OBJ): $(LIB_SRC) $(LIB_HDR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

double_pendulum: $(EXA_DIR)/double_pendulum.c $(LIB_A)
	$(CC) $(CFLAGS) -I$(LIB_DIR) -o $@ $(EXA_DIR)/double_pendulum.c -L$(LIB_DIR) -lrigidbodylib $(LFLAGS)

friction: $(EXA_DIR)/friction.c $(LIB_A)
	$(CC) $(CFLAGS) -I$(LIB_DIR) -o $@ $(EXA_DIR)/friction.c -L$(LIB_DIR) -lrigidbodylib $(LFLAGS)

clean:
	rm -f $(LIB_OBJ) $(LIB_A) $(EXAMPLE_BIN)
	rm -rf $(LIB_DIR)

.PHONY: all library clean
