#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>
#include <string.h>

int main() {
  NDL_Init(0);
  int w, h;
  
  void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  printf("bmp loaded %p\n", bmp);
  assert(bmp);
  NDL_OpenCanvas(&w, &h);
  NDL_DrawRect(bmp, 0, 0, w, h);
  free(bmp);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
