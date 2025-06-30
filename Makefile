BOOTLOADER_S1_SRC := boot0/boot0.asm
BOOTLOADER_S1_BIN := build/boot0.bin

BOOTLOADER_S2_SRCS := $(shell find boot1 -name '*.c')
BOOTLOADER_S2_OBJS := $(patsubst boot1/%.c, build/%.o, $(BOOTLOADER_S2_SRCS))
BOOTLOADER_S2_OBJS_ORDERED := $(filter build/main.o, $(BOOTLOADER_S2_OBJS)) \
					$(filter-out build/main.o, $(BOOTLOADER_S2_OBJS))
BOOTLOADER_S2_BIN := build/boot1.bin

LINKER_SCRIPT := link.ld
IMAGE := boinkboot.img

CC := i686-elf-gcc
LD := i386-elf-ld
CFLAGS := -m32 -ffreestanding -O2 -Wall -Wextra -Isrc
LD_FLAGS := -T $(LINKER_SCRIPT) --oformat binary -nostdlib -e boots2main

all: $(IMAGE)
	
build:
	mkdir -p build

$(BOOTLOADER_S1_BIN): $(BOOTLOADER_S1_SRC) | build
	nasm -f bin $(BOOTLOADER_S1_SRC) -o $(BOOTLOADER_S1_BIN)

build/%.o: boot1/%.c | build
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(BOOTLOADER_S2_BIN): $(BOOTLOADER_S2_OBJS_ORDERED) $(IVEC_OBJ) $(EVEC_OBJ) $(LINKER_SCRIPT)
	$(LD) $(LD_FLAGS) $(BOOTLOADER_S2_OBJS_ORDERED) -o $(BOOTLOADER_S2_BIN)

$(IMAGE): $(BOOTLOADER_S1_BIN) $(BOOTLOADER_S2_BIN)
	dd if=/dev/zero of=$(IMAGE) bs=512 count=2880
	dd if=$(BOOTLOADER_S1_BIN) of=$(IMAGE) conv=notrunc
	dd if=$(BOOTLOADER_S2_BIN) of=$(IMAGE) bs=512 seek=1 conv=notrunc

run: $(IMAGE)
	qemu-system-i386 -fda $(IMAGE) -vga std

clean:
	rm -rf build *.img