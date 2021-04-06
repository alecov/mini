#include <math.h>

#include <mini.h>

void _start(void) {
	int x, y, z, d;
	z = 0, d = 1;
	for (;;) {
		for (y = 0; y < VIDEO_H; ++y)
			for (x = 0; x < VIDEO_W; ++x) {
				double xp = ((double)x - VIDEO_W/2)/VIDEO_H;
				double yp = (VIDEO_H/2 - (double)y)/VIDEO_H;
				double r = sqrt(xp*xp + yp*yp);
				VIDEO(x, y).r = 256/(r*0.001*z);
				VIDEO(x, y).g = 256*(r*0.001*z);
			}
		z += d;
		if (z > 1000 || z < -1000) d *= -1;
	}
}
