#define VIDEO_W 800
#define VIDEO_H 600
#define VIDEO_D 32

#define BASE_ADDR  0x00100000
#define BASE_SIZE  0x00300000
#define VIDEO_ADDR 0x00200000
#define VIDEO_SIZE 0x00200000

#define IMAGE_ADDR 0x00400000
#define ENTRY_ADDR 0x00400000

#define IMAGE_SIZE 0x00001000
#define STACK_SIZE 0x00001000

#ifdef __i386__
	#include "arch/x86.h"
#elif defined __x86_64__
	#include "arch/x86-64.h"
#else
	#error Unsupported architecture
#endif
