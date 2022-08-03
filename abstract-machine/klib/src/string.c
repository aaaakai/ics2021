#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  
  while (s[len] != '\0') {
  	len = len + 1;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  size_t len = strlen(src);
  
  for (size_t i = 0; i <= len; i ++) {
  	dst[i] = src[i];
  }
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
  
  for (i = 0; i < n && src[i] != '\0'; i ++) {
  	dst[i] = src[i];
  }
  for ( ; i < n; i ++) {
  	dst[i] = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t dst_len = strlen(dst);
  size_t src_len = strlen(src);
  
	for (size_t i = 0; i <= src_len; i ++) {
		dst[dst_len + i] = src[i];
	}    
	return dst;
}

int strcmp(const char *s1, const char *s2) {
  size_t len = strlen(s1);
  size_t idx = 0;
  int diff = 0;
  
  while (diff == 0 && idx <= len) {
  	diff = s1[idx] - s2[idx];
  	idx ++;
  }
  return diff;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t len = strlen(s1);
  size_t idx = 0;
  int diff = 0;
  
  while (diff == 0 && idx <= len && idx < n) {
  	diff = s1[idx] - s2[idx];
  	idx ++;
  }
  return diff;
}

void *memset(void *s, int c, size_t n) {
  for (size_t i = 0; i < n; i ++) {
  	((char *) s)[i] = (char) c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char temp[n + 1];
  for (size_t i = 0; i < n; i ++) {
  	temp[i] = ((char *) src)[i];
  }
  for (size_t i = 0; i < n; i ++) {
  	((char *) dst)[i] = temp[i];
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  for (size_t i = 0; i < n; i ++) {
  	((char *) out)[i] = ((char *) in)[i];
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  size_t idx = 0;
  int diff = 0;
  
  while (diff == 0 && idx < n) {
  	diff = ((char *)s1)[idx] - ((char *)s2)[idx];
  	idx ++;
  }
  return diff;
}

#endif
