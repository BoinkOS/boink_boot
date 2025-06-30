#include "drivers/video/fb.h"
#include "drivers/video/text.h"
#include "bshell/boot_console.h"
#include "utils.h"

#include "interrupts/idt.h"
#include "interrupts/pic.h"
#include "interrupts/irq.h"
#include "interrupts/pit.h"
#include "input/keyboard/keyboard.h"


void boots2main() {
	init_framebuffer();
	
	draw_test_pattern();
	
	pic_remap();
	idt_init(0x08);
	irq_init(0x08);
	pit_init(100);
	keyboard_init();
	
	bshell_init();
	
	bshell_println("Boink Bootloader Shell");
	
	__asm__ __volatile__("sti");
	
	bshell_println("\n");
	char input[128];
	while (1) {
		bshell_print("boink$ ");
		bshell_input(input, sizeof(input));
		bshell_print("ECHO: ");
		bshell_print(input);
		bshell_putc('\n');
		bshell_putc('\n');
	}
}

