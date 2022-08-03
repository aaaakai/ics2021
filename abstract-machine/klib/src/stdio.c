#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char* itoa(int num,char* str,int radix)
{
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned unum;
    int i=0,j,k;
 
    if(radix==10&&num<0)
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;
    do
    {
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
    }while(unum);
 
    str[i]='\0';
    if(str[0]=='-') k=1;
    else k=0;
 
    char temp;
    for(j=k;j<=(i-1)/2;j++)
    {
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
 
}
int printf(const char *fmt, ...) {
  char buf[255];
  va_list args;
	va_start(args, fmt);
	int result = vsprintf(buf, fmt, args);
	va_end(args);
	for (char *p = buf; *p; p++) {
		putch(*p);
	}
	return result;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int result = vsnprintf(out, 2147483647, fmt, ap);
  return result;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int result = vsnprintf(out, 2147483647, fmt, args);
	va_end(args);
	return result;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int result = vsnprintf(out, n, fmt, args);
	va_end(args);
	return result;
}

int vsnprintf(char *out, size_t max_len, const char *fmt, va_list ap) {
	char *str = out;
	while(*fmt) {
		if(*fmt != '%') {
			*str = *(fmt++);
			str++;
			continue;
		}
		fmt++;
		
		//record bit width
		const char *format_begin = fmt;
		char bit_width[32];
		int num_len = 0;
		int width = 0;
		while ((48 <= *(fmt)) && (*(fmt) <= 57)) {
			num_len ++;
			fmt ++;
		}
		//int has_width = num_len != 0;
		if (num_len != 0) {
			strncpy(bit_width, format_begin, num_len);
			width = atoi(bit_width);
		}
		
		switch (*(fmt++)) {
			case 's' : {
				char *t = va_arg(ap, char*);
				int str_len = strlen(t);
				for (int i = 0; i < str_len; i ++) {
					*(str++) = t[i];
				}
				break;
			}
			case 'd' : {
				int n = va_arg(ap, int);
				//uint32_t num = (uint32_t) n;
				char num_str[32];
				//sprintf(num_str, "%d", n);
				itoa(n, num_str, 10);
				int num_str_len = strlen(num_str);
				if (num_len != 0) {
					if (num_str_len < width) {
						memset(str, ' ', width - num_str_len);
						str += width - num_str_len;
					}
				}
				strcpy(str, num_str);
				str += num_str_len;
				break;
			}
			default : {
				printf("%c\n", *(--fmt));
				break;
			}
		}
	}
	*str = '\0';
	int pre_len = strlen(out);
	if (pre_len > max_len) {
		memset(out+max_len, '\0', pre_len - max_len);
	} 
	return 0;
}

#endif
