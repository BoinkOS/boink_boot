#include <stdint.h>


#define FONT_WIDTH 8
#define FONT_HEIGHT 16

void bshell_init();
void bshell_set_title(const char *str);
void bshell_clear();
void bshell_putc(char c);
void bshell_print(const char *str);
void bshell_println(const char *str);
void bshell_print_dec(uint32_t i);
void bshell_print_hex(uint32_t i);
void bshell_input(char *buf, uint32_t max_len);