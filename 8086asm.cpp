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

static void wide_check(Instruction *inst, u8 *buffer, int i, b32 ir)
{
    if(!ir)
    {
        u8 wide = buffer[i] & w_mask;
        if((wide ^ w_bit) == 0) inst->w = true;
        else inst->w = false;
        #if 0
        fprintf(stdout, "\n\nu8 wide = %x\n", wide);
        fprintf(stdout, "w_mask = %x\n", w_mask);
        fprintf(stdout, "w_bit = %x\n", w_bit);
        fprintf(stdout, "\n\ninst->w = %u\n", inst->w);
        #endif
    }
    else if(ir)
    {
        u8 wide = buffer[i] & fifth_bit_mask;
        if((wide ^ fifth_bit_mask) == 0) inst->w = true;
        else inst->w = false;
    }
}

static void dest_check(Instruction *inst, u8 *buffer, int i)
{
    u8 dest = buffer[i] & d_mask;
    if((dest ^ d_bit) == 0) inst->d = true;
    else inst->d = false;
}

static void sign_ext_check(Instruction *inst, u8 *buffer, int i)
{
    u8 sign_ext = buffer[i] & s_mask;
    if((sign_ext ^ s_bit) == 0) inst->s = true;
    else inst->s = false;
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
        u8 reg = buffer[i] & mask;
        if((reg ^ (al_ax_bits >> shift)) == 0) fprintf(stdout, ax);
        if((reg ^ (cl_cx_bits >> shift)) == 0) fprintf(stdout, cx);
        if((reg ^ (dl_dx_bits >> shift)) == 0) fprintf(stdout, dx);
        if((reg ^ (bl_bx_bits >> shift)) == 0) fprintf(stdout, bx);
        if((reg ^ (ah_sp_bits >> shift)) == 0) fprintf(stdout, sp);
        if((reg ^ (ch_bp_bits >> shift)) == 0) fprintf(stdout, bp);
        if((reg ^ (dh_si_bits >> shift)) == 0) fprintf(stdout, si);
        if((reg ^ (bh_di_bits >> shift)) == 0) fprintf(stdout, di);
    }
    else 
    {
        u8 reg = buffer[i] & mask;
        if((reg ^ (al_ax_bits >> shift)) == 0) fprintf(stdout, al);
        if((reg ^ (cl_cx_bits >> shift)) == 0) fprintf(stdout, cl);
        if((reg ^ (dl_dx_bits >> shift)) == 0) fprintf(stdout, dl);
        if((reg ^ (bl_bx_bits >> shift)) == 0) fprintf(stdout, bl);
        if((reg ^ (ah_sp_bits >> shift)) == 0) fprintf(stdout, ah);
        if((reg ^ (ch_bp_bits >> shift)) == 0) fprintf(stdout, ch);
        if((reg ^ (dh_si_bits >> shift)) == 0) fprintf(stdout, dh);
        if((reg ^ (bh_di_bits >> shift)) == 0) fprintf(stdout, bh);
    }
}

static void rm_check(Instruction *inst, u8 *buffer, int i)
{
    u8 rm = buffer[i] & last_three_mask;
    if((rm ^ zzz) == 0) fprintf(stdout, "[bx + si"); 
    if((rm ^ zzo) == 0) fprintf(stdout, "[bx + di"); 
    if((rm ^ zoz) == 0) fprintf(stdout, "[bp + si"); 
    if((rm ^ zoo) == 0) fprintf(stdout, "[bp + di"); 
    if((rm ^ ozz) == 0) fprintf(stdout, "[si"); 
    if((rm ^ ozo) == 0) fprintf(stdout, "[di");  
    if((rm ^ ooz) == 0) fprintf(stdout, "[bp");
    if((rm ^ ooo) == 0) fprintf(stdout, "[bx"); 
}

static void direct_address_check(Instruction *inst, u8 *buffer, int i)
{
    u8 rm = buffer[i] & last_three_mask;
    if((inst->mod == MEM_NO_DISP) && ((rm ^ ooz) == 0)) inst->directaddress = true;
}

static void rm_disp(Instruction *inst, u8 *buffer, int i)
{
    if(inst->mod == MEM_BYTE_DISP)
    {
        s8 disp = (s8)buffer[i];
        if(disp != 0)
        {
            if(disp < 0)
            {
                fprintf(stdout, " - %d]", (disp*-1));
            }
            else
            {
                fprintf(stdout, " + %d]", disp);
            }
        }
        else
        {
            fprintf(stdout, "]");
        }
    }
    else if (inst->mod == MEM_WORD_DISP)
    {
        s16 disp;
        memcpy(&disp, buffer + i, sizeof(disp));
        if(disp < 0)
        {
            fprintf(stdout, " - %d]", (disp*-1));
        }
        else
        {
            fprintf(stdout, " + %d]", disp);
        }
    }
}

static int rmr(Instruction *inst, u8 *buffer, int index)
{
    int i = index;
    dest_check(inst, buffer, i);
    wide_check(inst, buffer, i, false);
    mod_check(inst, buffer, i + 1);
    direct_address_check(inst, buffer, i + 1);

    if (inst->mod == REG_NO_DISP)
    {
        if (inst->d)
        {
            reg_check(inst, 0, reg_mask, buffer, i + 1);
            fprintf(stdout, comma);
            reg_check(inst, 3, rm_mask, buffer, i + 1);
        }
        else
        {
            reg_check(inst, 3, rm_mask, buffer, i + 1);
            fprintf(stdout, comma);
            reg_check(inst, 0, reg_mask, buffer, i + 1);
        }
    }

    else if (inst->mod == MEM_NO_DISP)
    {
        if (inst->directaddress)
        {
            reg_check(inst, 0, reg_mask, buffer, i + 1);
            fprintf(stdout, comma);

            if (!inst->w)
            {
                u8 data = buffer[i + 2];
                fprintf(stdout, "[%u]", data);
            }
            else if (inst->w)
            {
                u16 data;
                memcpy(&data, buffer + (i + 2), sizeof(data));
                fprintf(stdout, "[%u]", data);
                ++i; // three byte instruction since w=1
            }
        }
        else if (inst->d)
        {
            reg_check(inst, 0, reg_mask, buffer, i + 1);
            fprintf(stdout, comma);
            rm_check(inst, buffer, i + 1);
            fprintf(stdout, "]");
        }
        else
        {
            rm_check(inst, buffer, i + 1);
            fprintf(stdout, "]");
            fprintf(stdout, comma);
            reg_check(inst, 0, reg_mask, buffer, i + 1);
        }
    }

    else if (inst->mod == MEM_BYTE_DISP || inst->mod == MEM_WORD_DISP)
    {
        if (inst->d)
        {
            reg_check(inst, 0, reg_mask, buffer, i + 1);
            fprintf(stdout, comma);
            rm_check(inst, buffer, i + 1);
            rm_disp(inst, buffer, i + 2);
            ++i; // one byte disp
            if (inst->mod == MEM_WORD_DISP)
                ++i; // two byte disp
        }
        else
        {
            rm_check(inst, buffer, i + 1);
            rm_disp(inst, buffer, i + 2);
            fprintf(stdout, comma);
            reg_check(inst, 0, reg_mask, buffer, i + 1);
            ++i; // one byte disp
            if (inst->mod == MEM_WORD_DISP)
                ++i; // two byte disp
        }
    }

    fprintf(stdout, end_of_inst);
    ++i; // two byte instruction
    return(i);
}

//TODO: Sign extension
static int irm(Instruction *inst, u8 *buffer, int index)
{
    int i = index;
    if(inst->op == ADD)
    {
        sign_ext_check(inst, buffer, i);
    }
    wide_check(inst, buffer, i, false);
    mod_check(inst, buffer, i + 1);

    if (inst->mod == REG_NO_DISP)
    {
        fprintf(stdout, "\nadd irm reg no disp not implemented\n");
    }

    if (inst->mod == MEM_NO_DISP)
    {
        rm_check(inst, buffer, i + 1);
        fprintf(stdout, "]");
    }

    if (inst->mod == MEM_BYTE_DISP || inst->mod == MEM_WORD_DISP)
    {
        rm_check(inst, buffer, i + 1);
        rm_disp(inst, buffer, i + 2);
        i++; // one byte disp plus at least one byte of disp
        if (inst->mod == MEM_WORD_DISP)
            ++i; // two byte disp
    }

    fprintf(stdout, comma);

    if (!inst->w)
    {
        fprintf(stdout, "byte ");
        u8 data = buffer[i + 2];
        fprintf(stdout, "%u", data);
    }
    else if (inst->w)
    {
        fprintf(stdout, "word ");
        u16 data;
        memcpy(&data, buffer + (i + 2), sizeof(data));
        fprintf(stdout, "%u", data);
        ++i; // three byte instruction since w=1
    }

    fprintf(stdout, end_of_inst);
    i = i + 2;
    return(i);
}

static void disassemble(size_t byte_count, u8 *buffer)
{
    fprintf(stdout, opening);

    for(int i = 0; i < byte_count; ++i)
    {
        Instruction inst = {};
        u8 op;

        op = buffer[i] & first_four_mask;
        if((op ^ mov_ir_bits) == 0) 
        {
            inst.op = MOV;
            fprintf(stdout, mov);

            wide_check(&inst, buffer, i, true);
            reg_check(&inst, 3, last_three_mask, buffer, i);
            fprintf(stdout, comma);
            if(!inst.w)
            {
                u8 data = buffer[i+1];
                fprintf(stdout, "%u", data);
            }
            else if (inst.w)
            {
                u16 data;
                memcpy(&data, buffer + i+1, sizeof(data));
                fprintf(stdout, "%u", data);
                ++i; // three byte instruction since w=1
            }
            ++i; // it was at least a two byte insturction
            fprintf(stdout, end_of_inst);
            continue;
        }

        op = buffer[i] & first_six_mask;

        if((op ^ add_rmr_bits) == 0)
        {
            fprintf(stdout, add);
            inst.op = ADD;
            i = rmr(&inst, buffer, i);
            continue;
        }

        if((op ^ add_irm_bits) == 0)
        {
            fprintf(stdout, add);
            inst.op = ADD;
            i = irm(&inst, buffer, i);
            continue;
        }

        if((op ^ mov_rmr_bits) == 0) 
        {
            fprintf(stdout, mov);
            inst.op = MOV;
            i = rmr(&inst, buffer, i);
            continue;
        }

        op = buffer[i] & first_seven_mask;

        if((op ^ add_ia_bits) == 0)
        {
            fprintf(stdout, add);
            inst.op = ADD;

            wide_check(&inst, buffer, i, false);
            fprintf(stdout, "ax, ");

            if(!inst.w)
            {
                u8 data = buffer[i+1];
                fprintf(stdout, "[%u]", data);
            }
            else if (inst.w)
            {
                u16 data;
                memcpy(&data, buffer + (i+1), sizeof(data));
                fprintf(stdout, "[%u]", data);
                ++i; // three byte instruction since w=1
            }
            fprintf(stdout, end_of_inst);
            ++i;

            continue;
        }

        if((op ^ mov_irm_bits) == 0)
        {
            fprintf(stdout, mov);
            inst.op = MOV;
            i = irm(&inst, buffer, i);
            continue;
        }

        if((op ^ mov_ma_bits) == 0)
        {
            fprintf(stdout, mov);
            inst.op = MOV;

            wide_check(&inst, buffer, i, false);
            fprintf(stdout, "ax, ");

            if(!inst.w)
            {
                u8 data = buffer[i+1];
                fprintf(stdout, "[%u]", data);
            }
            else if (inst.w)
            {
                u16 data;
                memcpy(&data, buffer + (i+1), sizeof(data));
                fprintf(stdout, "[%u]", data);
                ++i; // three byte instruction since w=1
            }
            fprintf(stdout, end_of_inst);
            ++i;
            continue;
        }

        if((op ^ mov_am_bits) == 0)
        {
            fprintf(stdout, mov);
            inst.op = MOV;

            wide_check(&inst, buffer, i, false);
            if(!inst.w)
            {
                u8 data = buffer[i+1];
                fprintf(stdout, "[%u]", data);
            }
            else if (inst.w)
            {
                u16 data;
                memcpy(&data, buffer + (i+1), sizeof(data));
                fprintf(stdout, "[%u]", data);
                ++i; // three byte instruction since w=1
            }
            fprintf(stdout, ", ax");
            fprintf(stdout, end_of_inst);
            ++i;
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