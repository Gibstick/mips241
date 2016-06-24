#include <assert.h>
#include <stdio.h>
#include "machine/impl.h"

// Macros
#define ASSIGN_RTYPE_TABLE(ID) \
    do { \
        RTYPE_TABLE[FUNC_##ID] = ID##_impl; \
    } while(0)

#define ASSIGN_ITYPE_TABLE(ID) \
    do { \
        ITYPE_TABLE[OP_##ID] = immediate_impl; \
    } while(0)

#define R_REG(REGISTER) m->registers[ins->decoded.r.REGISTER]
#define I_REG(REGISTER) m->registers[ins->decoded.i.REGISTER]
#define M_REG(REGISTER) m->registers[REGISTER]

// Constants
static const uint32_t MAPPED_INPUT_ADDR = 0xFFFF000C; 
static const uint32_t MAPPED_OUTPUT_ADDR = 0xFFFF0004;


enum instruction_retcode (*RTYPE_TABLE[RTYPE_TABLE_SIZE]) (Machine * const, const Instruction * const);
enum instruction_retcode (*ITYPE_TABLE[ITYPE_TABLE_SIZE]) (Machine * const, const Instruction * const);


// Invalid opcodes point to this function
static enum instruction_retcode
invalid_instruction(Machine * const m, const Instruction * const i) {
    return IR_INVALID_INSTRUCTION; 
}

static enum instruction_retcode ADD_impl(Machine * const m, const Instruction * const ins) {
    R_REG(d) = R_REG(s) + R_REG(t);
    return IR_SUCCESS;
}

static enum instruction_retcode SUB_impl(Machine * const m, const Instruction * const ins) {
    R_REG(d) = R_REG(s) - R_REG(t);
    return IR_SUCCESS;
}

static enum instruction_retcode MULTU_impl(Machine * const m, const Instruction * const ins)
{
    // need to sign extend
    const int32_t lhs = R_REG(s) > 0x7FFFFFF ? ~R_REG(s) + 1 : R_REG(s);
    const int32_t rhs = R_REG(t) > 0x7FFFFFF ? ~R_REG(t) + 1 : R_REG(t);
    const uint64_t result = lhs * rhs;
    m->hi = result >> 32;
    m->lo = result & 0xFFFFFFFF;
    return IR_SUCCESS;
}

static enum instruction_retcode MULT_impl(Machine * const m, const Instruction * const ins)
{
    const uint64_t result = R_REG(s) * R_REG(t);
    m->hi = result >> 32;
    m->lo = result & 0xFFFFFFFF;
    return IR_SUCCESS;
}

static enum instruction_retcode DIV_impl(Machine * const m, const Instruction * const ins)
{
    m->lo = R_REG(s) / R_REG(t);
    m->hi = R_REG(s) % R_REG(t);
    return IR_SUCCESS;
}

static enum instruction_retcode DIVU_impl(Machine * const m, const Instruction * const ins)
{
    const int32_t lhs = R_REG(s) > 0x7FFFFFF ? ~R_REG(s) + 1 : R_REG(s);
    const int32_t rhs = R_REG(t) > 0x7FFFFFF ? ~R_REG(t) + 1 : R_REG(t);
    m->lo = lhs / rhs;
    m->hi = lhs % rhs;
    return IR_SUCCESS;
}

static enum instruction_retcode MFLO_impl(Machine * const m, const Instruction * const ins)
{
    R_REG(d) = m->lo;  
    return IR_SUCCESS;
}

static enum instruction_retcode MFHI_impl(Machine * const m, const Instruction * const ins)
{
    R_REG(d) = m->hi;
    return IR_SUCCESS;
}

static enum instruction_retcode LIS_impl(Machine * const m, const Instruction * const ins)
{
    R_REG(d) = m->mem[m->pc / 4];
    m->pc += 4;
    return IR_SUCCESS;
}

static enum instruction_retcode SLT_impl(Machine * const m, const Instruction * const ins)
{
    const int32_t lhs = R_REG(s) > 0x7FFFFFF ? ~R_REG(s) + 1 : R_REG(s);
    const int32_t rhs = R_REG(t) > 0x7FFFFFF ? ~R_REG(t) + 1 : R_REG(t);
    R_REG(d) = lhs < rhs;
    return IR_SUCCESS;
}

static enum instruction_retcode SLTU_impl(Machine * const m, const Instruction * const ins)
{
    R_REG(d) = R_REG(s) < R_REG(t);
    return IR_SUCCESS;
}

static enum instruction_retcode JR_impl(Machine * const m, const Instruction * const ins)
{
    m->pc = R_REG(s);
    return IR_SUCCESS;
}

static enum instruction_retcode JALR_impl(Machine * const m, const Instruction * const ins)
{
    const uint32_t temp = R_REG(s);
    M_REG(31) = m->pc;
    m->pc = temp;
    return IR_SUCCESS;
}


static enum instruction_retcode
immediate_impl(Machine * const m, const Instruction * const ins)
{
    const int16_t immediate = ins->decoded.i.imm;
    const int64_t byte_addr = (I_REG(s) + immediate);
    const int64_t word_addr = byte_addr / 4;

    // check for bad memory access and return accordingly
    if (ins->code == OP_LW || ins->code == OP_SW) {
        if (word_addr < 0 || word_addr >= m->mem_size) {
            return IR_OUT_OF_RANGE_MEMORY_ACCESS;
        }
        if (byte_addr % 4 == 0) {
            return IR_UNALIGNED_MEMORY_ACCESS;
        }
    }

    switch (ins->code) {
        case OP_LW:
            if (byte_addr == MAPPED_INPUT_ADDR) {
                // TODO: does the reference emulator do this?
                int c = getchar();
                if (c != EOF)
                    m->registers[ins->decoded.i.t] = c;
            } else
                I_REG(t) = m->mem[word_addr];
            break;
        case OP_SW:
            if (byte_addr == MAPPED_OUTPUT_ADDR)
                printf("%c", (char) (I_REG(t) & 0xFF));
            else
                m->mem[word_addr] = I_REG(t);

            break;
        case OP_BEQ:
            m->pc += (immediate * 4 * I_REG(s) == I_REG(t));
            break;
        case OP_BNE:
            m->pc += (immediate * 4 * I_REG(s) != I_REG(t));
    }

    return IR_SUCCESS;
}

void init_tables(void) {
    assert(RTYPE_TABLE_SIZE == ITYPE_TABLE_SIZE);

    for (int i = 0; i < RTYPE_TABLE_SIZE; ++i) {
        RTYPE_TABLE[i] = invalid_instruction;
        ITYPE_TABLE[i] = invalid_instruction;
    }

    ASSIGN_RTYPE_TABLE(ADD);
    ASSIGN_RTYPE_TABLE(SUB);
    ASSIGN_RTYPE_TABLE(MULT);
    ASSIGN_RTYPE_TABLE(MULTU);
    ASSIGN_RTYPE_TABLE(DIV);
    ASSIGN_RTYPE_TABLE(DIVU);
    ASSIGN_RTYPE_TABLE(MFHI);
    ASSIGN_RTYPE_TABLE(MFLO);
    ASSIGN_RTYPE_TABLE(LIS);
    ASSIGN_RTYPE_TABLE(SLT);
    ASSIGN_RTYPE_TABLE(SLTU);
    ASSIGN_RTYPE_TABLE(JR);
    ASSIGN_RTYPE_TABLE(JALR);

    ASSIGN_ITYPE_TABLE(LW);
    ASSIGN_ITYPE_TABLE(SW);
    ASSIGN_ITYPE_TABLE(BEQ);
    ASSIGN_ITYPE_TABLE(BNE);

#ifndef NDEBUG
    {
        int rcount = 0;
        for (int i = 0; i < RTYPE_TABLE_SIZE; ++i)
            rcount += RTYPE_TABLE[i] != invalid_instruction;
        assert(rcount == 13);

        int icount = 0;
        for (int i = 0; i < ITYPE_TABLE_SIZE; ++i)
            icount += ITYPE_TABLE[i] != invalid_instruction;
        assert(icount == 4);
    }
#endif
}
