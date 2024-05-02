#include <proc.h>
#include <elf.h>
#include <fs.h>
#include <memory.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

//size_t ramdisk_read(void *buf, size_t offset, size_t len);

static void read(int fd, void *buf, size_t offset, size_t len) {
	fs_lseek(fd, offset, SEEK_SET);
	fs_read(fd, buf, len);
}

#define NR_PAGE 8
#define PAGESIZE 4096

static void *alloc_section_space(AddrSpace *as, 
														uintptr_t vaddr, size_t p_memsz){
  size_t page_n = ((vaddr + p_memsz - 1) >> 12) - (vaddr >> 12) + 1;
  void *page_start = new_page(page_n);

  printf("Loaded Segment from [%x to %x)\n", vaddr, vaddr + p_memsz);
  
  for (int i = 0; i < page_n; ++i){
    map(as, (void *)((vaddr & ~0xfff) + i * PAGESIZE), 
    	(void *)(page_start + i * PAGESIZE), 1);
  }

  return page_start;
}

static uintptr_t loader(PCB *pcb, const char *filename) {
// naive implementation without fs
/**
  Elf_Ehdr elf_header;
  ramdisk_read(&elf_header, 0, sizeof(elf_header));
  int pheaders_num = elf_header.e_phnum;
  int phdr_size = elf_header.e_phentsize;
  printf("%d\n",phdr_size);
  int phdr_offset = elf_header.e_phoff;
  for (int i = 0; i < pheaders_num; i ++) {
  	Elf_Phdr program_header;
  	ramdisk_read(&program_header, phdr_offset + i * phdr_size,
  		phdr_size);
  	printf("%d\n",phdr_offset + i * phdr_size);
  	
  	if (program_header.p_type == PT_LOAD) {
  		printf("a phdr\n");
  		ramdisk_read((void *)(program_header.p_vaddr), program_header.p_offset,
  			program_header.p_memsz);
  		printf("%x %x %x\n",program_header.p_vaddr,program_header.p_offset,
  			program_header.p_filesz);
  		memset((void *)(program_header.p_vaddr + program_header.p_filesz), 0,
  			program_header.p_memsz - program_header.p_filesz);
  	}
  }
  return elf_header.e_entry;
**/

	int fd = fs_open(filename, 0, 0);
	if (fd == -1){
		if(filename == NULL) {
			panic("no filename is given");
			assert(0);
		}else {
		panic("cant find file %s", filename);
		assert(0);
		}
	}
	printf("load fd%d\n", fd);
	
	AddrSpace *as = &pcb->as;
  
  Elf_Ehdr elf_header;
  read(fd, &elf_header, 0, sizeof(elf_header));
  
  int pheaders_num = elf_header.e_phnum;
  int phdr_size = elf_header.e_phentsize;
  printf("%d\n",phdr_size);
  int phdr_offset = elf_header.e_phoff;
  
  for (int i = 0; i < pheaders_num; i ++) {
  	Elf_Phdr program_header;
  	read(fd, &program_header, phdr_offset + i * phdr_size,
  		phdr_size);
  	printf("%d\n",phdr_offset + i * phdr_size);
  	void *phys_addr;
  	uintptr_t virt_addr;
  	
  	if (program_header.p_type == PT_LOAD) {
  		printf("a phdr\n");
  		virt_addr = program_header.p_vaddr;
  		phys_addr = alloc_section_space(as, virt_addr,
  			program_header.p_memsz);
  		read(fd, (void *)(phys_addr + (virt_addr & 0xfff)), 
  			program_header.p_offset, program_header.p_memsz);
  		printf("%x %x %x\n",program_header.p_vaddr,program_header.p_offset,
  			program_header.p_filesz);
  		memset((void *)(phys_addr + (virt_addr & 0xfff) + 
  			program_header.p_filesz), 0,
  			program_header.p_memsz - program_header.p_filesz);
  			
  		if (program_header.p_filesz < program_header.p_memsz){
  			printf("Setting .bss end %x\n", 
  				program_header.p_vaddr + program_header.p_memsz);
  			pcb->max_brk = ROUNDUP(program_header.p_vaddr +
  				program_header.p_memsz, 0xfff);
  		}
  	}
  }
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
	Area karea;
	karea.start = &pcb->cp;
	karea.end   = &pcb->cp + STACK_SIZE;
	
	pcb->cp = kcontext(karea, entry, arg);
	printf("kcontext address is %p\n", pcb->cp);
}

void context_uload(PCB *pcb, const char *filename, 
									char *const argv[], char *const envp[]) {
	int envc = 0, argc = 0;
	AddrSpace *as = &pcb->as;
	protect(as);
	
	if (envp){
		for (; envp[envc]; envc ++) {}
	}
	if (argv){
		for (; argv[argc]; argc ++) {}
	}
	char *envp_ustack[envc];
	void *alloced_page = new_page(NR_PAGE) + NR_PAGE * PAGESIZE;
	
	map(as, as->area.end - 8 * PAGESIZE, alloced_page - 8 * PAGESIZE, 1); 
  map(as, as->area.end - 7 * PAGESIZE, alloced_page - 7 * PAGESIZE, 1);
  map(as, as->area.end - 6 * PAGESIZE, alloced_page - 6 * PAGESIZE, 1); 
  map(as, as->area.end - 5 * PAGESIZE, alloced_page - 5 * PAGESIZE, 1);
  map(as, as->area.end - 4 * PAGESIZE, alloced_page - 4 * PAGESIZE, 1); 
  map(as, as->area.end - 3 * PAGESIZE, alloced_page - 3 * PAGESIZE, 1);
  map(as, as->area.end - 2 * PAGESIZE, alloced_page - 2 * PAGESIZE, 1); 
  map(as, as->area.end - 1 * PAGESIZE, alloced_page - 1 * PAGESIZE, 1); 
	
	char *brk = (char *)(alloced_page - 4);
	for (int i = 0; i < envc; i++) {
		brk -= strlen(envp[i]) + 1;
		envp_ustack[i] = brk;
		strcpy(brk, envp[i]);
	}
	
	char *argv_ustack[argc];
	for (int i = 0; i < argc; i ++) {
		brk -= strlen(argv[i]) + 1;
		argv_ustack[i] = brk;
		strcpy(brk, argv[i]);
	}
	
	intptr_t *ptr_brk = (intptr_t *)brk;
	
	ptr_brk -= 1;
	*ptr_brk = 0;
	ptr_brk -= envc;
	for (int i = 0; i < envc; i ++) {
		ptr_brk[i] = (intptr_t)(envp_ustack[i]);
	}
	
	ptr_brk -= 1;
	*ptr_brk = 0;
	ptr_brk -= argc;
	for (int i = 0; i < argc; i ++) {
		ptr_brk[i] = (intptr_t)(argv_ustack[i]);
	}
	
	ptr_brk -= 1;
	*ptr_brk = argc;
	
	uintptr_t entry = loader(pcb, filename);
	printf("uload entry %p\n", (void *)entry);
	Area stack = {&pcb->cp, &pcb->cp + STACK_SIZE};
	pcb->cp    = ucontext(as, stack, (void *)entry);
	printf("ucontext address is %p\n", pcb->cp);
	
	ptr_brk -= 1;
	*ptr_brk = 0;
	
	pcb->cp->gpr[2] = (uintptr_t)ptr_brk - (uintptr_t)alloced_page +
		(uintptr_t)as->area.end;
	pcb->cp->GPRx = (uintptr_t)ptr_brk - (uintptr_t)alloced_page +
		(uintptr_t)as->area.end + 4;
}
