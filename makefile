COMPILER = g++
PARAMS = -Wall -Wpedantic -Wextra

BUILD_DIR = build
SRC_DIR = src
SRC = ${SRC_DIR}/main.cpp ${SRC_DIR}/ClientInterface.cpp ${SRC_DIR}/Disk.cpp 
EXT2_BIN = ${BUILD_DIR}/ext2

.PHONY: clean run

all: ${EXT2_BIN}

${EXT2_BIN}:
	mkdir -p ${BUILD_DIR}
	${COMPILER} ${PARAMS} -o $@ ${SRC}


run: all
	${EXT2_BIN}


clean:
	rm -rf ${BUILD_DIR}/*

