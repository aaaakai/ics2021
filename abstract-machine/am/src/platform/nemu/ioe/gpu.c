#include <am.h>
#include <nemu.h>

#include "../../../riscv/riscv.h"

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  int i;
  int w = io_read(AM_GPU_CONFIG).width;;  // TODO: get the correct width
  int h = io_read(AM_GPU_CONFIG).height;  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	uint16_t screen_hight = inw(VGACTL_ADDR);
	uint16_t screen_width = inw(VGACTL_ADDR + 2);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = screen_width, .height = screen_hight,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
	int win_width = io_read(AM_GPU_CONFIG).width;
	
	uintptr_t fb = FB_ADDR;
	uint32_t *pi = (uint32_t *)(ctl->pixels);
	int block_width = ctl->w;
	int block_height = ctl->h;
	for (int y_index = 0; y_index < block_height; y_index ++) {
		for (int x_index = 0; x_index < block_width; x_index ++) {
			outl(fb + (win_width * ctl->y + win_width * y_index + ctl->x + x_index) * 4, pi[y_index * block_width + x_index]);
		}
	}
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
