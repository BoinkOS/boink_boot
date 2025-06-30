#include "drivers/video/fb.h"
#include "drivers/video/text.h"
#include "bshell/boot_console.h"


void boots2main() {
	init_framebuffer();
	
	bshell_init();
	bshell_println("GLFS Disk Error.");
	
	while (1);
}

