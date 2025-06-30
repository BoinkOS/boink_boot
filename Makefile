BOOTLOADER_S1_SRC := boot0/boot0.asm
BOOTLOADER_S1_BIN := build/boot0.bin

IMAGE := boinkboot.img

all: $(IMAGE)
	
build:
	mkdir -p build

# assemble bootloader
$(BOOTLOADER_S1_BIN): $(BOOTLOADER_S1_SRC) | build
	nasm -f bin $(BOOTLOADER_S1_SRC) -o $(BOOTLOADER_S1_BIN)

$(IMAGE): $(BOOTLOADER_S1_BIN)
	dd if=/dev/zero of=$(IMAGE) bs=512 count=2880
	dd if=$(BOOTLOADER_S1_BIN) of=$(IMAGE) conv=notrunc

run: $(IMAGE)
	qemu-system-i386 -fda $(IMAGE) -vga std

clean:
	rm -rf build *.img