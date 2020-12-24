#include <stdio.h>
#include <Zydis/Zydis.h>


int disassemble(char buffer[]) {

    buffer = buffer;

    ZyanU8 data[] =
    {
        0x51, 0x8D, 0x45, 0xFF, 0x50, 0xFF, 0x75, 0x0C, 0xFF, 0x75,
        0x08, 0xFF, 0x15, 0xA0, 0xA5, 0x48, 0x76, 0x85, 0xC0, 0x0F,
        0x88, 0xFC, 0xDA, 0x02, 0x00
    };

    ZydisDecoder decoder = { 0 };
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);

    ZydisFormatter formatter = { 0 };
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);


    ZyanU64 runtime_address = 0x4b0;
    ZyanUSize offset = 0;

    const ZyanUSize length = sizeof(data);
    ZydisDecodedInstruction insn;

    char buffer[256];
    while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, data + offset, length - offset,
        &insn)))
    {
        printf("%016lx ", runtime_address);
        ZydisFormatterFormatInstruction(&formatter, &insn, buffer, sizeof(buffer),\
            runtime_address);

        puts(buffer);

        offset += insn.length;
        runtime_address += insn.length;
    }

    return 0;
}
