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

static void disassemble(size_t byte_count, u8 *buffer)
{
    fprintf(stdout, opening);

    for(int i = 0; i < byte_count; i+=2)
    {
        Instruction inst = {};

        u8 op = buffer[i] & opcode_mask;
        if((op ^ mov_bits) == 0) fprintf(stdout, mov);

        u8 dest = buffer[i] & d_mask;
        if((dest ^ d_bit) == 0) inst.d = true;
        else inst.d = false;

        u8 wide = buffer[i] & w_mask;
        if((wide ^ w_bit) == 0) inst.w = true;
        else inst.w = false;

        u8 mode = buffer[i+1] & mod_mask;
        if((mode ^ mem_no_disp_bits) == 0) inst.mod = MEM_NO_DISP;
        if((mode ^ mem_byte_disp_bits) == 0) inst.mod = MEM_BYTE_DISP;
        if((mode ^ mem_word_disp_bits) == 0) inst.mod = MEM_WORD_DISP;
        if((mode ^ reg_no_disp_bits) == 0) inst.mod = REG_NO_DISP;

        if(inst.mod == REG_NO_DISP)
        {
            if(inst.d)
            {
                if(inst.w)
                {
                    u8 reg = buffer[i+1] & reg_mask;
                    if((reg ^ al_ax_bits) == 0) fprintf(stdout, al);
                    if((reg ^ cl_cx_bits) == 0) fprintf(stdout, cl);
                    if((reg ^ dl_dx_bits) == 0) fprintf(stdout, dl);
                    if((reg ^ bl_bx_bits) == 0) fprintf(stdout, bl);
                    if((reg ^ ah_sp_bits) == 0) fprintf(stdout, ah);
                    if((reg ^ ch_bp_bits) == 0) fprintf(stdout, ch);
                    if((reg ^ dh_si_bits) == 0) fprintf(stdout, dh);
                    if((reg ^ bh_di_bits) == 0) fprintf(stdout, bh);
                }
                else 
                {
                    u8 reg = buffer[i+1] & reg_mask;
                    if((reg ^ al_ax_bits) == 0) fprintf(stdout, ax);
                    if((reg ^ cl_cx_bits) == 0) fprintf(stdout, cx);
                    if((reg ^ dl_dx_bits) == 0) fprintf(stdout, dx);
                    if((reg ^ bl_bx_bits) == 0) fprintf(stdout, bx);
                    if((reg ^ ah_sp_bits) == 0) fprintf(stdout, sp);
                    if((reg ^ ch_bp_bits) == 0) fprintf(stdout, bp);
                    if((reg ^ dh_si_bits) == 0) fprintf(stdout, si);
                    if((reg ^ bh_di_bits) == 0) fprintf(stdout, di);
                }

                fprintf(stdout, comma);

                if(inst.w)
                {
                    u8 reg_mem = buffer[i+1] & rm_mask;
                    if((reg_mem ^ rm_al_ax_bits) == 0) fprintf(stdout, al);
                    if((reg_mem ^ rm_cl_cx_bits) == 0) fprintf(stdout, cl);
                    if((reg_mem ^ rm_dl_dx_bits) == 0) fprintf(stdout, dl);
                    if((reg_mem ^ rm_bl_bx_bits) == 0) fprintf(stdout, bl);
                    if((reg_mem ^ rm_ah_sp_bits) == 0) fprintf(stdout, ah);
                    if((reg_mem ^ rm_ch_bp_bits) == 0) fprintf(stdout, ch);
                    if((reg_mem ^ rm_dh_si_bits) == 0) fprintf(stdout, dh);
                    if((reg_mem ^ rm_bh_di_bits) == 0) fprintf(stdout, bh);
                }
                else
                {
                    u8 reg_mem = buffer[i+1] & rm_mask;
                    if((reg_mem ^ rm_al_ax_bits) == 0) fprintf(stdout, ax);
                    if((reg_mem ^ rm_cl_cx_bits) == 0) fprintf(stdout, cx);
                    if((reg_mem ^ rm_dl_dx_bits) == 0) fprintf(stdout, dx);
                    if((reg_mem ^ rm_bl_bx_bits) == 0) fprintf(stdout, bx);
                    if((reg_mem ^ rm_ah_sp_bits) == 0) fprintf(stdout, sp);
                    if((reg_mem ^ rm_ch_bp_bits) == 0) fprintf(stdout, bp);
                    if((reg_mem ^ rm_dh_si_bits) == 0) fprintf(stdout, si);
                    if((reg_mem ^ rm_bh_di_bits) == 0) fprintf(stdout, di);
                }
            }
            else
            {
                if(inst.w)
                {
                    u8 reg_mem = buffer[i+1] & rm_mask;
                    if((reg_mem ^ rm_al_ax_bits) == 0) fprintf(stdout, al);
                    if((reg_mem ^ rm_cl_cx_bits) == 0) fprintf(stdout, cl);
                    if((reg_mem ^ rm_dl_dx_bits) == 0) fprintf(stdout, dl);
                    if((reg_mem ^ rm_bl_bx_bits) == 0) fprintf(stdout, bl);
                    if((reg_mem ^ rm_ah_sp_bits) == 0) fprintf(stdout, ah);
                    if((reg_mem ^ rm_ch_bp_bits) == 0) fprintf(stdout, ch);
                    if((reg_mem ^ rm_dh_si_bits) == 0) fprintf(stdout, dh);
                    if((reg_mem ^ rm_bh_di_bits) == 0) fprintf(stdout, bh);
                }
                else
                {
                    u8 reg_mem = buffer[i+1] & rm_mask;
                    if((reg_mem ^ rm_al_ax_bits) == 0) fprintf(stdout, ax);
                    if((reg_mem ^ rm_cl_cx_bits) == 0) fprintf(stdout, cx);
                    if((reg_mem ^ rm_dl_dx_bits) == 0) fprintf(stdout, dx);
                    if((reg_mem ^ rm_bl_bx_bits) == 0) fprintf(stdout, bx);
                    if((reg_mem ^ rm_ah_sp_bits) == 0) fprintf(stdout, sp);
                    if((reg_mem ^ rm_ch_bp_bits) == 0) fprintf(stdout, bp);
                    if((reg_mem ^ rm_dh_si_bits) == 0) fprintf(stdout, si);
                    if((reg_mem ^ rm_bh_di_bits) == 0) fprintf(stdout, di);
                }

                fprintf(stdout, comma);

                if(inst.w)
                {
                    u8 reg = buffer[i+1] & reg_mask;
                    if((reg ^ al_ax_bits) == 0) fprintf(stdout, al);
                    if((reg ^ cl_cx_bits) == 0) fprintf(stdout, cl);
                    if((reg ^ dl_dx_bits) == 0) fprintf(stdout, dl);
                    if((reg ^ bl_bx_bits) == 0) fprintf(stdout, bl);
                    if((reg ^ ah_sp_bits) == 0) fprintf(stdout, ah);
                    if((reg ^ ch_bp_bits) == 0) fprintf(stdout, ch);
                    if((reg ^ dh_si_bits) == 0) fprintf(stdout, dh);
                    if((reg ^ bh_di_bits) == 0) fprintf(stdout, bh);
                }
                else 
                {
                    u8 reg = buffer[i+1] & reg_mask;
                    if((reg ^ al_ax_bits) == 0) fprintf(stdout, ax);
                    if((reg ^ cl_cx_bits) == 0) fprintf(stdout, cx);
                    if((reg ^ dl_dx_bits) == 0) fprintf(stdout, dx);
                    if((reg ^ bl_bx_bits) == 0) fprintf(stdout, bx);
                    if((reg ^ ah_sp_bits) == 0) fprintf(stdout, sp);
                    if((reg ^ ch_bp_bits) == 0) fprintf(stdout, bp);
                    if((reg ^ dh_si_bits) == 0) fprintf(stdout, si);
                    if((reg ^ bh_di_bits) == 0) fprintf(stdout, di);
                }
            }
        }

        fprintf(stdout, end_of_inst);
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
        assert(bytes_read > 0);
        disassemble(bytes_read, buffer);
    }
}