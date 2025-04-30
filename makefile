COMPILER = g++
PARAMS = -Wall -Wpedantic -Wextra

BUILD_DIR = build
SRC_DIR = src
SRC = ${SRC_DIR}/main.cpp ${SRC_DIR}/Disk.cpp ${SRC_DIR}/Ext2FS.cpp ${SRC_DIR}/SuperBlock.cpp

EXT2_BIN = ${BUILD_DIR}/ext2

.PHONY: clean run

all: ${EXT2_BIN}

${EXT2_BIN}: ${SRC}
	mkdir -p ${BUILD_DIR}
	${COMPILER} ${PARAMS} -o $@ ${SRC}


run: all
	${EXT2_BIN}


clean:
	rm -rf ${BUILD_DIR}/*

