#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t keystate[sizeof(keyname) / sizeof(keyname[0])] = {0};

int SDL_PushEvent(SDL_Event *ev) {
  printf("please implement me\n");
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char key[64]={0};
  key[0] = '\0';
  int keycode = 0;
  NDL_PollEvent(key, sizeof(key));
  if (key[0] == '\0') {
  	ev->key.keysym.sym = SDLK_NONE;
  	ev->type = SDL_USEREVENT;
  	return 0;
  }
  //printf("%s\n",key);
  char* str = key+3;
  for(int i = 0,cnt = 0;i < sizeof(keyname);i+=sizeof(keyname[cnt]),cnt+=1)
  {
    if(!strncmp(keyname[cnt], str, strlen(str))&&
    		!strncmp(keyname[cnt], str, strlen(keyname[cnt]))) {
    	keycode = cnt;
    	break;
		}
    assert(i <= sizeof(keyname));
  }
  //printf("in waitevent\n");
  if(key[1] == 'u') {
    ev->key.keysym.sym = keycode;
    ev->type = SDL_KEYUP;
    keystate[keycode] = 0;
  }else if(key[1] == 'd'){
    ev->key.keysym.sym = keycode;
    ev->type = SDL_KEYDOWN;
    keystate[keycode] = 1;
  }else assert(0);
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  char key[64]={0};
  memset(key, 0, sizeof(key));
  key[0] = '0';
  int keycode = 0;
  while(!NDL_PollEvent(key, sizeof(key)));
  //printf("%s\n",key);
  char* str = key+3;
  for(int i = 0,cnt = 0;i < sizeof(keyname);i+=sizeof(keyname[cnt]),cnt+=1)
  {
    if(!strncmp(keyname[cnt], str, strlen(str))&&
    		!strncmp(keyname[cnt], str, strlen(keyname[cnt]))) {
    	keycode = cnt;
    	break;
		}
    assert(i <= sizeof(keyname));
  }
  //printf("in waitevent\n");
  if(key[1] == 'u') {
    event->key.keysym.sym = keycode;
    event->type = SDL_KEYUP;
    keystate[keycode] = 0;
  }else if(key[1] == 'd'){
    event->key.keysym.sym = keycode;
    event->type = SDL_KEYDOWN;
    keystate[keycode] = 1;
  }//else assert(0);
  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  printf("please implement me\n");
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  SDL_Event ev;
  
  if(numkeys) {
  	*numkeys = sizeof(keystate) / sizeof(keystate[0]);
  }
  return keystate;
}
