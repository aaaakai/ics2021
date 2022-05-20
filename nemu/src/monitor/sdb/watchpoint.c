#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  word_t pre_value;
  char watch_expr[64];

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
	if (free_ == NULL) {
		printf("there is no free wp in wp_pool\n");
		return NULL;
	}
	WP *wp = free_;
	free_ = wp -> next;
	wp -> next = head;
	head = wp;
	return wp;
}

void add_wp(char *arg) {
	bool b_false = false;
	bool *expr_suc = &b_false;
	word_t expr_value = expr(arg, expr_suc);
	if (!*expr_suc) {
		printf("format of expr not right\n");
		return;
	}
	WP *watchp = new_wp();
	if (watchp == NULL) { return;}
	watchp -> pre_value = expr_value;
	strcpy(watchp -> watch_expr, arg);
	printf("watch point %s is set\n", arg);
	return;
}

void free_wp(WP *wp) {
	printf("watch point %s is delete\n", wp -> watch_expr);
	wp -> pre_value = 0;
	memset(wp -> watch_expr, 0, 64);
	WP *wp_inpool = head;
	if (wp_inpool == wp) {
		head = wp_inpool -> next;
		wp_inpool -> next = free_ -> next;
		free_ = wp_inpool;
		return;
	}
	while ((wp_inpool != NULL) && (wp_inpool -> next != wp)) {
		wp_inpool = wp_inpool -> next;
	}
	if (wp_inpool == NULL) { 
		printf("given wp dont exit in wp_pool\n");
		return;
	}
	wp_inpool -> next = wp -> next;
	wp -> next = free_;
	free_ = wp;
	return;
}

// delete wp by NO
void delete_wp(int N) {
	free_wp(&wp_pool[N]);
}

// show all the used wp
void show_wp() {
	WP *wp = head;
	if (wp == NULL) {
		printf("No watch points used.\n");
		return;
	}
	printf("Num\tWhat\n");
	while (wp != NULL) {
		printf("%d\t%s\n", wp -> NO, wp -> watch_expr);
		wp = wp -> next;
	}
	return;
} 

// scan the wp and examine whether changed
bool scan_wp() {
	bool changed = false;
	WP *wp = head;
	if (wp == NULL) {return changed;}
	while (wp != NULL) {
		bool b_false = false;
		bool *expr_suc = &b_false;
		word_t cur_value = expr(wp -> watch_expr, expr_suc);
		if (!*expr_suc) {
			printf("format of expr now is not right\n");
			return true;
		}
		if (wp -> pre_value != cur_value) {
			changed = true;
			printf("watch points %s changes from %u to %u\n",\
			  wp -> watch_expr, wp -> pre_value, cur_value);
			wp -> pre_value = cur_value;
		}
		wp = wp -> next;
	}
	return changed;
}

