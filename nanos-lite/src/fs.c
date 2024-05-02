#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

//#define sfstrace 1		// comment this to cancel fs trace

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("cant read should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("cant write should not reach here");
  return 0;
}

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB]     = {"/dev/fb", 0, 0, invalid_read, fb_write},
  {"/dev/events", 0, 0, events_read, invalid_write},
  {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

// return the fd, if failure return -1
int fs_open(const char *pathname, int flags, int mode) {
	int file_num = sizeof(file_table) / sizeof(Finfo);
	if (pathname == NULL) {
		Log("try to open NULL file");
		return -1;
	}
	for (int i = 0; i < file_num; i ++) {
		if (strcmp(pathname, file_table[i].name) == 0) {
			file_table[i].open_offset = 0;
			#ifdef sfstrace
			printf("find fd %d\n",i);
			#endif
			return i;
		}
	}
	panic("dant find the fd");
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
	int file_num = sizeof(file_table) / sizeof(Finfo);
	if (fd >= file_num) {
		panic("cant find the fd");
		return 0;
	}
	/** switch (fd) {
		case FD_STDIN:
		case FD_STDOUT:
		case FD_STDERR: {
			printf("cant read fd %d\n", fd);
			return 0;
		} 
		default: { **/
			#ifdef sfstrace
			printf("want read fd%d %dbytes\n", fd, len);
			#endif
			Finfo *info = &file_table[fd];
			size_t real_len;
			
			if(info->read) {
				real_len = info->read(buf, info->open_offset, len);
				info->open_offset += real_len;
			}else {
				real_len = info->open_offset + len <= info->size ?
									len : info->size - info->open_offset;
				ramdisk_read(buf, info->disk_offset + info->open_offset,
										real_len);
				info->open_offset += real_len;
			}
			
			#ifdef sfstrace
			printf("read fd%d %dbytes\n", fd, real_len);
			#endif
			return real_len;
		/**}
	}**/
}
size_t fs_write(int fd, const void *buf, size_t len) {
	int file_num = sizeof(file_table) / sizeof(Finfo);
	if (fd >= file_num) {
		panic("cant find the fd");
		return 0;
	}
	
	Finfo *info = &file_table[fd];
	size_t real_len = 0;
	
	if(info->write) {
		real_len = info->write(buf, info->open_offset, len);
		info->open_offset += real_len;
	}else {
		if (info->open_offset + len >= info->size) {
			printf("file space is not enough");	// this may cause error
		}else {
			ramdisk_write(buf, info->disk_offset + info->open_offset,
										len);
			info->open_offset += len;
			real_len = len;
		}
	}
	return real_len;
}
			
size_t fs_lseek(int fd, size_t offset, int whence) {
	int file_num = sizeof(file_table) / sizeof(Finfo);
	if (fd >= file_num) {
		panic("cant find the fd");
		return 0;
	}
	
	Finfo *info = &file_table[fd];
	
	switch(whence) {
		case SEEK_SET: {
			assert(info->size >= offset);
			info->open_offset = offset;
			break;
		}
		case SEEK_CUR: {
			assert((info->open_offset + offset) <= info->size);
			info->open_offset += offset;
			break;
		}
		case SEEK_END: {
			assert((offset + info->size) >= 0 &&
						offset <= 0);
			info->open_offset = offset + info->size;
			break;
		}
		default: {
			panic("whence is not right");
			return -1;
		}
	}
	#ifdef sfstrace
	printf("lseek fd%d to %d\n", fd, info->open_offset);
	#endif
	return info->open_offset;
}
			
	

int fs_close(int fd) {
	file_table[fd].open_offset = 0;
	return 0;
}

void init_fs() {
	AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
	file_table[FD_FB].size = 
		config.width * config.height * sizeof(uint32_t);
	#ifdef sfstrace
	printf("fd_fb size %d\n", file_table[FD_FB].size);
	#endif
  // TODO: initialize the size of /dev/fb
}
