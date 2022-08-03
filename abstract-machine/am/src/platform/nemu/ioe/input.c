#include <am.h>
#include <nemu.h>

#include "../../../riscv/riscv.h"

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
	uint32_t am_scancode = inl(KBD_ADDR);
	if (am_scancode != AM_KEY_NONE) {
		kbd->keydown = am_scancode & KEYDOWN_MASK;
		kbd->keycode = am_scancode & (~KEYDOWN_MASK);
	}
	else {
		kbd->keydown = false;
		kbd->keycode = AM_KEY_NONE;
	}
}
