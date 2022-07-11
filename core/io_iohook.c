/*
 * Copyright (c) 2007, 2008 University of Tsukuba
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Tsukuba nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "current.h"
#include "debug.h"
#include "initfunc.h"
#include "io_io.h"
#include "io_iohook.h"
#include "panic.h"
#include "printf.h"
#include "mm.h"

#ifdef DEBUG_IO_MONITOR
static enum ioact
kbdio_monitor (enum iotype type, u32 port, void *data)
{
	do_io_default (type, port, data);
	if (type == IOTYPE_INB)
		printf ("IO Monitor test: INB PORT=0x60 DATA=0x%X\n", *(u8 *)data);
	return IOACT_CONT;
}
#endif

#ifdef DEBUG_IO0x20_MONITOR
#include "vramwrite.h"
static enum ioact
io0x20_monitor (enum iotype type, u32 port, void *data)
{
	do_io_default (type, port, data);
	if (type == IOTYPE_OUTB) {
		vramwrite_save_and_move_cursor (56, 23);
		printf ("OUT0x20,0x%02X", *(u8 *)data);
		vramwrite_restore_cursor ();
	}
	return IOACT_CONT;
}
#endif

#if defined (F11PANIC) || defined (F12MSG)
#include "keyboard.h"
static enum ioact
kbdio_dbg_monitor (enum iotype type, u32 port, void *data)
{
	static int led = 0;
	static u8 lk = 0;
#ifdef NTTCOM_TEST
        unsigned long int session;
        int i, j;
        char sig[1024];
        unsigned short int siglen ;
#endif

	do_io_default (type, port, data);
	if (type == IOTYPE_INB) {
#ifdef CARDSTATUS
		extern int ps2_locked;
		if (ps2_locked) {
			printf ("Ignoring PS/2 input\n");
			*(u8 *)data = 0;
			return IOACT_CONT;
		}
#endif
		switch (*(u8 *)data) {
#if defined(F10USBTEST)
		case 0x44 | 0x80: /* F10 */
			if (lk == 0x44) {
				extern void usb_api_batchtest(void);
			
				printf ("F10 pressed.\n");
				usb_api_batchtest();
			}
			break;
#endif /* defined(F10USBTEST) */
		case 0x57 | 0x80: /* F11 */
			if (lk == 0x57) {
#ifdef NTTCOM_TEST
                                printf ("F11 pressed.\n");
                                printf ("IDMan_IPInitializeReader.\n");
                                i = IDMan_IPInitializeReader( );
                                printf ("IDMan_IPInitializeReader return = %d.\n", i);
                                printf ("IDMan_IPInitialize.\n");
                                i = IDMan_IPInitialize("123456789@ABCDEF",  &session);
                                printf ("IDMan_IPInitialize return = %d.\n", i);
                                printf ("IDMan_generateSignatureByIndex.\n");
                                i = IDMan_generateSignatureByIndex( session, 1, "1234567890abcdef", strlen("1234567890abcdef"), sig, &siglen, 544);
                                printf ("IDMan_generateSignatureByIndex return = %d siglen=%d\n", i, siglen);
                                printf ("IDMan_IPFinalize.\n");
                                i = IDMan_IPFinalize(session);
                                printf ("IDMan_IPFinalize return = %d.\n", i);
                                printf ("IDMan_IPFinalizeReader.\n");
                                i = IDMan_IPFinalizeReader( );
                                printf ("IDMan_IPFinalizeReader return = %d.\n", i);
#else
#ifdef F11PANIC
				if (config.vmm.f11panic)
					panic ("F11 pressed.");
#endif
#endif
			}
			break;
		case 0x58 | 0x80: /* F12 */
			if (lk == 0x58) {
#if defined(F12DUMPEHCI)
				extern void ehci_dump_all(int, void *);
#endif /* defined(F12DUMPEHCI) */
#if defined(F12MSG)
				if (config.vmm.f12msg) {
					debug_gdb ();
					led ^= LED_CAPSLOCK_BIT |
						LED_NUMLOCK_BIT;
					setkbdled (led);
					printf ("F12 pressed.\n");
				}
#endif /* defined(F12MSG) */
#if defined(F12DUMPEHCI)
				ehci_dump_all(0, NULL);
#endif /* defined(F12DUMPEHCI) */
			}
			break;
		}
		lk = *(u8 *)data;
	}
	return IOACT_CONT;
}
#endif



#define SVGA_INDEX_PORT         		0x0000
#define SVGA_VALUE_PORT         		0x0001
#define SVGA_BIOS_PORT          		0x0002
#define SVGA_IRQSTATUS_PORT     		0x0008

#define SVGA_REG_ENABLE					0x0001


void
mmio_hphys_access (phys_t gphysaddr, bool wr, void *buf, uint len, u32 flags)
{
	void *p;

	if (!len)
		return;
	p = mapmem_hphys (gphysaddr, len, flags);
	if (wr)
		memcpy (p, buf, len);
	else
		memcpy (buf, p, len);
	unmapmem (p, len);
}


__attribute__((always_inline))
static inline void __outdword(u16 port, u32 data)
{
  asm volatile("outl %0,%1" :: "a" (data), "Nd" (port));
}

__attribute__((always_inline))
static inline u32 __indword(u16 port)
{
  u32 value;
  asm volatile("inl %1,%0" : "=a" (value) : "Nd" (port));
  return value;
}

static u32 ReadReg(u32 ioBase, u32 index)
{
	__outdword(ioBase + SVGA_INDEX_PORT, index);
	return __indword(ioBase + SVGA_VALUE_PORT);
}

// changes register in certain hardware
static void WriteReg(u32 ioBase, u32 index, u32 value)
{
	__outdword(ioBase + SVGA_INDEX_PORT, index);
	__outdword(ioBase + SVGA_VALUE_PORT, value);
}

typedef enum
{
	VGA_COLOR_BLACK,
	VGA_COLOR_BLUE,
	VGA_COLOR_GREEN,
	VGA_COLOR_CYAN,
	VGA_COLOR_RED,
	VGA_COLOR_MAGENTA,
	VGA_COLOR_BROWN,
	VGA_COLOR_LIGHT_GRAY,
	VGA_COLOR_DARK_GRAY,
	VGA_COLOR_LIGHT_BLUE,
	VGA_COLOR_LIGHT_GREEN,
	VGA_COLOR_LIGHT_CYAN,
	VGA_COLOR_LIGHT_RED,
	VGA_COLOR_LIGHT_MAGENTA,
	VGA_COLOR_YELLOW,
	VGA_COLOR_WHITE,
} VgaColor;

// writes string in row and column
void VmwareSvgaPutString(char *s, u64 row, u64 column)
{
	//vramwrite_vga_putchar('@'); // ******* TO DELETE *******
	char *p = mapmem(MAPMEM_GPHYS | MAPMEM_WRITE, 0xB8000, 4096);
	p += (row * 80 + column) * 2;
	for (int i = 0; *s; ++s, p += 2, i++) { // index 'i' is for DEBUG 
		*p = *s;
	}
	unmapmem(p, 4096);
	printf("\nVmwareSvgaPutString : after mmio_hphys_access\n"); /***** DEBUG *****/
}
// only for text mode
void VmwareSvgaColorFill(VgaColor back, VgaColor fore)
{
	printf("color fill\n");
	char *p = mapmem(MAPMEM_GPHYS | MAPMEM_WRITE, 0xB8000, 4096);
	for (u64 i = 0; i < 80 * 25; ++i)
		p[i * 2 + 1] = (back << 4) | fore;
	unmapmem(p, 4096);
	printf("done p=%lx\n", p);
}
// isSvga = false -> vga text
u8 VmwareSvgaSwitchTo(bool isSvga)
{
	WriteReg(0x1070 /*gpu address*/, SVGA_REG_ENABLE, isSvga /*false*/);
}

static enum ioact
my_kbd_handler (enum iotype type, u32 port, void *data)
{
	do_io_default (type, port, data);
	if (type == IOTYPE_INB)
	{
		if (*(u8*)data == 41) // tilde
		{
			printf("switching\n");
			VmwareSvgaSwitchTo(false);
			VmwareSvgaColorFill(VGA_COLOR_BLUE, VGA_COLOR_WHITE);
			VmwareSvgaPutString("hello", 0, 0);
		}
		// printf("scancode %d\n", *(u8*)data);
	}
	return IOACT_CONT;
}

static void
setiohooks (void)
{	
// 	if (current->vcpu0 != current)
// 		return;

// 	set_iofunc (0x60, my_kbd_handler);

// #ifdef DEBUG_IO_MONITOR
// 	set_iofunc (0x60, kbdio_monitor);
// #endif
// #ifdef DEBUG_IO0x20_MONITOR
// 	set_iofunc (0x20, io0x20_monitor);
// #endif
// #if defined (F11PANIC) || defined (F12MSG)
// 	if (config.vmm.f11panic || config.vmm.f12msg)
// 		set_iofunc (0x60, kbdio_dbg_monitor);
// #endif
}

INITFUNC ("pass1", setiohooks);
