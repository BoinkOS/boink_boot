#include "drivers/video/fb.h"
#include "drivers/video/text.h"


void boots2main() {
	init_framebuffer();
	draw_string(50, 50, 0xFFFFFF, 0x000000, "an ominous hello appears.\nvga fonts galore!");
	
	while (1);
}

