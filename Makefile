# Makefile variables
CC = g++
LANG_STD = -std=c++20
COMPILER_FLAGS = -Wall -Wfatal-errors
INCLUDE_PATH = -I"./libs"
SRC_FILES = src/*.cpp src/*/*.cpp
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -llua5.4
OBJ_NAME = gameengine

#Makefile rules
build:
	${CC} ${COMPILER_FLAGS} ${LANG_STD} ${INCLUDE_PATH} ${SRC_FILES} ${LINKER_FLAGS} -o ${OBJ_NAME};

run:
	./${OBJ_NAME}

clean:
	rm -f ${OBJ_NAME}