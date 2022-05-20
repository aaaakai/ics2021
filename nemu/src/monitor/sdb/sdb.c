#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/vaddr.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
void add_wp(char *arg);
void show_wp();
void delete_wp(int N);
//word_t vaddr_read(vaddr_t, int);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
	if (arg == NULL) {
		/* no argument given */
		cpu_exec(1);
	}
	else {
		int intr_num = atoi(arg);
		cpu_exec(intr_num);
	}
	return 0;
}

static int cmd_info(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  char *char_r = "r";
  char *char_w = "w";
	if (arg == NULL) {
		/* no argument given */
		printf("use info with a argument like r or w");
	}
	else if (strcmp(arg, char_r) == 0) {
		isa_reg_display();
	}
	else if (strcmp(arg, char_w) == 0) {
		show_wp();
	}
	return 0;
}

static int cmd_x(char *args) {
  // extract the first argument
  char *arg1 = strtok(NULL, " ");
  if (arg1 == NULL) {
		// no args given 
		printf("use x with a arg N and a arg EXPR");
		return 0;
	}
	char *arg2 = strtok(NULL, " ");
  if (arg2 == NULL) {
		// no args given 
		printf("use x with a arg N and a arg EXPR");
		return 0;
	}
	bool b_false = false;
	bool *expr_suc = &b_false;
	paddr_t paddress = expr(arg2, expr_suc);
	if (!(*expr_suc)) {
		printf("EXPR's format is not right");
		return 0;
	}
	int len_paddr = atoi(arg1);
	if (arg1 <= 0) {
		printf("Len of addr should be positive");
		return 0;
	}
	for (int i = 0; i < len_paddr; i++) {
		word_t mem = vaddr_read(paddress + 4 * i, 4);
		printf("men0x%08x : 0x%08x\n", paddress + 4 * i, mem);
	}
	return 0;
} 

static int cmd_p(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
	if (arg == NULL) {
		/* no argument given */
		printf("use p with a expr");
	}
	bool b_false = false;
	bool *expr_suc = &b_false;
	word_t expr_value = expr(arg, expr_suc);
	if (*expr_suc) {
		printf("%u\n",expr_value);
		return 0;
	}
	else {
		printf("expr not defined well\n");
		return 0;
	}
}

static int cmd_w(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
	if (arg == NULL) {
		/* no argument given */
		printf("use w with a expr");
	}
	add_wp(arg);
	return 0;
}

static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");
	if (arg == NULL) {
		/* no argument given */
		printf("use w with a expr");
	}
	int N = atoi(arg);
	delete_wp(N);
	return 0;
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Exec num instructions of the program", cmd_si },
  { "info", "Print infomation of reg or watchpoint", cmd_info },
  { "x", "Print n word of memory from addr of value of EXPR", cmd_x },
  { "p", "Print value of the following expr", cmd_p },
  { "w", "Watch the given expr and stop if the value changes", cmd_w },
  { "d", "Delete the watch point of given NO", cmd_d },

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
