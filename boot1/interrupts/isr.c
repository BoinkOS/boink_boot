// isr.c
#include "isr.h"
#include "../bshell/boot_console.h"
#include "../llio.h"
#include "pic.h"

void idt_default_handler() {
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	bshell_println("!!! PANIC !!!");
	bshell_println("An exception has occurred and has been handled by the bootloader's default handler.");
	bshell_println("Maybe something's wrong with the file you chose to load?");
	bshell_println("");
	bshell_println("Boink Bootloader loads the file in at 0x100000 and calls it. Please ensure that your programs and linker scripts are aligned correctly.");

    // halt CPU
	for (;;) __asm__ __volatile__("hlt");
}