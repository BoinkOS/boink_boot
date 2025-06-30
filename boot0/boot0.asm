[bits 16]
[org 0x7c00]

start:
	cli
	xor ax, ax	; zero out
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, STACK	; set stack address
	mov sp, bp
	
	mov si, entry_msg
	call print
	
	mov si, vesa_msg
	call print
	call vesa_init
	call load_gdt
	call load_kernel
	jmp enter_pm

; constants
KERNEL_OFFSET equ 0x100000
STACK equ 0x7000
VESA_MODE equ 0x118
VESA_LFB equ 0x4000 ; linear framebuffer flag
VESA_INFO_BLOCK equ 0x8000 
VESA_MODE_INFO_BLOCK equ 0x9000
FRAMEBUFFER_ADDR equ 0x7E00
FRAMEBUFFER_PITCH equ 0x7E04
FRAMEBUFFER_WIDTH equ 0x7E06
FRAMEBUFFER_HEIGHT equ 0x7E08
FRAMEBUFFER_BPP equ 0x7E0A
vesa_msg db "setting video mode...", 0

; variables
BOOT_DRV db 0
entry_msg db "hello from boink bootloader!", 0

; includes
%include "boot0/print.asm"
%include "boot0/gdt.asm"
%include "boot0/load_sec.asm"
%include "boot0/load_kern.asm"
%include "boot0/vesa.asm"
%include "boot0/pm.asm"

; MBR boot signature
times 510-($-$$) db 0 ; padding
dw 0xaa55 ; magic number