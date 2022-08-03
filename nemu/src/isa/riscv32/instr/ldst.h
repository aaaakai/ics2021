// my lb
def_EHelper(lb) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
  rtl_sext(s, ddest, ddest, 8);
}

// my lh
def_EHelper(lh) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
  rtl_sext(s, ddest, ddest, 16);
}

def_EHelper(lw) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 4);
}

// my lbu
def_EHelper(lbu) {
	rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
}

// my lhu
def_EHelper(lhu) {
	rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(sw) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
}

// my store half-word
def_EHelper(sh) {
	rtl_sm(s, ddest, dsrc1, id_src2->simm, 2);
}

// my store byte
def_EHelper(sb) {
	rtl_sm(s, ddest, dsrc1, id_src2->simm, 1);
}
