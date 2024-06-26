#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

uint32_t NDL_GetTicks() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	
  return tv.tv_usec / 1000 + tv.tv_sec * 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0);
  
  return read(fd, buf, len);
}

static int canvas_w, canvas_h, canvas_x, canvas_y = 0;

void NDL_OpenCanvas(int *w, int *h) {
	if (*w == 0) {
		*w = screen_w;
	}else {
		assert(*w <= screen_w);
	}
	if (*h == 0) {
		*h = screen_h;
	}else {
		assert(*h <= screen_h);
	}
	canvas_w = *w;
	canvas_h = *h;
	canvas_x = (screen_w - canvas_w) / 2;
	canvas_y = (screen_h - canvas_h) / 2;
	
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	int graphics = open("/dev/fb", 0);
	
	for (int i = 0; i < h; i ++) {
		lseek(graphics, 
			((canvas_y + y + i) * screen_w + canvas_x + x) * sizeof(uint32_t),
			SEEK_SET);
		write(graphics, pixels + w * i, w * sizeof(uint32_t));
	}	
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

static void read_key_value(char *str, char *key, int* value){
  char buffer[128];
  int len = 0;
  for (char* c = str; *c; ++c){
    if(*c != ' '){
      buffer[len++] = *c;
    }
  }
  buffer[len] = '\0';

  sscanf(buffer, "%[a-zA-Z]:%d", key, value);
  // printf("read_key_value\n");
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  
  char info[128], key[64];
  int value;
  
  int dispinfo = open("/proc/dispinfo", 0);
  //printf("dispinfo %d\n", dispinfo);
  read(dispinfo, info, sizeof(info));
  //printf("dispinfo read done\n");
  //printf("%s\n", info);
  close(dispinfo);
  
  char *token = strtok(info, "\n");
  
  while (token != NULL) {
  	read_key_value(token, key, &value);
  	//printf("key %s value %d \n", key, value);
  	
  	if(strcmp(key, "WIDTH") == 0) {
  		screen_w = value;
  	}else if(strcmp(key, "HEIGHT") == 0){
  		screen_h = value;
  	}
  	
  	token = strtok(NULL, "\n");
  } 
  return 0;
}

void NDL_Quit() {
}
