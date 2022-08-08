def_EHelper(inv) {
  rtl_hostcall(s, HOSTCALL_INV, NULL, NULL, NULL, 0);
}

def_EHelper(nemu_trap) {
  rtl_hostcall(s, HOSTCALL_EXIT, NULL, &gpr(10), NULL, 0); // gpr(10) is $a0
}

// system call

 #define Machine_Software_Interrupt 11

rtlreg_t* decode_csr_no(int csr_no){
	switch (csr_no) {
		case 0x300:
			return &(csr.mstatus.value);
		case 0x305:
			return &(csr.mtvec);
		case 0x341:
			return &(csr.mepc);
		case 0x342:
			return &(csr.mcause);
		default :{
			printf("csr_no%d is wrong", csr_no);
			assert(0);
		}
	}
}

def_EHelper(ecall_ebreak) {
	switch (id_src2->imm) {
		case 0b000000000000 :{	// ecall
			rtlreg_t dnpc = 0;
			if (csr.mstatus.m.MPP == 3) {
				dnpc = isa_raise_intr(Machine_Software_Interrupt, s->pc);
			}
			else {
				printf("mstatus.MPP is not machine state\n");
				assert(0);
			}
			rtl_j(s, dnpc);
			break;
		}
		case 0b001100000010 :{	// mret
			rtl_j(s, csr.mepc);
			//csr.mstatus.m.MIE = csr.mstatus.m.MPIE;
			//csr.mstatus.m.MPIE = 1;
			break;
		}
		default :{
			printf("fmt imm %x is not complete\n", id_src2->imm);
			assert(0);
		}
	}
}

def_EHelper(csrrw) {
	rtl_mv(s, s0, decode_csr_no(id_src2->imm));
	rtl_mv(s, decode_csr_no(id_src2->imm), dsrc1);
	rtl_mv(s, ddest, s0);
}

def_EHelper(csrrs) {
	rtl_mv(s, s0, decode_csr_no(id_src2->imm));
	rtl_or(s, decode_csr_no(id_src2->imm), s0, dsrc1);
	rtl_mv(s, ddest, s0);
} 
	
