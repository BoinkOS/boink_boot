// isr.c
#include "isr.h"
#include "../bshell/boot_console.h"
#include "../llio.h"
#include "pic.h"

void idt_default_handler() {
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	bshell_println("!!! PANIC !!!");
	bshell_println("An exception has occurred and has been handled by the kernel's default handler.");
	bshell_println("");
	bshell_println("IRQ fired; handled by default handler. Unadvisable to not have IRQ handler.");
	bshell_println("IRQ fired; handled by default handler. Unadvisable to not have IRQ handler.");
	bshell_println("It is advisable to implement your own exception handling.");


    // halt CPU
	for (;;) __asm__ __volatile__("hlt");

	__asm__ __volatile__("popa; leave; iret");
}