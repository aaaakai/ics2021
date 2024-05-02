#include <string.h>
#include <stdio.h>
#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,
  
//#define simudelay 1 // comment this to cancle io-delay

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
	#ifdef simudelay
	yield();
	#endif
  char *str = (char *)buf;
  for (int i = 0; i < len; i ++) {
  	putch(str[i]);
  }
  return len;
}

void switch_program_index(int new_index);
size_t events_read(void *buf, size_t offset, size_t len) {
	#ifdef simudelay
	yield();
	#endif
	AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
	if (ev.keycode == AM_KEY_NONE) {
  	return 0;
  }
  
  switch (ev.keycode){
  case AM_KEY_F1:
    switch_program_index(1);
    return 0;

  case AM_KEY_F2:
    switch_program_index(2);
    return 0;

  case AM_KEY_F3:
    switch_program_index(3);
    return 0;
  
  default:
    break;
  }
  
  char *tag = ev.keydown ? "kd " : "ku ";
  strcpy(buf, tag);
  strcat(buf, keyname[ev.keycode]);
  return 1;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  if (offset > 0) {
  	return 0;
  }
  
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  
  int ret = snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d", w, h);
  if (ret >= len) {
  	panic("disinfo too long");
  	assert(0);
  }
  
  return ret + 1;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
	#ifdef simudelay
	yield();
	#endif
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  int x = (offset/4)%w;
  int y = (offset/4)/w;
  if(offset+len > w*h*4) len = w*h*4 - offset;
  io_write(AM_GPU_FBDRAW,x,y,(uint32_t*)buf,len/4,1,true);
  assert(offset <= w*h*4);
  
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
