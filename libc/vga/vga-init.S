/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.internal.h"
#include "libc/vga/vga.internal.h"

//	Code snippet for initializing the VGA video mode for bare metal.
//
//	If a program requests VGA support (by yoinking vga_console),
//	and it is started in bare metal mode, then try to ensure that
//	the VGA monitor is in a known mode.  This is easier to do while
//	the program is still running in real mode.
//
//	This module also ropes in the sys_writev_vga routine, which
//	implements the actual VGA console output under x86-64 long mode.
//
//	@see	rlinit & .sort.text.real.init.* (ape/ape.S)
//	@see	ape/ape.lds
//	@see	sys_writev_vga (libc/vga/writev-vga.c)
	.section .sort.text.real.init.2,"ax",@progbits
	.code16
	mov	$0x4f03,%ax		# get current video mode via VESA
	int	$0x10
	cmp	$0x004f,%ax		# is VESA a thing here?
	jz	1f
	mov	$0x0f,%ah		# if not, get the video mode via a
	int	$0x10			# classical BIOS call
	cbtw
	xchgw	%ax,%bx
1:	mov	$0x0003,%ax		# check if we are in a 80 × ? × 16
	cmp	%ax,%bx			# text mode
	jnz	2f
	cmpb	$25-1,0x0484		# check if number of screen rows
	jnz	2f			# (BDA.ROWS + 1) is 25; if so, then
	mov	$0x0500,%ax		# just make sure we are on display
					# page 0
2:	int	$0x10			# otherwise, change the video mode
	mov	$0x1003,%ax		# enable/disable VGA text blinking
#ifdef VGA_USE_BLINK
	mov	$1,%bx
#else
	xor	%bx,%bx
#endif
	int	$0x10
	.previous
	.code64
	.section .rodata,"a",@progbits
vga_console:
	.endobj	vga_console,globl,hidden
	.previous
	.yoink	sys_writev_vga
	.yoink	sys_readv_vga
