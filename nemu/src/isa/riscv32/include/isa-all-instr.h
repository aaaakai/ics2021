#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(nemu_trap) f(addi) f(auipc) f(jal) f(jalr) \
											f(add_sub_mul) f(sltiu) f(beq) f(bne) f(sltu_mulhu) f(xor_div) f(or_rem) f(sh) f(srli_srai) \
											f(lbu) f(andi) f(sll_mulh) f(and_remu) f(xori) f(sb) f(bge) f(blt) f(bltu) f(bgeu) f(slt_mulhsu) \
											f(lb) f(lh) f(lhu) f(slli) f(slti) f(ori) f(srl_sra_divu)

def_all_EXEC_ID();
