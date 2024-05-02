#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
	csr.mepc = epc;
	csr.mcause = NO;
	csr.mstatus.m.MPIE = csr.mstatus.m.MIE;
	csr.mstatus.m.MIE = 0;

  return csr.mtvec;
}

#define IRQ_TIMER 0x80000007

word_t isa_query_intr() {
	if (csr.mstatus.m.MIE == 1 && cpu.INTR) {
		cpu.INTR = false;
		return IRQ_TIMER;
	}
  return INTR_EMPTY;
}
