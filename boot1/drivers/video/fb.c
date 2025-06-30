#include <stdint.h>
#include <stddef.h>
#include "fb.h"

uint32_t fb_addr = 0;
uint32_t *framebuffer = 0;
uint16_t fb_pitch = 2560;
uint16_t fb_width = 640;
uint16_t fb_height = 480;
uint8_t fb_bpp = 32;

void init_framebuffer() {
	fb_addr = *FRAMEBUFFER_ADDRESS_PTR;
	fb_pitch = *FRAMEBUFFER_PITCH_PTR;
	fb_width = *FRAMEBUFFER_WIDTH_PTR;
	fb_height = *FRAMEBUFFER_HEIGHT_PTR;
	fb_bpp = *FRAMEBUFFER_BPP_PTR;
	framebuffer = (uint32_t*)fb_addr;
}

uint32_t encode_color(uint8_t r, uint8_t g, uint8_t b) {
	return (b) | (g << 8) | (r << 16);
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
	uint8_t *fb = (uint8_t *)framebuffer;
	uint32_t bytes_per_pixel = fb_bpp / 8;
	uint32_t offset = y * fb_pitch + x * bytes_per_pixel;
	*(uint32_t *)(fb + offset) = color;
}

void copy_pixel(uint32_t sx, uint32_t sy, uint32_t dx, uint32_t dy) {
	uint8_t *fb = (uint8_t *)framebuffer;
	uint32_t bytes_per_pixel = fb_bpp / 8;
	uint32_t s_offset = sy * fb_pitch + sx * bytes_per_pixel;
	uint32_t d_offset = dy * fb_pitch + dx * bytes_per_pixel;
	
	*(uint32_t *)(fb + d_offset) = *(uint32_t *)(fb + s_offset);
}

void draw_test_pattern() {
	for (size_t y = 0; y < fb_height; ++y) {
		for (size_t x = 0; x < fb_width; ++x) {
			uint8_t r = (x * 255) / fb_width;
			uint8_t g = (y * 255) / fb_height;
			uint8_t b = 128;
			put_pixel(x, y, encode_color(r, g, b));
		}
	}
}

void draw_prekernel_test_pattern() {
	const uint32_t color1 = encode_color(0, 0, 0);         // black
	const uint32_t color2 = encode_color(20, 20, 20);       // medium grey

	const size_t square_size = 32; // 32x32 pixel tiles

	for (size_t y = 0; y < fb_height; ++y) {
		for (size_t x = 0; x < fb_width; ++x) {
			size_t square_x = x / square_size;
			size_t square_y = y / square_size;
			int use_alt_color = (square_x + square_y) % 2;

			put_pixel(x, y, use_alt_color ? color2 : color1);
		}
	}
}