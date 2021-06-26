/* mini: a mini console for demos. */

#define _GNU_SOURCE
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#define MINI_BUILD
#include "arch.h"

#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x100000
#endif

const char optstring[] = "s:b:";
const struct option longopts[] = {
	{"image-size", required_argument, NULL, 's'},
	{"bits", required_argument, NULL, 'b'},
	{}
};

unsigned bits;
const char* file;
size_t image_size = IMAGE_SIZE;

size_t parse_size(const char* str) {
	errno = 0;
	char* error;
	uintmax_t result = strtoumax(str, &error, 0);
	if (errno)
		err(1, "strtoumax");
	int shift = 0;
	switch (*error) {
		case 0:
		case 'B': break;
		case 'K': shift = 10; break;
		case 'M': shift = 20; break;
		case 'G': shift = 30; break;
		case 'T': shift = 40; break;
		default: errx(1, "syntax error");
	}
	if (
		result & (-((uintmax_t)1 << shift)) ||
		(result <<= shift) > SIZE_MAX
	)
		errx(1, "integer overflow");
	return result;
}

int main(int argc, char* argv[]) {
	int option;
	while ((option = getopt_long
		(argc, argv, optstring, longopts, NULL)) != -1)
		switch (option) {
			case 's':
				image_size = parse_size(optarg);
				break;
			case 'b': {
				errno = 0;
				char* error;
				bits = strtoul(optarg, &error, 10);
				if (*error || errno)
					errx(1, "invalid bits: %s", optarg);
				break;
			}
			default:
				return EXIT_FAILURE;
		}
	file = argv[optind];

	/* Open the image file. */
	int fd;
	if (!file)
		fd = STDIN_FILENO;
	else if ((fd = open(file, 0)) < 0)
		err(-1, "open");
	struct stat st;
	if (fstat(fd, &st) < 0)
		err(-1, "fstat");

	/* Map the image area. */
	if (image_size < (size_t)st.st_size + STACK_SIZE)
		image_size = st.st_size + STACK_SIZE;
	int pagesize = getpagesize();
	image_size = pagesize*(1 + (image_size - 1)/pagesize + 1);
	printf("Image size: %zu bytes (%zu x %i pages)\n", image_size,
		image_size/pagesize, pagesize);
	image_size += BASE_SIZE;
	int mem = syscall(SYS_memfd_create, "mem", 0);
	if (mem < 0)
		err(-1, "memfd");
	if (ftruncate(mem, image_size) < 0)
		err(-1, "ftruncate");
	if (mmap((void*)BASE_ADDR, image_size,
		PROT_EXEC | PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_FIXED_NOREPLACE | MAP_NORESERVE,
		mem, 0) == MAP_FAILED)
		err(-1, "mmap");
	if (read(fd, (void*)ENTRY_ADDR, st.st_size) < 0)
		err(-1, "read");
	close(fd);
	if (dup2(mem, 3) < 0)
		err(-1, "dup2");
	close(mem);

	/* Initialize SDL. */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		errx(-2, "SDL_Init: %s", SDL_GetError());

	/* Create an SDL window. */
	SDL_Window* window = SDL_CreateWindow(file,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		VIDEO_W, VIDEO_H, SDL_WINDOW_RESIZABLE);
	if (!window)
		errx(-2, "SDL_CreateWindow: %s", SDL_GetError());

	/* Grab the window's surface. */
	SDL_Surface* surface = SDL_GetWindowSurface(window);

	/* Create an SDL surface for the video buffer. */
	SDL_Surface* video = SDL_CreateRGBSurfaceFrom((void*)VIDEO_ADDR,
		VIDEO_W, VIDEO_H, VIDEO_D, VIDEO_D/CHAR_BIT*VIDEO_W,
		0x000000FF, 0x0000FF00, 0x00FF0000, 0);
	if (!video)
		errx(-2, "SDL_CreateRGBSurfaceFrom: %s", SDL_GetError());

	/* Fork a parallel process to run the image. */
	pid_t ppid = getpid();
	pid_t pid = fork();
	if (pid < 0)
		err(-1, "fork");
	if (pid == 0) {
		/* Setup a parent death signal. This guarantees we die if our
		   parent dies. */
		if (prctl(PR_SET_PDEATHSIG, SIGKILL) < 0)
			err(-1, "prctl");
		if (getppid() != ppid)
			raise(SIGKILL);

		/* Run runimg. */
		struct stat st;
		if (stat("/proc/self/exe", &st) < 0)
			err(-1, "stat");
		char* path = malloc(st.st_size + 32);
		if (readlink("/proc/self/exe", path, st.st_size + 32) < 0)
			err(-1, "readlink");
		char* dir = strrchr(path, '/');
		if (dir)
			++dir;
		else {
			path[0] = '.';
			path[1] = '/';
			dir = &path[2];
		}
		if (!bits)
			strncpy(dir, "runimg", st.st_size + 32 - (dir - path));
		else
			snprintf(dir, st.st_size + 32 - (dir - path), "runimg%u", bits);
		path[st.st_size + 32] = 0;
		execl(path, path, NULL);
		err(-1, "execl");
	}

	/* Loop for SDL events. */
	for (;;) {
		SDL_Event event;
		if (SDL_WaitEventTimeout(&event, 50))
			switch (event.type) {
			case SDL_QUIT:
				return EXIT_SUCCESS;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					/* Grab the window's surface
					   after resizing. */
					surface = SDL_GetWindowSurface(window);
					break;
				}
				break;
			}

		/* Blit the surface and loop. */
		SDL_BlitScaled(video, NULL, surface, NULL);
		SDL_UpdateWindowSurface(window);
	}
}
