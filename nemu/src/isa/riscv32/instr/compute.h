def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}


// my addi
def_EHelper(addi) {
	rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

// my andi
def_EHelper(andi) {
	rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

// my slli
def_EHelper(slli) {
	rtl_slli(s, ddest, dsrc1, id_src2->imm);
}

// my slti
def_EHelper(slti) {
	rtl_setrelopi(s, RELOP_LT, ddest, dsrc1, id_src2->imm);
}

// my ori
def_EHelper(ori) {
	rtl_ori(s, ddest, dsrc1, id_src2->imm);
}

// my xori
def_EHelper(xori) {
	rtl_xori(s, ddest, dsrc1, id_src2->imm);
}

// my srli_srai
def_EHelper(srli_srai) {
	switch(s->isa.instr.r.funct7) {
		case 0b0000000: rtl_srli(s, ddest, dsrc1, s->isa.instr.r.rs2); break;
		case 0b0100000: rtl_srai(s, ddest, dsrc1, s->isa.instr.r.rs2); break;
		default: printf("funct7%x is not complete", s->isa.instr.r.funct7); break;
	}
}

// my sll_mulh
def_EHelper(sll_mulh) {
	switch(s->isa.instr.r.funct7) {
		case 0b0000000: rtl_sll(s, ddest, dsrc1, dsrc2);	break;	// sll
		case 0b0000001: rtl_muls_hi(s, ddest, dsrc1, dsrc2); break;	// mulh
		default: printf("funct7%x is not complete", s->isa.instr.r.funct7); break;
	}
}

// my slt_mulhsu
def_EHelper(slt_mulhsu) {
	switch(s->isa.instr.r.funct7) {
		case 0b0000000: rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);	break;	// slt
		case 0b0000001: rtl_mulsu_hi(s, ddest, dsrc1, dsrc2); break;	// mulsu
		default: printf("funct7%x is not complete", s->isa.instr.r.funct7); break;
	}
}

// my sltu_mulhu
def_EHelper(sltu_mulhu) {
	switch(s->isa.instr.r.funct7) {
		case 0b0000000: rtl_setrelop(s, RELOP_LTU, ddest, dsrc1, dsrc2);	break;	// sll
		case 0b0000001: rtl_mulu_hi(s, ddest, dsrc1, dsrc2); break;	// mulhu
		default: printf("funct7%x is not complete", s->isa.instr.r.funct7); break;
	}
}

// my xor_div
def_EHelper(xor_div) {
	switch(s->isa.instr.r.funct7) {
		case 0b0000000: rtl_xor(s, ddest, dsrc1, dsrc2);	break;	// xor
		case 0b0000001: rtl_divs_q(s, ddest, dsrc1, dsrc2); break; // div
		default: printf("funct7%x is not complete", s->isa.instr.r.funct7); break;
	}
}

// my or
def_EHelper(or_rem) {
	switch(s->isa.instr.r.funct7) {
		case 0b0000000: rtl_or(s, ddest, dsrc1, dsrc2);	break;	// or
		case 0b0000001: rtl_divs_r(s, ddest, dsrc1, dsrc2); break; // rem
		default: printf("funct7%x is not complete", s->isa.instr.r.funct7); break;
	}
}

// my and_remu
def_EHelper(and_remu) {
	switch(s->isa.instr.r.funct7) {
		case 0b0000000: rtl_and(s, ddest, dsrc1, dsrc2);	break;	// and
		case 0b0000001: rtl_divu_r(s, ddest, dsrc1, dsrc2); break; // remu
		default: printf("funct7%x is not complete", s->isa.instr.r.funct7); break;
	}
}

// my srl_sra_divu
def_EHelper(srl_sra_divu) {
	switch(s->isa.instr.r.funct7) {
		case 0b0000000: rtl_srl(s, ddest, dsrc1, dsrc2);	break;	// srl
		case 0b0100000: rtl_sra(s, ddest, dsrc1, dsrc2); break; // sra
		case 0b0000001: rtl_divu_q(s, ddest, dsrc1, dsrc2); break; // divu
		default: printf("funct7%x is not complete", s->isa.instr.r.funct7); break;
	}
}

// my auipc
def_EHelper(auipc) {
	rtl_li(s, s0, s->pc);
	rtl_addi(s, ddest, s0, id_src1->imm);
}

// my jal
def_EHelper(jal) {
	rtl_li(s, s0, s->pc);
	rtl_addi(s, ddest, s0, 4);
	rtl_addi(s, s0, s0, id_src1->imm);
	rtl_jr(s, s0);
}

// my jalr
def_EHelper(jalr) {
	rtl_li(s, s0, s->pc);
	rtl_addi(s, s0, s0, 4);
	rtl_li(s, s1, id_src2->imm);
	rtl_sext(s, s1, s1, 12);
	rtl_add(s, s1, s1, dsrc1);
	rtl_andi(s, s1, s1, ~1);
	rtl_jr(s, s1);
	rtl_mv(s, ddest, s0);
}

// my add_sub_mul
def_EHelper(add_sub_mul) {
	switch(s->isa.instr.r.funct7) {
		case 0b0000000: rtl_add(s, ddest, dsrc1, dsrc2); break; // add
		case 0b0100000: rtl_sub(s, ddest, dsrc1, dsrc2); break; // sub
		case 0b0000001: rtl_mulu_lo(s, ddest, dsrc1, dsrc2); break; // mul
		default: printf("funct7%x is not complete", s->isa.instr.r.funct7); break;
	}
}

// my sltiu
def_EHelper(sltiu) {
	rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
}

// my beq
def_EHelper(beq) {
	rtl_li(s, s0, s->pc);
	rtl_addi(s, s0, s0, id_dest->imm);
	rtl_jrelop(s, RELOP_EQ, dsrc1, dsrc2, *s0);
}

// my bne
def_EHelper(bne) {
	rtl_li(s, s0, s->pc);
	rtl_addi(s, s0, s0, id_dest->imm);
	rtl_jrelop(s, RELOP_NE, dsrc1, dsrc2, *s0);
}

// my blt
def_EHelper(blt) {
	rtl_li(s, s0, s->pc);
	rtl_addi(s, s0, s0, id_dest->imm);
	rtl_jrelop(s, RELOP_LT, dsrc1, dsrc2, *s0);
}

// my bge
def_EHelper(bge) {
	rtl_li(s, s0, s->pc);
	rtl_addi(s, s0, s0, id_dest->imm);
	rtl_jrelop(s, RELOP_GE, dsrc1, dsrc2, *s0);
}

// my bltu
def_EHelper(bltu) {
	rtl_li(s, s0, s->pc);
	rtl_addi(s, s0, s0, id_dest->imm);
	rtl_jrelop(s, RELOP_LTU, dsrc1, dsrc2, *s0);
}

// my bgeu
def_EHelper(bgeu) {
	rtl_li(s, s0, s->pc);
	rtl_addi(s, s0, s0, id_dest->imm);
	rtl_jrelop(s, RELOP_GEU, dsrc1, dsrc2, *s0);
}


