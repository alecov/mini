#pragma once
#include "arch.h"

union pixel { struct { unsigned char r, g, b, a; }; unsigned int pix; };

#define VIDEO(x, y) ((volatile union pixel*)VIDEO_ADDR)[(y)*VIDEO_W + (x)]
