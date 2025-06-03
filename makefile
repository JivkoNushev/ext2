COMPILER = g++
PARAMS = -Wall -Wpedantic -Wextra

FS_SRC = src/*.cpp src/utils/*.cpp

EXT2_IMG = build/ext2.img
EXT2_BIN = build/ext2
EXT2_SRC = src/ext2/*.cpp ${FS_SRC}

.PHONY: clean ext2

all: ${EXT2_IMG} ${EXT2_BIN}

ext2: ${EXT2_IMG} ${EXT2_BIN}
	${EXT2_BIN} -d build/ext2.img

${EXT2_IMG}:
	mkdir -p build
	sudo dd if=/dev/zero of=build/ext2.img bs=1K count=1K
	sudo mkfs -t ext2 build/ext2.img
	sudo losetup -f build/ext2.img
	sudo mkdir -p /mnt/EXT2
	sudo mount /dev/loop0 /mnt/EXT2

	sudo rm -rf /mnt/EXT2/*
	sudo mkdir -p /mnt/EXT2/dir1/dir2
	sudo touch /mnt/EXT2/file1
	sudo echo "Contents in file2 for testing" | sudo tee /mnt/EXT2/dir1/file2 > /dev/null
	sudo touch /mnt/EXT2/dir1/dir2/file3

	sudo umount /mnt/EXT2
	sudo losetup -d /dev/loop0
	sudo chown $(shell id -u):$(shell id -g) build/ext2.img

${EXT2_BIN}: ${EXT2_SRC}
	mkdir -p build
	${COMPILER} ${PARAMS} -o $@ ${EXT2_SRC}

clean:
	rm -rf build/*

