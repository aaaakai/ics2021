#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  pf += nr_page * 4 * 1024;
  return pf - nr_page * 4 * 1024;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  assert((n % 4096) == 0);
  void *ret = new_page(n / 4096);
  memset(ret, 0, n);
  return ret;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

extern PCB *current;
/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  uintptr_t max_page_end = current->max_brk; 
  uintptr_t max_page_pn = max_page_end >> 12;
  uintptr_t brk_pn = brk >> 12;
  
  if (brk >= max_page_end){
    void *allocted_page =  new_page(brk_pn - max_page_pn + 1);
    for (int i = 0; i < brk_pn - max_page_pn + 1; ++i){
      map(&current->as, (void *)(((uintptr_t)(max_page_pn + i)) << 12),
       (void *)((uintptr_t)(allocted_page) + (i << 12)), 1);
    }

    current->max_brk = (brk_pn + 1) << 12;
    assert(current->max_brk > brk);
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
