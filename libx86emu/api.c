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
*   Public interface functions.
*
****************************************************************************/


#include <core.h>
#include "x86emu_int.h"

#define LINE_LEN 16

static void x86emu_rdmsr(x86emu_t *emu)
{
  ASSERT(false);
}

static void x86emu_wrmsr(x86emu_t *emu)
{
  ASSERT(false);
}

API_SYM void x86emu_new(x86emu_t *emu, unsigned def_mem_perm, unsigned def_io_perm, u8 *mem_type)
{
  // void ops_fix_all(void);
  // ops_fix_all();
  
  x86emu_set_memio_handler(emu, vm_memio);
  x86emu_set_rdmsr_handler(emu, x86emu_rdmsr);
  x86emu_set_wrmsr_handler(emu, x86emu_wrmsr);
  emu->mem_type = mem_type;

  x86emu_reset(emu);
}

API_SYM void x86emu_reset(x86emu_t *emu)
{
  x86emu_regs_t *x86 = &emu->x86;

  memset(x86, 0, sizeof *x86);

  x86->R_EFLG = 2;

  x86->R_CS_LIMIT = x86->R_DS_LIMIT = x86->R_SS_LIMIT = x86->R_ES_LIMIT =
  x86->R_FS_LIMIT = x86->R_GS_LIMIT = 0xffff;

  // resp. 0x4093/9b for 4GB
  x86->R_CS_ACC = 0x9b;
  x86->R_SS_ACC = x86->R_DS_ACC = x86->R_ES_ACC = x86->R_FS_ACC = x86->R_GS_ACC = 0x93;

  x86->R_CS = 0xf000;
  x86->R_CS_BASE = 0xf0000;
  x86->R_EIP = 0xfff0;

  x86->R_GDT_LIMIT = 0xffff;
  x86->R_IDT_LIMIT = 0xffff;
}


API_SYM x86emu_memio_handler_t x86emu_set_memio_handler(x86emu_t *emu, x86emu_memio_handler_t handler)
{
  x86emu_memio_handler_t old = NULL;

  if(emu) {
    old = emu->memio;
    emu->memio = handler;
  }

  return old;
}


API_SYM x86emu_intr_handler_t x86emu_set_intr_handler(x86emu_t *emu, x86emu_intr_handler_t handler)
{
  x86emu_intr_handler_t old = NULL;

  if(emu) {
    old = emu->intr;
    emu->intr = handler;
  }

  return old;
}


API_SYM x86emu_code_handler_t x86emu_set_code_handler(x86emu_t *emu, x86emu_code_handler_t handler)
{
  x86emu_code_handler_t old = NULL;

  if(emu) {
    old = emu->code_check;
    emu->code_check = handler;
  }

  return old;
}

API_SYM x86emu_cpuid_handler_t x86emu_set_cpuid_handler(x86emu_t *emu, x86emu_cpuid_handler_t handler)
{
  x86emu_cpuid_handler_t old = NULL;

  if(emu) {
    old = emu->cpuid;
    emu->cpuid = handler;
  }

  return old;
}

API_SYM x86emu_wrmsr_handler_t x86emu_set_wrmsr_handler(x86emu_t *emu, x86emu_wrmsr_handler_t handler)
{
  x86emu_wrmsr_handler_t old = NULL;

  if(emu) {
    old = emu->wrmsr;
    emu->wrmsr = handler;
  }

  return old;
}

API_SYM x86emu_rdmsr_handler_t x86emu_set_rdmsr_handler(x86emu_t *emu, x86emu_rdmsr_handler_t handler)
{
  x86emu_rdmsr_handler_t old = NULL;

  if(emu) {
    old = emu->rdmsr;
    emu->rdmsr = handler;
  }

  return old;
}

API_SYM unsigned x86emu_read_byte(x86emu_t *emu, unsigned addr)
{
  u32 val = 0xff;

  if(emu) emu->memio(emu, addr, &val, X86EMU_MEMIO_R | X86EMU_MEMIO_8);
 
  return val;
}


API_SYM unsigned x86emu_read_byte_noperm(x86emu_t *emu, unsigned addr)
{
  u32 val = 0xff;

  if(emu) emu->memio(emu, addr, &val, X86EMU_MEMIO_R | X86EMU_MEMIO_8_NOPERM);
 
  return val;
}


API_SYM unsigned x86emu_read_word(x86emu_t *emu, unsigned addr)
{
  u32 val = 0xffff;

  if(emu) emu->memio(emu, addr, &val, X86EMU_MEMIO_R | X86EMU_MEMIO_16);
 
  return val;
}


API_SYM unsigned x86emu_read_dword(x86emu_t *emu, unsigned addr)
{
  u32 val = 0xffffffff;

  if(emu) emu->memio(emu, addr, &val, X86EMU_MEMIO_R | X86EMU_MEMIO_32);
 
  return val;
}


API_SYM void x86emu_write_byte(x86emu_t *emu, unsigned addr, unsigned val)
{
  u32 val32 = val;

  if(emu) emu->memio(emu, addr, &val32, X86EMU_MEMIO_W | X86EMU_MEMIO_8);
}


API_SYM void x86emu_write_byte_noperm(x86emu_t *emu, unsigned addr, unsigned val)
{
  u32 val32 = val;

  if(emu) emu->memio(emu, addr, &val32, X86EMU_MEMIO_W | X86EMU_MEMIO_8_NOPERM);
}


API_SYM void x86emu_write_word(x86emu_t *emu, unsigned addr, unsigned val)
{
  u32 val32 = val;

  if(emu) emu->memio(emu, addr, &val32, X86EMU_MEMIO_W | X86EMU_MEMIO_16);
}


API_SYM void x86emu_write_dword(x86emu_t *emu, unsigned addr, unsigned val)
{
  u32 val32 = val;

  if(emu) emu->memio(emu, addr, &val32, X86EMU_MEMIO_W | X86EMU_MEMIO_32);
}

/*
 * flags:
 *   bits 0-7:
 *     0: show all initialized memory
 *     1: show only accessed memory
 *     2: show only invalidly accessed memory
 *
 *   bit 8: show ascii, too
 */
static void dump_data(x86emu_t *emu, unsigned char *data, unsigned char *attr, char *str_data, char *str_attr, int flags)
{
  unsigned u, u1, flag_ascii;
  char c;
  int ok = 0;
  char *sd = str_data, *sa = str_attr;
  char *ascii = str_data + 4 * LINE_LEN + 2;

  flag_ascii = flags & 0x100;
  flags &= 0xff;

  for(u = 0; u < LINE_LEN; u++) {
    *str_data++ = (attr[u] & X86EMU_ACC_INVALID) ? '*' : ' ';
    if(
      (flags == 0 && (attr[u] & X86EMU_PERM_VALID)) ||
      (flags == 1 && (attr[u] & (X86EMU_ACC_R | X86EMU_ACC_W | X86EMU_ACC_X | X86EMU_ACC_INVALID))) ||
      (flags == 2 && (attr[u] & X86EMU_ACC_INVALID))
    ) {
      ok = 1;
      decode_hex2(emu, &str_data, u1 = data[u]);

      c = (attr[u] & X86EMU_PERM_R) ? (attr[u] & X86EMU_ACC_R) ? 'R' : 'r' : ' ';
      *str_attr++ = c;
      c = (attr[u] & X86EMU_PERM_W) ? (attr[u] & X86EMU_ACC_W) ? 'W' : 'w' : ' ';
      *str_attr++ = c;
      c = (attr[u] & X86EMU_PERM_X) ? (attr[u] & X86EMU_ACC_X) ? 'X' : 'x' : ' ';
      *str_attr++ = c;

      if(u1 < 0x20 || u1 >= 0x7f) u1 = '.';
      ascii[u] = u1;
    }
    else {
      *str_data++ = ' ';
      *str_data++ = ' ';

      *str_attr++ = ' ';
      *str_attr++ = ' ';
      *str_attr++ = ' ';

      ascii[u] = ' ';
    }
    *str_data++ = ' ';
    *str_attr++ = ' ';
  }

  if(ok) {
    if(flag_ascii) {
      str_data[0] = ' ';
      str_data[1] = ' ';
      str_data += 2 + LINE_LEN;
    }
  }
  else {
    str_data = sd;
    str_attr = sa;
  }

  *str_data = *str_attr = 0;

  while(str_data > sd && str_data[-1] == ' ') *--str_data = 0;
  while(str_attr > sa && str_attr[-1] == ' ') *--str_attr = 0;
}

