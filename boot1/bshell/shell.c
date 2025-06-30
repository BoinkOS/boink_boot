#include "../fs/glfs.h"
#include "../bshell/boot_console.h"
#include "../drivers/video/fb.h"
#include "../utils.h"
#include "../llio.h"

#define KERNEL_LOAD_ADDR ((void*)0x100000)

void shell_cmd_help();
void shell_cmd_list();
void shell_cmd_autorun(const char* arg);
void shell_cmd_load(const char* arg);
void shell_cmd_run();
void shell_cmd_memdump(const char* arg1, const char* arg2);
void exitsafely();

void shell_main() {
	bshell_println(":: Boink Bootloader Interactive Shell ::");
	bshell_println("\n");
	char input[128];
	while (1) {
		bshell_print("bbis> ");
		bshell_input(input, sizeof(input));

		char* cmd = strtok(input, " ");
		char* arg1 = strtok(NULL, " ");
		char* arg2 = strtok(NULL, " ");

		if (!cmd) continue;
		
		bshell_set_title(input);

		if (strcmp(cmd, "help") == 0) {
			shell_cmd_help();
		} else if (strcmp(cmd, "list") == 0) {
			shell_cmd_list();
		} else if (strcmp(cmd, "autorun") == 0) {
			if (!arg1) bshell_println("missing filename");
			else shell_cmd_autorun(arg1);
		} else if (strcmp(cmd, "load") == 0) {
			if (!arg1) bshell_println("missing filename");
			else shell_cmd_load(arg1);
		} else if (strcmp(cmd, "run") == 0) {
			shell_cmd_run();
		} else if (strcmp(cmd, "memdump") == 0) {
			if (!arg1 || !arg2) bshell_println("usage: memdump <start> <end>");
			else shell_cmd_memdump(arg1, arg2);
		} else {
			bshell_print("unknown command: ");
			bshell_println(cmd);
		}

		bshell_putc('\n');
	}
}

void shell_cmd_help() {
	bshell_println("Available commands:");
	bshell_println("list                  - lists available files");
	bshell_println("autorun <filename>    - loads and runs file");
	bshell_println("load <filename>       - loads file");
	bshell_println("run                   - calls default offset");
	bshell_println("memdump <start> <end> - dumps memory in hex");
}

void shell_cmd_list() {
	glfs_read_directory();
	glfs_list_files();
}

void shell_cmd_autorun(const char* filename) {
	int file_index = glfs_find_file(filename);
	if (file_index < 0) {
		bshell_println("file not found");
		return;
	}

	glfs_load_file(file_index, KERNEL_LOAD_ADDR);
	bshell_print("executing ");
	bshell_println(filename);
	
	draw_prekernel_test_pattern();
	
	void (*kernel_entry)(void) = (void*)KERNEL_LOAD_ADDR;
	exitsafely();
	kernel_entry();
}

void shell_cmd_load(const char* filename) {
	int file_index = glfs_find_file(filename);
	if (file_index < 0) {
		bshell_println("file not found");
		return;
	}

	glfs_load_file(file_index, KERNEL_LOAD_ADDR);
	bshell_print("loaded ");
	bshell_println(filename);
}

void shell_cmd_run() {
	bshell_println("jumping to 0x100000");
	void (*kernel_entry)(void) = (void*)KERNEL_LOAD_ADDR;
	exitsafely();
	kernel_entry();
}

static uint32_t parse_hex(const char* str) {
	uint32_t result = 0;

	// skip "0x" or "0X" if present
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		str += 2;
	}

	while (*str) {
		result <<= 4;

		if (*str >= '0' && *str <= '9')
			result |= (*str - '0');
		else if (*str >= 'A' && *str <= 'F')
			result |= (*str - 'A' + 10);
		else if (*str >= 'a' && *str <= 'f')
			result |= (*str - 'a' + 10);
		else
			break; // invalid char

		str++;
	}

	return result;
}

void shell_cmd_memdump(const char* arg1, const char* arg2) {
	uint32_t start = parse_hex(arg1);
	uint32_t end = parse_hex(arg2);

	if (start >= end || end - start > 0x1000) {
		bshell_println("invalid range or too large (max 0x1000)");
		return;
	}

	dump_hex_range(start, end);
}

void exitsafely() {
	__asm__ volatile ("cli");

	struct {
		uint16_t limit;
		uint32_t base;
	} __attribute__((packed)) null_idt = { 0, 0 };

	__asm__ volatile ("lidt %0" : : "m"(null_idt));

	pbout(0x21, 0xFF);
	pbout(0xA1, 0xFF);	
}