#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  vaddr_t pc;
  bool INTR;
} riscv32_CPU_state;

typedef struct {
	rtlreg_t mepc;
	rtlreg_t mtvec;
	union {
		struct{
      uint32_t UIE    : 1;
      uint32_t SIE    : 1;
      uint32_t WPRI_0 : 1;
      uint32_t MIE    : 1;
      uint32_t UPIE   : 1;
      uint32_t SPIE   : 1;
      uint32_t WPRI   : 1;
      uint32_t MPIE   : 1;
      uint32_t SPP    : 1;
      uint32_t WPRI_1_2 : 2;
      uint32_t MPP    : 2;
      uint32_t FS     : 2;
      uint32_t XS     : 2;
      uint32_t MPRV   : 1;
      uint32_t SUM    : 1;
      uint32_t MXR    : 1;
      uint32_t TVM    : 1;
      uint32_t TW     : 1;
      uint32_t TSR    : 1;
      uint32_t WPRI_3_10 : 8;
      uint32_t SD     : 1;
    } m;
    rtlreg_t value;
  } mstatus;
  rtlreg_t mcause;
  rtlreg_t satp;
  rtlreg_t mscratch;
} riscv32_CSR_state;
  

// decode
typedef struct {
  union {
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      int32_t  simm11_0  :12;
    } i;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm4_0    : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      int32_t  simm11_5  : 7;
    } s;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t imm31_12  :20;
    } u;
    struct {
    	uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t imm19_12  : 8;
      uint32_t imm11     : 1;
      uint32_t imm10_1   :10;
      uint32_t imm20     : 1;
    } uj;
    struct {
    	uint32_t opcode1_0 : 2;
    	uint32_t opcode6_2 : 5;
    	uint32_t rd        : 5;
    	uint32_t funct3    : 3;
    	uint32_t rs1       : 5;
    	uint32_t rs2       : 5;
    	uint32_t funct7    : 7;
    } r;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm11     : 1;
      uint32_t imm4_1    : 4;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t imm10_5   : 6;
      uint32_t imm12     : 1;
    } sb;
    uint32_t val;
  } instr;
} riscv32_ISADecodeInfo;

#define isa_mmu_check(vaddr, len, type) (csr.satp & (1ul << 31) ? MMU_TRANSLATE : MMU_DIRECT)

#endif
