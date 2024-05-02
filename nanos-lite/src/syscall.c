#include <common.h>
#include "syscall.h"
#include "fs.h"
#include <sys/time.h>
#include <stdint.h>
#include <proc.h>

//#define STRACE 1		// undef this to cancel strace

void naive_uload(PCB *pcb, const char *filename);
int execve(const char *filename, char *const argc[],
					char *const envp[]);

void sys_yield(Context *c) {
	yield();
	c->GPRx = 0;
}

void sys_exit(Context *c) {

	halt(c->GPR2);
	c->GPRx = 0;

	Log("exit");
	//c->GPRx = execve("/bin/menu", NULL, NULL);
}

void sys_write(Context *c) {
	/**
	uintptr_t a[4];
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;
	if (a[1] == 1 || a[1] == 2) {
		for (int i = 0; i < a[3]; i ++) {
			putch(*(((char *)a[2]) + i));
		}
		c->GPRx = a[3];
	}else {
		c->GPRx = -1;
	}
	**/
	
	int ret = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
	c->GPRx = ret;
}

void sys_open(Context *c) {
	int ret = fs_open((char *)c->GPR2, c->GPR3, c->GPR4);
	c->GPRx = ret;
} 

void sys_read(Context *c) {
	int ret = fs_read(c->GPR2, (void *)c->GPR3, c->GPR4);
	c->GPRx = ret;
} 

void sys_lseek(Context *c) {
	int ret = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
	c->GPRx = ret;
} 

void sys_close(Context *c) {
	int ret = fs_close(c->GPR2);
	c->GPRx = ret;
} 

int mm_brk(uintptr_t brk);
void sys_brk(Context *c) {
	uintptr_t addr = (uintptr_t)(c->GPR2);
	c->GPRx = mm_brk(addr);	// crrent simple implementation
}

void sys_execve(Context *c) {
	const char *filename = (const char*)c->GPR2;
	char **argv = (char **)c->GPR3;
	char **envp = (char **)c->GPR4;
	Log("exevce %s", filename);
	c->GPRx = execve(filename, argv, envp);
}

void sys_gettimeofday(Context *c) {
	struct timeval *tv = (struct timeval *)c->GPR2;
	uint64_t time = io_read(AM_TIMER_UPTIME).us;
	tv->tv_usec = (time % 1000000);
	tv->tv_sec = time / 1000000;
	#ifdef STRACE
  Log("System call grttime %d %d\n", tv->tv_usec, tv->tv_sec);
  #endif
	c->GPRx = 0;
}

static void strace(Context *c){
  #ifdef STRACE
    Log("System call trace\nmcause\t\tGPR1\t\tGPR2\t\tGPR3\t\tGPR4 \n0x%x\t\t%d\t\t0x%x\t\t0x%x\t\t0x%x",
      c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);
  #endif
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  
  strace(c);

  switch (a[0]) {
  	case SYS_yield: sys_yield(c); break;
  	case SYS_exit: sys_exit(c); break;
  	case SYS_write: sys_write(c); break;
  	case SYS_open: sys_open(c); break;
  	case SYS_read: sys_read(c); break;
  	case SYS_lseek: sys_lseek(c); break;
  	case SYS_close: sys_close(c); break;
  	case SYS_brk: sys_brk(c); break;
  	case SYS_execve: sys_execve(c); break;
  	case SYS_gettimeofday: sys_gettimeofday(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
