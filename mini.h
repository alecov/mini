#pragma once
#include "arch.h"

#define ENTRY void __attribute__((__section__(".init"))) _entry(void)

union pixel { struct { unsigned char r, g, b, a; }; unsigned int pix; };

#define VIDEO(x, y) ((volatile union pixel*)VIDEO_ADDR)[(y)*VIDEO_W + (x)]
