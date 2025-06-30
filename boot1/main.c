#include "drivers/video/fb.h"
#include "drivers/video/text.h"
#include "bshell/boot_console.h"
#include "disk/disk.h"
#include "fs/glfs.h"
#include "utils.h"
#include "main.h"

#include "interrupts/idt.h"
#include "interrupts/pic.h"
#include "interrupts/irq.h"
#include "interrupts/pit.h"
#include "input/keyboard/keyboard.h"
#include "uptime/uptime.h"
extern void pit_uptime_handler(uint32_t irq);
extern void ata_irq_handler(uint32_t irq_num);

void boots2main() {
	init_framebuffer();
	draw_test_pattern();
	pic_remap();
	idt_init(0x08);
	irq_init(0x08);
	pit_init(100);
	keyboard_init();
	irq_set_handler(0, pit_uptime_handler);
	
	bshell_init();
	bshell_println("\"hello from boink bootloader!\"");
	bshell_println("-------------------------");
	
	__asm__ __volatile__("sti");
	
	bshell_println("initializing disk...");
	irq_set_handler(14, ata_irq_handler);
	disk_init();
	bshell_println("reading sector 0...");
	uint8_t buf[512];
	disk_read(0, buf);
	bshell_println("done reading sector 0. attempting to read GLFS superblock...");
	
	char id[9];
	for (int i = 0; i < 8; i++)
		id[i] = buf[i];
	id[8] = '\0';
	
	int is_glfs_verified = check_glfs_magic(buf);
	
	if (is_glfs_verified) {
		bshell_print("read superblock identifier: ");
		bshell_print(id);
		bshell_print("[ ");
		bshell_print("GLFS PASS");
		bshell_print(" ]");
		bshell_println("");
	} else {
		bshell_println("Unable to verify primary slave as GLFS disk.");
		bshell_println("Reboot with the correct disk addressed to primary slave.");
		while (1) ;
	}
	

	bshell_println("\nFiles on disk:");
	glfs_read_directory();
	glfs_list_files();
	
	glfs_load_file(0, (void*)0x100000);
	void (*kernel_entry)(void) = (void*)0x100000;
	draw_test_pattern();
	kernel_entry();
	
	
	bshell_println("-------------------------");
	bshell_println("~ Boink Rescue ~");
	
	dump_hex_range(0x100000, 0x100100);
	bshell_println("\n");
	char input[128];
	while (1) {
		bshell_print("boot> ");
		bshell_input(input, sizeof(input));
		bshell_print("ECHO: ");
		bshell_set_title(input);
		bshell_print(input);
		bshell_putc('\n');
		bshell_putc('\n');
	}
}

