#include "8086asm.h"

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

size_t max_bytes  = 256;

static size_t load_memory_from_file(char *filename, u8 *buffer)
{
    size_t result = 0;

    FILE *file = fopen(filename, "rb");
    if(file)
    {
        result = fread(buffer, 1, max_bytes, file);
        fprintf(stdout, "Bytes Read: %zu\n", result);
        fclose(file);
    }
    else
    {
        fprintf(stderr, "ERROR: Unable to open %s.\n", filename);
    }

    return result;
}

static void disassemble(size_t disasm_byte_count, u8 *buffer)
{
    // TODO: We need a way to parse and generate instructions one after the other, and to
    // generate output from that. Maybe just an array of instructions.
}

int main(int arg_count, char **args)
{
    u8 buffer[256];
    memset(buffer, 0, 256);

    if(arg_count > 1)
    {
        char *filename = args[1];
        fprintf(stdout, "Filename: %s\n", filename);
        size_t bytes_read = load_memory_from_file(filename, buffer);
        assert(bytes_read > 0);
        disassemble(bytes_read, buffer);
    }
}