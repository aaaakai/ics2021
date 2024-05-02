#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

static int program_index = 1;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void switch_program_index(int new_index){
  if (new_index == program_index)
    return ;

  switch_boot_pcb();  
  
  program_index = new_index;
  //pcb[0].cp->pdir = NULL;
  printf("Switch to PCB[%d]\n", new_index);

  yield();
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    //Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", 
    //		(char *)arg, j);
    j ++;
    yield();
  }
}

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
void context_uload(PCB *pcb, const char *filename, 
									char *const argv[], char *const envp[]);

int execve(const char *filename, char *const argv[],
					char *const envp[]){
	context_uload(current, filename, argv, envp);
	switch_boot_pcb();
	
	Log("Switching processing%s...", filename);
	yield();
	return -1;
} 

#define PROG_PATH1 "/bin/pal"
#define PROG_PATH2 "/bin/bird"
#define PROG_PATH3 "/bin/nslider"
#define PROG_PATH4 "/bin/hello"
void init_proc() {
	context_kload(&pcb[0], hello_fun, "first");
	char *argv1[] = {PROG_PATH1, "--skip", NULL};
	context_uload(&pcb[1], PROG_PATH1, argv1, NULL);
  /**
	char *argv2[] = {PROG_PATH2, NULL};
	context_uload(&pcb[3], PROG_PATH2, argv2, NULL);
	char *argv3[] = {PROG_PATH3, NULL};
	context_uload(&pcb[2], PROG_PATH3, argv3, NULL);
	char *argv4[] = {PROG_PATH4, NULL};
	context_uload(&pcb[0], PROG_PATH4, argv4, NULL);
	**/
  switch_boot_pcb();

  Log("Initializing processes...");
  
  //naive_uload(NULL, "/bin/pal");

  // load program here

}

static int timer_split = 0;
static int total_time = 16;
Context* schedule(Context *prev) {
  current->cp = prev;
  current = (current != &pcb[0] && timer_split == 0) ? 
  	&pcb[0] : &pcb[program_index];
  timer_split = (timer_split + 1) % total_time;
  //current = &pcb[1];
  
  return current->cp;
}
