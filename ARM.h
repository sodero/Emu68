#ifndef _ARM_H
#define _ARM_H

/*
    Global registers:

    r0 - r9 - Free for emulator use

    r10 - Status Register of m68k
    r11 - the m68k context
    r12 - the Program Counter of m68k code
*/

#define REG_PC 12
#define REG_CTX 11
#define REG_SR 10

#define ARM_CC_EQ 0x00 /* Z=1 */
#define ARM_CC_NE 0x01 /* Z=0 */
#define ARM_CC_CS 0x02 /* C=1 */
#define ARM_CC_CC 0x03 /* C=0 */
#define ARM_CC_MI 0x04 /* N=1 */
#define ARM_CC_PL 0x05 /* N=0 */
#define ARM_CC_VS 0x06 /* V=1 */
#define ARM_CC_VC 0x07 /* V=0 */
#define ARM_CC_AL 0x0e /* Always */

/* Converts generated ARM instruction to little-endian */
static inline uint32_t INSN_TO_LE(uint32_t insn)
{
    union {
        uint32_t u32;
        uint8_t u8[4];
    } val;

    val.u8[3] = (insn >> 24) & 0xff;
    val.u8[2] = (insn >> 16) & 0xff;
    val.u8[1] = (insn >> 8) & 0xff;
    val.u8[0] = (insn)&0xff;

    return val.u32;
}

static inline uint32_t bx_lr()
{
    return INSN_TO_LE(0xe12fff1e);
}

static inline uint32_t udf(uint16_t immed)
{
    return INSN_TO_LE(0xe7f000f0 | (immed & 0x0f) | ((immed & 0xfff0) << 4));
}

static inline uint32_t mov_reg(uint8_t reg, uint8_t src)
{
    return INSN_TO_LE(0xe1a00000 | src | (reg << 12));
}

static inline uint32_t mov_immed_u8(uint8_t reg, uint8_t val)
{
    reg = reg & 0x0f;
    return INSN_TO_LE(0xe3a00000 | val | (reg << 12));
}

static inline uint32_t movs_immed_u8(uint8_t reg, uint8_t val)
{
    reg = reg & 0x0f;
    return INSN_TO_LE(0xe3a00000 | val | (reg << 12) | (1 << 20));
}

static inline uint32_t mov_immed_u8_shift(uint8_t reg, uint8_t val, uint8_t shift)
{
    reg = reg & 0x0f;
    shift &= 0x0f;
    return INSN_TO_LE(0xe3a00000 | val | (reg << 12) | (shift << 8));
}

static inline uint32_t movs_immed_u8_shift(uint8_t reg, uint8_t val, uint8_t shift)
{
    reg = reg & 0x0f;
    shift &= 0x0f;
    return INSN_TO_LE(0xe3a00000 | val | (reg << 12) | (1 << 20) | (shift << 8));
}

static inline uint32_t mvn_immed_u8(uint8_t reg, uint8_t val)
{
    reg = reg & 0x0f;
    return INSN_TO_LE(0xe3e00000 | val | (reg << 12));
}

static inline uint32_t mvns_immed_u8(uint8_t reg, uint8_t val)
{
    reg = reg & 0x0f;
    return INSN_TO_LE(0xe3e00000 | val | (reg << 12) | (1 << 20));
}

static inline uint32_t mvn_immed_u8_shift(uint8_t reg, uint8_t val, uint8_t shift)
{
    reg = reg & 0x0f;
    shift &= 0x0f;
    return INSN_TO_LE(0xe3e00000 | val | (reg << 12) | (shift << 8));
}

static inline uint32_t mvns_immed_u8_shift(uint8_t reg, uint8_t val, uint8_t shift)
{
    reg = reg & 0x0f;
    shift &= 0x0f;
    return INSN_TO_LE(0xe3e00000 | val | (reg << 12) | (1 << 20) | (shift << 8));
}

static inline uint32_t mov_immed_s8(uint8_t reg, int8_t val)
{
    reg = reg & 0x0f;
    if (val > 0)
        return mov_immed_u8(reg, (uint8_t)val);
    else
        return mvn_immed_u8(reg, (uint8_t)(-val - 1));
}

static inline uint32_t movs_immed_s8(uint8_t reg, int8_t val)
{
    reg = reg & 0x0f;
    if (val >= 0)
        return movs_immed_u8(reg, (uint8_t)val);
    else
        return mvns_immed_u8(reg, (uint8_t)(-val - 1));
}

static inline uint32_t bic_immed(uint8_t dest, uint8_t src, uint8_t mask)
{
    dest = dest & 15;
    src = src & 15;

    return INSN_TO_LE(0xe3c00000 | mask | (dest << 12) | (src << 16));
}

static inline uint32_t cmp_cc_immed(uint8_t cc, uint8_t src, uint8_t value)
{
    src = src & 15;

    return INSN_TO_LE(0x03500000 | (cc << 28) | (src << 16) | value);
}

static inline uint32_t cmp_immed(uint8_t src, uint8_t value)
{
    return cmp_cc_immed(ARM_CC_AL, src, value);
}

static inline uint32_t add_cc_immed(uint8_t cc, uint8_t dest, uint8_t src, uint8_t value)
{
    dest = dest & 15;
    src = src & 15;

    return INSN_TO_LE(0x02800000 | (cc << 28) | (dest << 12) | (src << 16) | value);
}

static inline uint32_t add_immed(uint8_t dest, uint8_t src, uint8_t value)
{
    return add_cc_immed(ARM_CC_AL, dest, src, value);
}

static inline uint32_t add_cc_reg(uint8_t cc, uint8_t dest, uint8_t src, uint8_t reg, uint8_t lsl)
{
    dest = dest & 15;
    src = src & 15;
    reg = reg & 15;
    lsl = lsl & 31;

    return INSN_TO_LE(0x00800000 | (cc << 28) | (dest << 12) | (src << 16) | reg | (lsl << 7));
}

static inline uint32_t add_reg(uint8_t dest, uint8_t src, uint8_t reg, uint8_t lsl)
{
    return add_cc_reg(ARM_CC_AL, dest, src, reg, lsl);
}

static inline uint32_t sub_cc_immed(uint8_t cc, uint8_t dest, uint8_t src, uint8_t value)
{
    dest = dest & 15;
    src = src & 15;

    return INSN_TO_LE(0x02400000 | (cc << 28) | (dest << 12) | (src << 16) | value);
}

static inline uint32_t sub_immed(uint8_t dest, uint8_t src, uint8_t value)
{
    return sub_cc_immed(ARM_CC_AL, dest, src, value);
}

static inline uint32_t or_cc_immed(uint8_t cc, uint8_t dest, uint8_t src, uint8_t value)
{
    return INSN_TO_LE(0x03800000 | (cc << 28) | (dest << 12) | (src << 16) | value);
}

static inline uint32_t or_immed(uint8_t dest, uint8_t src, uint8_t value)
{
    return or_cc_immed(ARM_CC_AL, dest, src, value);
}

static inline uint32_t lsl_cc_immed(uint8_t cc, uint8_t dest, uint8_t src, uint8_t value)
{
    return INSN_TO_LE(0x01a00000 | (cc << 28) | (dest << 12) | src | ((value & 0x1f) << 7));
}

static inline uint32_t lsl_immed(uint8_t dest, uint8_t src, uint8_t value)
{
    return lsl_cc_immed(ARM_CC_AL, dest, src, value);
}

static inline uint32_t lsr_cc_immed(uint8_t cc, uint8_t dest, uint8_t src, uint8_t value)
{
    return INSN_TO_LE(0x01a00020 | (cc << 28) | (dest << 12) | src | ((value & 0x1f) << 7));
}

static inline uint32_t lsr_immed(uint8_t dest, uint8_t src, uint8_t value)
{
    return lsr_cc_immed(ARM_CC_AL, dest, src, value);
}

static inline uint32_t str_cc_offset(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x05800000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x05000000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t str_offset(uint8_t dest, uint8_t src, int16_t offset)
{
    return str_cc_offset(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t str_cc_regoffset(uint8_t cc, uint8_t dest, uint8_t src, uint8_t reg, uint8_t shift)
{
    return INSN_TO_LE(0x07800000 | (cc << 28) | (dest << 16) | (src << 12) | reg | ((shift & 0x1f) << 7));
}

static inline uint32_t str_regoffset(uint8_t dest, uint8_t src, uint8_t reg, uint8_t shift)
{
    return str_cc_regoffset(ARM_CC_AL, dest, src, reg, shift);
}

static inline uint32_t str_cc_offset_preindex(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x05a00000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x05200000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t str_offset_preindex(uint8_t dest, uint8_t src, int16_t offset)
{
    return str_cc_offset_preindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t str_cc_offset_postindex(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x04800000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x04000000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t str_offset_postindex(uint8_t dest, uint8_t src, int16_t offset)
{
    return str_cc_offset_postindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldr_cc_offset(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x05900000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x05100000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t ldr_offset(uint8_t dest, uint8_t src, int16_t offset)
{
    return ldr_cc_offset(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldr_cc_offset_preindex(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x05b00000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x05300000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t ldr_offset_preindex(uint8_t dest, uint8_t src, int16_t offset)
{
    return ldr_cc_offset_preindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldr_cc_offset_postindex(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x04900000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x04100000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t ldr_offset_postindex(uint8_t dest, uint8_t src, int16_t offset)
{
    return ldr_cc_offset_postindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldr_cc_regoffset(uint8_t cc, uint8_t dest, uint8_t src, uint8_t reg, uint8_t shift)
{
    return INSN_TO_LE(0x07900000 | (cc << 28) | (dest << 16) | (src << 12) | reg | ((shift & 0x1f) << 7));
}

static inline uint32_t ldr_regoffset(uint8_t dest, uint8_t src, uint8_t reg, uint8_t shift)
{
    return ldr_cc_regoffset(ARM_CC_AL, dest, src, reg, shift);
}

static inline uint32_t ldrh_cc_offset(uint8_t cc, uint8_t dest, uint8_t src, int8_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x01d000b0 | (cc << 28) | (dest << 16) | (src << 12) | (offset & 0x0f) | ((offset << 4) & 0xf00));
    else
        return INSN_TO_LE(0x015000b0 | (cc << 28) | (dest << 16) | (src << 12) | (-offset & 0x0f) | ((-offset << 4) & 0xf00));
}

static inline uint32_t ldrh_offset(uint8_t dest, uint8_t src, int8_t offset)
{
    return ldrh_cc_offset(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldrh_cc_offset_preindex(uint8_t cc, uint8_t dest, uint8_t src, int8_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x01f000b0 | (cc << 28) | (dest << 16) | (src << 12) | (offset & 0x0f) | ((offset << 4) & 0xf00));
    else
        return INSN_TO_LE(0x017000b0 | (cc << 28) | (dest << 16) | (src << 12) | (-offset & 0x0f) | ((-offset << 4) & 0xf00));
}

static inline uint32_t ldrh_offset_preindex(uint8_t dest, uint8_t src, int8_t offset)
{
    return ldrh_cc_offset_preindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldrh_cc_offset_postindex(uint8_t cc, uint8_t dest, uint8_t src, int8_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x00d000b0 | (cc << 28) | (dest << 16) | (src << 12) | (offset & 0x0f) | ((offset << 4) & 0xf00));
    else
        return INSN_TO_LE(0x005000b0 | (cc << 28) | (dest << 16) | (src << 12) | (-offset & 0x0f) | ((-offset << 4) & 0xf00));
}

static inline uint32_t ldrh_offset_postindex(uint8_t dest, uint8_t src, int8_t offset)
{
    return ldrh_cc_offset_postindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldrh_cc_regoffset(uint8_t cc, uint8_t dest, uint8_t src, uint8_t offset)
{
    return INSN_TO_LE(0x019000b0 | (cc << 28) | (dest << 16) | (src << 12) | (offset & 0x0f));
}

static inline uint32_t ldrh_regoffset(uint8_t dest, uint8_t src, uint8_t offset)
{
    return ldrh_cc_regoffset(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t strh_cc_offset(uint8_t cc, uint8_t dest, uint8_t src, int8_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x01c000b0 | (cc << 28) | (dest << 16) | (src << 12) | (offset & 0x0f) | ((offset << 4) & 0xf00));
    else
        return INSN_TO_LE(0x014000b0 | (cc << 28) | (dest << 16) | (src << 12) | (-offset & 0x0f) | ((-offset << 4) & 0xf00));
}

static inline uint32_t strh_offset(uint8_t dest, uint8_t src, int8_t offset)
{
    return strh_cc_offset(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t strh_cc_offset_preindex(uint8_t cc, uint8_t dest, uint8_t src, int8_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x01e000b0 | (cc << 28) | (dest << 16) | (src << 12) | (offset & 0x0f) | ((offset << 4) & 0xf00));
    else
        return INSN_TO_LE(0x016000b0 | (cc << 28) | (dest << 16) | (src << 12) | (-offset & 0x0f) | ((-offset << 4) & 0xf00));
}

static inline uint32_t strh_offset_preindex(uint8_t dest, uint8_t src, int8_t offset)
{
    return strh_cc_offset_preindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t strh_cc_offset_postindex(uint8_t cc, uint8_t dest, uint8_t src, int8_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x00c000b0 | (cc << 28) | (dest << 16) | (src << 12) | (offset & 0x0f) | ((offset << 4) & 0xf00));
    else
        return INSN_TO_LE(0x004000b0 | (cc << 28) | (dest << 16) | (src << 12) | (-offset & 0x0f) | ((-offset << 4) & 0xf00));
}

static inline uint32_t strh_offset_postindex(uint8_t dest, uint8_t src, int8_t offset)
{
    return strh_cc_offset_postindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t strh_cc_regoffset(uint8_t cc, uint8_t dest, uint8_t src, uint8_t offset)
{
    return INSN_TO_LE(0x018000b0 | (cc << 28) | (dest << 16) | (src << 12) | (offset & 0x0f));
}

static inline uint32_t strh_regoffset(uint8_t dest, uint8_t src, uint8_t offset)
{
    return strh_cc_regoffset(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldrsh_cc_offset(uint8_t cc, uint8_t dest, uint8_t src, int8_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x01d000f0 | (cc << 28) | (dest << 16) | (src << 12) | (offset & 0x0f) | ((offset << 4) & 0xf00));
    else
        return INSN_TO_LE(0x015000f0 | (cc << 28) | (dest << 16) | (src << 12) | (-offset & 0x0f) | ((-offset << 4) & 0xf00));
}

static inline uint32_t ldrsh_offset(uint8_t dest, uint8_t src, int8_t offset)
{
    return ldrsh_cc_offset(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldrb_cc_offset(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x05d00000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x05500000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t ldrb_offset(uint8_t dest, uint8_t src, int16_t offset)
{
    return ldrb_cc_offset(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldrb_cc_offset_preindex(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x05f00000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x05700000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t ldrb_offset_preindex(uint8_t dest, uint8_t src, int16_t offset)
{
    return ldrb_cc_offset_preindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldrb_cc_offset_postindex(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x04d00000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x04500000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t ldrb_offset_postindex(uint8_t dest, uint8_t src, int16_t offset)
{
    return ldrb_cc_offset_postindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t ldrb_cc_regoffset(uint8_t cc, uint8_t dest, uint8_t src, uint8_t offset, uint8_t shift)
{
    return INSN_TO_LE(0x07d00000 | (cc << 28) | (dest << 16) | (src << 12) | offset | ((shift & 0x1f) << 7));
}

static inline uint32_t ldrb_regoffset(uint8_t dest, uint8_t src, uint8_t offset, uint8_t shift)
{
    return ldrb_cc_regoffset(ARM_CC_AL, dest, src, offset, shift);
}

static inline uint32_t strb_cc_offset(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x05c00000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x05400000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t strb_offset(uint8_t dest, uint8_t src, int16_t offset)
{
    return strb_cc_offset(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t strb_cc_offset_preindex(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x05e00000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x05600000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t strb_offset_preindex(uint8_t dest, uint8_t src, int16_t offset)
{
    return strb_cc_offset_preindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t strb_cc_offset_postindex(uint8_t cc, uint8_t dest, uint8_t src, int16_t offset)
{
    if (offset >= 0)
        return INSN_TO_LE(0x04c00000 | (cc << 28) | (dest << 16) | (src << 12) | offset);
    else
        return INSN_TO_LE(0x04400000 | (cc << 28) | (dest << 16) | (src << 12) | -offset);
}

static inline uint32_t strb_offset_postindex(uint8_t dest, uint8_t src, int16_t offset)
{
    return strb_cc_offset_postindex(ARM_CC_AL, dest, src, offset);
}

static inline uint32_t strb_cc_regoffset(uint8_t cc, uint8_t dest, uint8_t src, uint8_t offset, uint8_t shift)
{
    return INSN_TO_LE(0x07c00000 | (cc << 28) | (dest << 16) | (src << 12) | offset | ((shift & 0x1f) << 7));
}

static inline uint32_t strb_regoffset(uint8_t dest, uint8_t src, uint8_t offset, uint8_t shift)
{
    return strb_cc_regoffset(ARM_CC_AL, dest, src, offset, shift);
}

static inline uint32_t uxtab_cc(uint8_t cc, uint8_t dest, uint8_t second, uint8_t third)
{
    return INSN_TO_LE(0x06e00070 | (cc << 28) | (dest << 12) | (second << 16) | (third));
}

static inline uint32_t uxtab(uint8_t dest, uint8_t second, uint8_t third)
{
    return uxtab_cc(ARM_CC_AL, dest, second, third);
}

static inline uint32_t uxtah_cc(uint8_t cc, uint8_t dest, uint8_t second, uint8_t third)
{
    return INSN_TO_LE(0x06f00070 | (cc << 28) | (dest << 12) | (second << 16) | (third));
}

static inline uint32_t uxtah(uint8_t dest, uint8_t second, uint8_t third)
{
    return uxtah_cc(ARM_CC_AL, dest, second, third);
}

#endif /* _ARM_H */