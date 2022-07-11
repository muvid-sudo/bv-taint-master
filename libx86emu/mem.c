/****************************************************************************
*
* Realmode X86 Emulator Library
*
* Copyright (c) 2007-2017 SUSE LINUX GmbH; Author: Steffen Winterfeldt
*
*  ========================================================================
*
*  Permission to use, copy, modify, distribute, and sell this software and
*  its documentation for any purpose is hereby granted without fee,
*  provided that the above copyright notice appear in all copies and that
*  both that copyright notice and this permission notice appear in
*  supporting documentation, and that the name of the authors not be used
*  in advertising or publicity pertaining to distribution of the software
*  without specific, written prior permission.  The authors makes no
*  representations about the suitability of this software for any purpose.
*  It is provided "as is" without express or implied warranty.
*
*  THE AUTHORS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
*  EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
*  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
*  USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
*  OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
*  PERFORMANCE OF THIS SOFTWARE.
*
*  ========================================================================
*
* Description:
*   Memory and i/o access emulation functions.
*
****************************************************************************/

#include "include/x86emu_int.h"
#include "../core/cpu_mmu.h"

#define PERM16(a)	((a) + ((a) << 8))
#define PERM32(a)	(PERM16(a) + (PERM16(a) << 16))

// avoid unaligned memory accesses
#define STRICT_ALIGN	0

// char page[] = {0x03, 0x05, 0x00, 0x00, 0x00, 0x00, 0xf4};

// char* vm_get_page(unsigned addr)
// {
//   return page;
// }

unsigned vm_r_byte(unsigned addr)
{
  u8 data;
  ASSERT(read_linearaddr_b(addr, &data) == VMMERR_SUCCESS);
  return data;
  // char *page;
  // unsigned page_idx = addr & (X86EMU_PAGE_SIZE - 1);
  // unsigned char *perm;

  // page = vm_get_page(addr);
  // return page[page_idx];
}


unsigned vm_r_byte_noperm(unsigned addr)
{
  return vm_r_byte(addr);
  // char *page;
  // unsigned page_idx = addr & (X86EMU_PAGE_SIZE - 1);
  // // unsigned char *attr;

  // page = vm_get_page(addr);
  // // attr = page->attr + page_idx;

  // return page[page_idx];
}


unsigned vm_r_word(unsigned addr)
{
  u16 data;
  ASSERT(read_linearaddr_w(addr, &data) == VMMERR_SUCCESS);
  return data;
  // char *page;
  // unsigned val, page_idx = addr & (X86EMU_PAGE_SIZE - 1);
  // u16 *perm16;

  // page = vm_get_page(addr);

  // if(page_idx >= X86EMU_PAGE_SIZE - 1) {
  //   val = vm_r_byte(addr);
  //   val += vm_r_byte(addr + 1) << 8;

  //   return val;
  // }

  // val = *(u16 *) (page + page_idx);

  // return val;
}


unsigned vm_r_dword(unsigned addr)
{
  u32 data;
  ASSERT(read_linearaddr_l(addr, &data) == VMMERR_SUCCESS);
  return data;
  // char *page;
  // unsigned val, page_idx = addr & (X86EMU_PAGE_SIZE - 1);

  // page = vm_get_page(addr);

  // if(page_idx >= X86EMU_PAGE_SIZE - 3) {
  //   val = vm_r_byte(addr);
  //   val += vm_r_byte(addr + 1) << 8;
  //   val += vm_r_byte(addr + 2) << 16;
  //   val += vm_r_byte(addr + 3) << 24;

  //   return val;
  // }

  // val = *(u32 *) (page + page_idx);

  // return val;
}


unsigned vm_x_byte(unsigned addr)
{
  return vm_r_byte(addr);
  // char *page;
  // unsigned page_idx = addr & (X86EMU_PAGE_SIZE - 1);

  // page = vm_get_page(addr);
  // return page[page_idx];
}


unsigned vm_x_word(unsigned addr)
{
  return vm_r_word(addr);
  // return vm_x_byte(addr) + (vm_x_byte(addr + 1) << 8);
}


unsigned vm_x_dword(unsigned addr)
{
  return vm_r_dword(addr);
  // return vm_x_word(addr) + (vm_x_word(addr + 2) << 16);
}


void vm_w_byte(unsigned addr, unsigned val)
{
  ASSERT(write_linearaddr_b(addr, (u8)val) == VMMERR_SUCCESS);
  // char *page;
  // unsigned page_idx = addr & (X86EMU_PAGE_SIZE - 1);
  // page = vm_get_page(addr);
  // page[page_idx] = val;
}


void vm_w_byte_noperm(unsigned addr, unsigned val)
{
  vm_w_byte(addr, val);
}


void vm_w_word(unsigned addr, unsigned val)
{
  ASSERT(write_linearaddr_w(addr, (u16)val) == VMMERR_SUCCESS);
  // char *page;
  // unsigned page_idx = addr & (X86EMU_PAGE_SIZE - 1);

  // page = vm_get_page(addr);

  // if (page_idx >= X86EMU_PAGE_SIZE - 1) {
  //   vm_w_byte(addr, val);
  //   vm_w_byte(addr + 1, val >> 8);

  //   return;
  // }

  // *(u16 *) (page + page_idx) = val;
}


void vm_w_dword(unsigned addr, unsigned val)
{
  ASSERT(write_linearaddr_l(addr, (u32)val) == VMMERR_SUCCESS);

  // char *page;
  // unsigned page_idx = addr & (X86EMU_PAGE_SIZE - 1);

  // page = vm_get_page(addr);

  // if (page_idx >= X86EMU_PAGE_SIZE - 3) {
  //   vm_w_byte(addr, val);
  //   vm_w_byte(addr + 1, val >> 8);
  //   vm_w_byte(addr + 2, val >> 16);
  //   vm_w_byte(addr + 3, val >> 24);

  //   return;
  // }
  // *(u32 *) (page + page_idx) = val;
}

unsigned vm_memio(x86emu_t *emu, u32 addr, u32 *val, unsigned type)
{
  unsigned bits = type & 0xff;

  type &= ~0xff;

  switch(type) {
    case X86EMU_MEMIO_R:
      switch(bits) {
        case X86EMU_MEMIO_8:
          *val = vm_r_byte(addr);
          break;
        case X86EMU_MEMIO_16:
          *val = vm_r_word(addr);
          break;
        case X86EMU_MEMIO_32:
          *val = vm_r_dword(addr);
          break;
        case X86EMU_MEMIO_8_NOPERM:
          *val = vm_r_byte_noperm(addr);
          break;
      }
      break;

    case X86EMU_MEMIO_W:
      switch(bits) {
        case X86EMU_MEMIO_8:
          vm_w_byte(addr, *val);
          break;
        case X86EMU_MEMIO_16:
          vm_w_word(addr, *val);
          break;
        case X86EMU_MEMIO_32:
          vm_w_dword(addr, *val);
          break;
        case X86EMU_MEMIO_8_NOPERM:
          vm_w_byte_noperm(addr, *val);
          break;
      }
      break;

    case X86EMU_MEMIO_X:
      switch(bits) {
        case X86EMU_MEMIO_8:
          *val = vm_x_byte(addr);
          break;
        case X86EMU_MEMIO_16:
          *val = vm_x_word(addr);
          break;
        case X86EMU_MEMIO_32:
          *val = vm_x_dword(addr);
          break;
      }
      break;

    case X86EMU_MEMIO_I:
      ASSERT(false);
      break;

    case X86EMU_MEMIO_O:
      ASSERT(false);
      break;
  }

  return 0;
}

