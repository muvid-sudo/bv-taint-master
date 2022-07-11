#include "current.h"
#include "initfunc.h"
#include "mm.h"
#include "vmmcall.h"

#include "../libx86emu/include/x86emu.h"

static x86emu_t emulator;

static void 
copy_vm_to_emu (void)
{
    printf ("copy_vm_to_emu\n");

    // segment registers
    for (int i = R_ES_INDEX; i <= R_GS_INDEX; ++i)
    {
        current->vmctl.read_sreg_sel(i, &emulator.x86.seg[i].sel);
        current->vmctl.read_sreg_base(i, &emulator.x86.seg[i].base);
        current->vmctl.read_sreg_limit(i, &emulator.x86.seg[i].limit);
        current->vmctl.read_sreg_acr(i, &emulator.x86.seg[i].acc); // TODO: not sure it's correct
    }

    u64 value;

    // general purpose
    current->vmctl.read_general_reg(GENERAL_REG_RAX, &value);
    emulator.x86.gen.A.val.I32_reg.e_reg = (u32)value;

    current->vmctl.read_general_reg(GENERAL_REG_RBX, &value);
    emulator.x86.gen.B.val.I32_reg.e_reg = (u32)value;

    current->vmctl.read_general_reg(GENERAL_REG_RCX, &value);
    emulator.x86.gen.C.val.I32_reg.e_reg = (u32)value;

    current->vmctl.read_general_reg(GENERAL_REG_RDX, &value);
    emulator.x86.gen.D.val.I32_reg.e_reg = (u32)value;


    // special
    current->vmctl.read_general_reg(GENERAL_REG_RBP, &value);
    emulator.x86.spc.BP.val.I32_reg.e_reg = (u32)value;

    current->vmctl.read_general_reg(GENERAL_REG_RSP, &value);
    emulator.x86.spc.SP.val.I32_reg.e_reg = (u32)value;

    current->vmctl.read_general_reg(GENERAL_REG_RSI, &value);
    emulator.x86.spc.SI.val.I32_reg.e_reg = (u32)value;

    current->vmctl.read_general_reg(GENERAL_REG_RDI, &value);
    emulator.x86.spc.DI.val.I32_reg.e_reg = (u32)value;

    current->vmctl.read_ip(&value);
    emulator.x86.spc.IP.val.I32_reg.e_reg = (u32)value;

    current->vmctl.read_flags(&value);
    emulator.x86.spc.FLAGS = (u32)value;
}


static void 
copy_emu_to_vm (void)
{
    printf ("copy_emu_to_vm\n");

    // general purpose
    current->vmctl.write_general_reg(GENERAL_REG_RAX, emulator.x86.gen.A.val.I32_reg.e_reg);
    current->vmctl.write_general_reg(GENERAL_REG_RBX, emulator.x86.gen.B.val.I32_reg.e_reg);
    current->vmctl.write_general_reg(GENERAL_REG_RCX, emulator.x86.gen.C.val.I32_reg.e_reg);
    current->vmctl.write_general_reg(GENERAL_REG_RDX, emulator.x86.gen.D.val.I32_reg.e_reg);

    // special
    current->vmctl.write_general_reg(GENERAL_REG_RBX, emulator.x86.spc.BP.val.I32_reg.e_reg);
    current->vmctl.write_general_reg(GENERAL_REG_RSP, emulator.x86.spc.SP.val.I32_reg.e_reg);
    current->vmctl.write_general_reg(GENERAL_REG_RSI, emulator.x86.spc.SI.val.I32_reg.e_reg);
    current->vmctl.write_general_reg(GENERAL_REG_RDI, emulator.x86.spc.DI.val.I32_reg.e_reg);


    current->vmctl.write_ip(emulator.x86.spc.IP.val.I32_reg.e_reg);
    current->vmctl.write_flags(emulator.x86.spc.FLAGS);
}

static void 
track_it (void)
{
    copy_vm_to_emu ();
	printf ("starting emulation eip=%x\n", emulator.x86.R_EIP);
	emulator.max_instr = -1;
	x86emu_run (&emulator, X86EMU_RUN_MAX_INSTR | X86EMU_RUN_NO_EXEC | X86EMU_RUN_NO_CODE | X86EMU_RUN_LOOP);
	printf ("ending emulation eax=%d eip=%x\n", emulator.x86.R_EAX, emulator.x86.R_EIP);
    copy_emu_to_vm ();
}

static void set_taint(void)
{
    x86emu_t *emu = &emulator;
    u64 start, size, val;

    current->vmctl.read_general_reg(GENERAL_REG_RBX, &start);
    current->vmctl.read_general_reg(GENERAL_REG_RCX, &size);
    current->vmctl.read_general_reg(GENERAL_REG_RDX, &val);

    for (u64 i = 0; i < size; i += 8)
        MEM_TYPE_SET(start + i, val);
}

static void get_taint(void)
{
    x86emu_t *emu = &emulator;
    u64 start, size, val = 0;

    current->vmctl.read_general_reg(GENERAL_REG_RBX, &start);
    current->vmctl.read_general_reg(GENERAL_REG_RCX, &size);

    for (u64 i = 0; i < size; i += 8)
        val |= MEM_TYPE_GET(start + i);

    current->vmctl.write_general_reg(GENERAL_REG_RAX, val);
}

static void 
mem_track_init (void)
{
    printf ("mem_track_init\n");
    u64 mem_type_size = 4 * 1024 * 1024 * 1024 / 64;
    void *mem_type = alloc(mem_type_size);
    memset(mem_type, 0, mem_type_size);
    printf("mem_type=%lx\n", mem_type);
    x86emu_new(&emulator, X86EMU_PERM_R | X86EMU_PERM_W | X86EMU_PERM_X, 0, (u8*)mem_type);
	
    vmmcall_register ("track_it", track_it);
    vmmcall_register ("set_taint", set_taint);
    vmmcall_register ("get_taint", get_taint);

    i386_general_register *reg = 0;
    printf("marked offset = %d\n", &reg->marked);
}

INITFUNC ("vmmcal0", mem_track_init);