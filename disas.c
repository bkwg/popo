#include <stdio.h>
#include <Zydis/Zydis.h>
#include <sys/user.h>
#include "disas.h"
#include "popo_types.h"

static void popo_print_instruction(U64 address, void* insn)
{
    ZydisFormatter formatter = { 0 };
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

    char insn_buffer[256] = { 0 };
    ZydisFormatterFormatInstruction(
        &formatter,
        (ZydisDecodedInstruction*)insn,
        insn_buffer,
        sizeof(insn_buffer),
        address);

    printf("%016lx\t", address);
    puts(insn_buffer);
}

static void popo_init_disassembler(void* disassembler, U32 mode, U32 address_width)
{
    ZydisDecoderInit((ZydisDecoder*)disassembler, mode, address_width);
}

int popo_disassemble(U64 pc, char buffer[], void* insn)
{
    ZydisDecoder decoder    = { 0 };
    popo_init_disassembler(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);

    ZyanU8* data            = (ZyanU8*)buffer;
    const ZyanUSize length  = sizeof(data);

    ZyanU64 runtime_address = pc;
    ZyanUSize offset        = 0;

    if (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, data + offset, length - offset,
        insn)))
    {
        popo_print_instruction(runtime_address, (void*)insn);
        //offset += insn.length;
        //runtime_address += insn.length;
    }

    return 0;
}
