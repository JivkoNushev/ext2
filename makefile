COMPILER = g++
PARAMS = -Wall -Wpedantic -Wextra

FS_SRC = src/*.cpp

EXT2_BIN = build/ext2
EXT2_SRC = src/ext2/*.cpp ${FS_SRC}

.PHONY: clean ext2

all: ${EXT2_BIN}

ext2: ${EXT2_BIN}
	${EXT2_BIN}

${EXT2_BIN}: ${EXT2_SRC}
	mkdir -p build
	${COMPILER} ${PARAMS} -o $@ ${EXT2_SRC}


clean:
	rm -rf build/*

