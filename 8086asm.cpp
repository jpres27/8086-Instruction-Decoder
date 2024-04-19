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
        fclose(file);
    }
    else
    {
        fprintf(stderr, "ERROR: Unable to open %s.\n", filename);
    }

    return result;
}

static void wide_check(Instruction *inst, u8 *buffer, int i)
{
    u8 wide = buffer[i] & w_mask;
    if((wide ^ w_bit) == 0) inst->w = true;
    else inst->w = false;
}

static void dest_check(Instruction *inst, u8 *buffer, int i)
{
    u8 dest = buffer[i] & d_mask;
    if((dest ^ d_bit) == 0) inst->d = true;
    else inst->d = false;
}

static void mod_check(Instruction *inst, u8 *buffer, int i)
{
    u8 mode = buffer[i] & mod_mask;
    if((mode ^ mem_no_disp_bits) == 0) inst->mod = MEM_NO_DISP;
    if((mode ^ mem_byte_disp_bits) == 0) inst->mod = MEM_BYTE_DISP;
    if((mode ^ mem_word_disp_bits) == 0) inst->mod = MEM_WORD_DISP;
    if((mode ^ reg_no_disp_bits) == 0) inst->mod = REG_NO_DISP;
}

static void reg_check(Instruction *inst, u8 shift, u8 mask, u8 *buffer, int i)
{
    if(inst->w)
    {
        u8 reg = buffer[i+1] & mask;
        if((reg ^ (al_ax_bits >> shift)) == 0) fprintf(stdout, al);
        if((reg ^ (cl_cx_bits >> shift)) == 0) fprintf(stdout, cl);
        if((reg ^ (dl_dx_bits >> shift)) == 0) fprintf(stdout, dl);
        if((reg ^ (bl_bx_bits >> shift)) == 0) fprintf(stdout, bl);
        if((reg ^ (ah_sp_bits >> shift)) == 0) fprintf(stdout, ah);
        if((reg ^ (ch_bp_bits >> shift)) == 0) fprintf(stdout, ch);
        if((reg ^ (dh_si_bits >> shift)) == 0) fprintf(stdout, dh);
        if((reg ^ (bh_di_bits >> shift)) == 0) fprintf(stdout, bh);
    }
    else 
    {
        u8 reg = buffer[i+1] & mask;
        if((reg ^ (al_ax_bits >> shift)) == 0) fprintf(stdout, ax);
        if((reg ^ (cl_cx_bits >> shift)) == 0) fprintf(stdout, cx);
        if((reg ^ (dl_dx_bits >> shift)) == 0) fprintf(stdout, dx);
        if((reg ^ (bl_bx_bits >> shift)) == 0) fprintf(stdout, bx);
        if((reg ^ (ah_sp_bits >> shift)) == 0) fprintf(stdout, sp);
        if((reg ^ (ch_bp_bits >> shift)) == 0) fprintf(stdout, bp);
        if((reg ^ (dh_si_bits >> shift)) == 0) fprintf(stdout, si);
        if((reg ^ (bh_di_bits >> shift)) == 0) fprintf(stdout, di);
    }
}

static void disassemble(size_t byte_count, u8 *buffer)
{
    fprintf(stdout, opening);

    for(int i = 0; i < byte_count; ++i)
    {
        Instruction inst = {};
        u8 op;

        op = buffer[i] & first_four_mask;
        if(op ^ mov_ir_bits) 
        {
            fprintf(stdout, "ir\n");
            continue;
        }

        op = buffer[i] & first_six_mask;
        if((op ^ mov_rmr_bits) == 0) 
        {
            fprintf(stdout, mov);

            dest_check(&inst, buffer, i);
            wide_check(&inst, buffer, i);
            mod_check(&inst, buffer, i+1);

            if(inst.mod == REG_NO_DISP)
            {
                if(inst.d)
                {
                    reg_check(&inst, 0, reg_mask, buffer, i);
                    fprintf(stdout, comma);
                    reg_check(&inst, 3, rm_mask, buffer, i);
                }
                else
                {
                    reg_check(&inst, 3, rm_mask, buffer, i);
                    fprintf(stdout, comma);
                    reg_check(&inst, 0, reg_mask, buffer, i);
                }
            }
            ++i; // This was a two byte instruction so we increment twice to get to the next instruction
            fprintf(stdout, end_of_inst);
            continue;
        }

        op = buffer[i] & first_seven_mask;
        if((op ^ mov_irm_bits) == 0)
        {
            fprintf(stdout, "irm\n");
            continue;
        }

        if((op ^ mov_ma_bits) == 0)
        {
            fprintf(stdout, "ma\n");
            continue;
        }

        if((op ^ mov_am_bits) == 0)
        {
            fprintf(stdout, "am\n");
            continue;
        }
    }
}

int main(int arg_count, char **args)
{
    u8 buffer[256];
    memset(buffer, 0, 256);

    if(arg_count > 1)
    {
        char *filename = args[1];
        size_t bytes_read = load_memory_from_file(filename, buffer);
        fprintf(stdout, "Bytes read: %zu\n", bytes_read);
        disassemble(bytes_read, buffer);
    }
}