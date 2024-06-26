#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

#define NR_REGS ARRLEN(regs)

void isa_reg_display() {
	for (int i = 0; i < NR_REGS; i ++) {
		printf("%s : %x\n", regs[i], cpu.gpr[i]._32);
	}
}

word_t isa_reg_str2val(const char *s, bool *success) {
	int i;
	for (i = 0; i < NR_REGS; i ++) {
		if (strcmp(regs[i], s) == 0) { break;}
	}
	if (strcmp(s, "pc") == 0) { *success = true; return cpu.pc;}
	if (i == NR_REGS) { 
		*success = false; 
		printf("name of reg not right");
		return 0;
	}
	*success = true;
  return cpu.gpr[i]._32;
}
