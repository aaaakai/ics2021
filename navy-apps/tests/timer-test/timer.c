#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <NDL.h>

int main() {
// simple test without NDL
/**
	char str[100];
	struct timeval *tv = (struct timeval *)str;
	printf("timeval %p\n", tv);
	gettimeofday(tv, NULL);
	int more_than_5 = 0;
	while (1) {
		gettimeofday(tv, NULL);
		if (more_than_5 == 0) {
			if ((int)(tv->tv_usec) >= 500000) {
				printf("0.5s passed\n");
				more_than_5 = 1;
			}
		}else {
			if ((int)(tv->tv_usec) <= 500000) {
				printf("0.5s passed\n");
				more_than_5 = 0;
			}
		}
	}
	return 0;
**/
	NDL_Init(0);
	
	uint32_t time;
	uint32_t msec = 500;
	while(1) {
		time = NDL_GetTicks();
		while(time < msec) {
			time = NDL_GetTicks();
		}
		
		printf("0.5s passed.\n");
		msec += 500;
	}
	
	NDL_Quit();
}	
		
