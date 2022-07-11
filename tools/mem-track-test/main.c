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

#include <windows.h>
#include <stdio.h>
#include "../common/call_vmm.h"

static unsigned get_vmcall_num(char *name)
{
	unsigned res;
	asm volatile ("vmcall"
				: "=a" (res)
				: "a" (0), "b" (name));
	return res;
}

int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
	unsigned arr[2] = { 4, 2};
	unsigned has_taint = 0;

	unsigned vmc_trackit = get_vmcall_num("track_it");
	unsigned vmc_set_taint = get_vmcall_num("set_taint");
	unsigned vmc_get_taint = get_vmcall_num("get_taint");

	asm volatile ("vmcall" : : "a" (vmc_set_taint), "b" (&arr[0]), "c" (4), "d" (1));
	asm volatile (	"vmcall					\n\t"
					"mov (%%ebx), %%eax		\n\t"
					"add $5, %%eax			\n\t"
					"add %%eax, (%%ecx)		\n\t"
					"hlt"
			      	: : "a" (vmc_trackit), "b" (&arr[0]), "c" (&arr[1]));					
	asm volatile ("vmcall" : "=a" (has_taint) : "a" (vmc_get_taint), "b" (&arr[1]), "c" (4));
	printf ("arr[1]=%d [%d]\n", arr[1], has_taint);
	
}


int WINAPI2 WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
	unsigned arr[2] = { 4, 2};
	unsigned has_taint = 0;

	unsigned vmc_trackit = get_vmcall_num("track_it");
	unsigned vmc_set_taint = get_vmcall_num("set_taint");
	unsigned vmc_get_taint = get_vmcall_num("get_taint");

	asm volatile ("vmcall" : : "a" (vmc_set_taint), "b" (&arr[0]), "c" (4), "d" (1));
	asm volatile (	"vmcall					\n\t"
					"mov (%%esp), %%edi		\n\t"
					"add $5, %%edi			\n\t"
					"add %%esp, (%%edi)		\n\t"
					"hlt"
			      	: : "a" (vmc_trackit), "b" (&arr[0]), "c" (&arr[1]));					
	asm volatile ("vmcall" : "=a" (has_taint) : "a" (vmc_get_taint), "b" (&arr[1]), "c" (4));
	printf ("arr[1]=%d [%d]\n", arr[1], has_taint);
	
}


int WINAPI3 WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
	unsigned arr[2] = { 4, 2};
	unsigned has_taint = 0;

	unsigned vmc_trackit = get_vmcall_num("track_it");
	unsigned vmc_set_taint = get_vmcall_num("set_taint");
	unsigned vmc_get_taint = get_vmcall_num("get_taint");

	asm volatile ("vmcall" : : "a" (vmc_set_taint), "b" (&arr[0]), "c" (4), "d" (1));
	asm volatile (	"vmcall					\n\t"
					"mov (%%esp), %%edi		\n\t"
					"add $5, %%edi			\n\t"
					"add %%esp, (%%edi)		\n\t"
					"hlt"
			      	: : "a" (vmc_trackit), "b" (&arr[0]), "c" (&arr[1]));					
	asm volatile ("vmcall" : "=a" (has_taint) : "a" (vmc_get_taint), "b" (&arr[1]), "c" (4));
	printf ("arr[1]=%d [%d]\n", arr[1], has_taint);
	
}
