#pragma once

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;
typedef long long unsigned u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef s32 b32;

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

u8 first_four_mask = 0xf0;
u8 first_six_mask = 0xfc;
u8 first_seven_mask = 0xfe;
u8 d_mask, d_bit = 0x2;
u8 w_mask, w_bit = 0x1;
u8 mod_mask = 0xc0;
u8 reg_mask = 0x38;
u8 rm_mask = 0x7;

u8 mov_ir_bits = 0xb0;
u8 mov_irm_bits = 0xc6;
u8 mov_ma_bits = 0xa0;
u8 mov_am_bits = 0xa2;
u8 mov_rmr_bits = 0x88;

u8 mem_no_disp_bits = 0x0;
u8 mem_byte_disp_bits = 0x40;
u8 mem_word_disp_bits = 0x80;
u8 reg_no_disp_bits = 0xc0;

u8 al_ax_bits = 0x0;
u8 cl_cx_bits = 0x8;
u8 dl_dx_bits = 0x10;
u8 bl_bx_bits = 0x18;
u8 ah_sp_bits = 0x20;
u8 ch_bp_bits = 0x28;
u8 dh_si_bits = 0x30;
u8 bh_di_bits = 0x38;

u8 rm_al_ax_bits = al_ax_bits >> 3;
u8 rm_cl_cx_bits = cl_cx_bits >> 3;
u8 rm_dl_dx_bits = dl_dx_bits >> 3;
u8 rm_bl_bx_bits = bl_bx_bits >> 3;
u8 rm_ah_sp_bits = ah_sp_bits >> 3;
u8 rm_ch_bp_bits = ch_bp_bits >> 3;
u8 rm_dh_si_bits = dh_si_bits >> 3;
u8 rm_bh_di_bits = bh_di_bits >> 3;

char *opening = "bits 16\n\n";
char *comma = ", ";
char *mov = "mov ";
char *al = "al";
char *ax = "ax";
char *cl = "cl";
char *cx = "cx";
char *dl = "dl";
char *dx = "dx";
char *bl = "bl";
char *bx = "bx";
char *ah = "ah";
char *sp = "sp";
char *ch = "ch";
char *bp = "bp";
char *dh = "dh";
char *si = "si";
char *bh = "bh";
char *di = "di";
char *end_of_inst = "\n";

enum Opcode
{
    MOV
};

enum Mod
{
    MEM_NO_DISP,
    MEM_BYTE_DISP,
    MEM_WORD_DISP,
    REG_NO_DISP
};

enum Reg
{
    AL,
    CL,
    DL,
    BL,
    AH,
    CH,
    DH,
    BH,
    AX,
    CX,
    DX,
    BX,
    SP,
    BP,
    SI,
    DI
};

struct Instruction
{
    Opcode op;
    b32 d;
    b32 w;
    Mod mod;
    Reg reg;
    Reg rm;
};