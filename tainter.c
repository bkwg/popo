#include <Zydis/Zydis.h>
#include <sys/user.h>
#include <stdio.h>
#include "tainter.h"
#include "disas.h"

char* op_type_map[] =  {
    "unused",
    "register",
    "memory",
    "pointer",
    "immediate"
};

char* op_action_map[] =  {
    "read",
    "written",
    "condread",
    "condwritten",
    "read & written",
    "condread & condwritten",
    "read & condwritten",
    "condread & written",
};

void print_actions(ZydisDecodedOperand operand, int position)
{
    if (operand.actions & ZYDIS_OPERAND_ACTION_MASK_READ)
        printf("n°%d is read\n", position);
    if (operand.actions & ZYDIS_OPERAND_ACTION_MASK_WRITE)
        printf("n°%d is written\n", position);
}

//void set_virtual_cpu(void)
//{
//
//}

PopoVCPU vcpu = { 0 };

void set_reg(ZyanI8 reg_id, ZydisRegisterWidth w)
{
    POPO_UNUSED(w);

    if (((long long int *)&vcpu)[reg_id])
    {
        printf("REGISTER %d TAINTED !!!!", reg_id);
    }

    switch (w)
    {
        case 64:
            ((U64 *)&vcpu)[reg_id] = 12;
            break;
        case 32:
        default:
            break;
    }
}

//registers[ZYDIS_REGISTER_MAX_VALUE];

void popo_set_register_taint(I8 reg_id, ZydisRegisterWidth w)
{
    switch (w)
    {
        case 8:
            break;
        case 16:
            break;
        case 32:
            break;
        case 64:
            break;
        default:
            break;
    }
}


void popo_print_VCPU(void)
{
    for (U8 i = 0; i < 16; ++i)
        printf("VCPU[%u]: %lld\n", i, ((long long int *)&vcpu)[i]);
}

void handle_mov(ZydisDecodedInstruction insn)
{
    ZydisDecodedOperand destination = insn.operands[0];
    ZydisDecodedOperand source      = insn.operands[1];

    //popo_set_register_taint();

    // destination can never be an immediate
    switch (source.type)
    {
        case ZYDIS_OPERAND_TYPE_IMMEDIATE:
            // If immediate:  DELETE THE TAINT
            if (destination.type == ZYDIS_OPERAND_TYPE_REGISTER)
            {
                ZydisRegister reg = destination.reg.value;
                //reg
                //ZydisRegisterGetId
                //ZydisRegisterGetString
                //ZydisRegisterGetWidth
                set_reg(ZydisRegisterGetId(reg), ZydisRegisterGetWidth(64, reg));
            }
        break;
        case ZYDIS_OPERAND_TYPE_MEMORY:
        case ZYDIS_OPERAND_TYPE_REGISTER:
            // TRANSFER TAINT
            printf("r/m -> transfer taint\n");
            if (destination.type == ZYDIS_OPERAND_TYPE_REGISTER)
            {
                ZydisRegister reg = destination.reg.value;
                printf("reg n°: %d\n", ZydisRegisterGetId(reg));
//                printf("width : %d\n", ZydisRegisterGetWidth(mode, reg));
            }
        break;
            break;
        default:
            break;
    }
}

PopoStatus popo_disas_and_taint(struct user_regs_struct* regs, char* buffer)
{
    ZydisDecodedInstruction insn = { 0 };
    popo_disassemble(regs->rip, buffer, &insn);

    switch (insn.mnemonic)
    {
        // For now we do not deal with CMOVs
        // TODO: deal with CMOVs
        case ZYDIS_MNEMONIC_MOV:
        case ZYDIS_MNEMONIC_MOVBE:
        case ZYDIS_MNEMONIC_MOVD:
        case ZYDIS_MNEMONIC_MOVQ:
        case ZYDIS_MNEMONIC_MOVSX:
        case ZYDIS_MNEMONIC_MOVSXD:
        case ZYDIS_MNEMONIC_MOVZX:
            printf("a move of interest spotted\n");
            handle_mov(insn);
            break;
        default:
            break;
    }

    //printf("nb of operands %d\n", insn.operand_count);
    for (int i = 0; i < insn.operand_count; ++i)
    {
        //For now we just deal with EXPLICIT operands.
        //TODO: handle implicit operands.
        if (insn.operands[i].visibility == ZYDIS_OPERAND_VISIBILITY_EXPLICIT)
        {
            print_actions(insn.operands[i], i);
            printf("operand type: %s\n", op_type_map[insn.operands[i].type]);
        }
    }

    puts("");

    return POPO_STATUS_SUCCESS;
}
