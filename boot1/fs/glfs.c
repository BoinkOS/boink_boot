#include <stdint.h>
#include "../disk/disk.h"
#include "../bshell/boot_console.h"
#include "../utils.h"
#include "../mem/mem.h"
#include "glfs.h"

glfs_file_entry glfs_files[MAX_FILES];
int glfs_file_count = 0;

uint8_t dir_buffer[SECTOR_SIZE * 128]; 
int dir_buffer_size = 0;

uint8_t end_marker[] = "__END__\n";

int check_glfs_magic(uint8_t* buffer) {
	const char* expected = "GLFSv0\n";
	int valid = 1;

	for (int i = 0; i < 7; i++) {
		if (buffer[i] != expected[i]) {
			valid = 0;
			break;
		}
	}

	if (valid)
		return 1;
	else
		return 0;
}

void glfs_read_directory() {
	uint8_t sector[SECTOR_SIZE];
	int sector_num = 1;
	
	int done = 0;

	dir_buffer_size = 0;

	while (!done) {
		disk_read(sector_num, sector);

		for (int i = 0; i < SECTOR_SIZE; i++) {
			dir_buffer[dir_buffer_size++] = sector[i];
		}

		if (dir_buffer_size >= 8) { 
			for (int i = 0; i <= dir_buffer_size - 8; i++) {
				if (mem_cmp(&dir_buffer[i], end_marker, 8) == 0) {
					dir_buffer_size = i;
					done = 1;
				}
			}
		}

		sector_num++;
	}

	int offset = 0;
	glfs_file_count = 0;

	while (offset + DIR_ENTRY_SIZE <= dir_buffer_size) {
		if (glfs_file_count >= MAX_FILES)
			break;

		glfs_file_entry* entry = &glfs_files[glfs_file_count];

		mem_cpy(entry->filename, &dir_buffer[offset], FILENAME_SIZE);
		entry->filename[32] = '\0'; // null-terminate

		entry->start_sector = *((uint32_t*)(&dir_buffer[offset + 32]));
		entry->size = *((uint32_t*)(&dir_buffer[offset + 36]));

		glfs_file_count++;
		offset += DIR_ENTRY_SIZE;
	}
}

void glfs_list_files() {
	for (int i = 0; i < glfs_file_count; i++) {
		bshell_print(glfs_files[i].filename);

		bshell_print(" -> ");
		bshell_print(itoa(glfs_files[i].size, 10));

		bshell_println(" bytes");
	}
}

void glfs_load_file(int index, void* dest) {
	if (index >= glfs_file_count) return;

	glfs_file_entry* file = &glfs_files[index];

	bshell_print("loading ");
	bshell_print(file->filename);
	bshell_print("...");
	bshell_putc('\n');

	uint32_t sector = file->start_sector;
	uint32_t remaining = file->size;
	uint8_t buffer[SECTOR_SIZE];
	uint8_t* write_ptr = (uint8_t*)dest;

	while (remaining > 0) {
		disk_read(sector, buffer);

		uint32_t to_copy = remaining > SECTOR_SIZE ? SECTOR_SIZE : remaining;
		mem_cpy(write_ptr, buffer, to_copy);

		write_ptr += to_copy;
		remaining -= to_copy;
		sector++;
	}
}

int glfs_find_file(const char* filename) {
	for (int i = 0; i < glfs_file_count; i++) {
		if (strcmp(glfs_files[i].filename, filename) == 0)
			return i;
	}
	return -1;
}