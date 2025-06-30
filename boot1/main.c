#include "drivers/video/fb.h"
#include "drivers/video/text.h"
#include "bshell/boot_console.h"
#include "utils.h"

#include "interrupts/idt.h"
#include "interrupts/pic.h"
#include "interrupts/irq.h"
#include "interrupts/pit.h"
#include "input/keyboard/keyboard.h"
#include "uptime/uptime.h"
extern void pit_uptime_handler(uint32_t irq);


void boots2main() {
	init_framebuffer();
	
	pic_remap();
	idt_init(0x08);
	irq_init(0x08);
	pit_init(100);
	keyboard_init();
	irq_set_handler(0, pit_uptime_handler);
	
	bshell_init();
	bshell_println("\"hello from boink bootloader!\"");
	bshell_println("-------------------------");
	bshell_println("~ Boink Interactive Bootloader ~");
	
	__asm__ __volatile__("sti");
	
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

