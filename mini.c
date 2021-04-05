/* mini: a mini console for demos. */

#define _GNU_SOURCE
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <linux/seccomp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "arch.h"

#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0
#endif

const char optstring[] = "s:";
const struct option longopts[] = {
	{"image-size", required_argument, NULL, 's'}
};

const char* file;
size_t image_size = IMAGE_SIZE;

size_t parse_size(const char* str) {
	errno = 0;
	char* error;
	uintmax_t result = strtoumax(str, &error, 0);
	if (errno || result > SIZE_MAX)
		err(1, "strtoumax");
	uintmax_t unit = result;
	switch (*error) {
		case 0:
		case 'B': break;
		case 'K': result <<= 10; break;
		case 'M': result <<= 20; break;
		case 'G': result <<= 30; break;
		case 'T': result <<= 40; break;
		default: errx(1, "syntax error");
	}
	if (unit > result || result > SIZE_MAX)
		err(1, "integer overflow");
	return result;
}

int main(int argc, char* argv[]) {
	char* error;
	int option;
	while ((option = getopt_long
		(argc, argv, optstring, longopts, NULL)) != -1)
		switch (option) {
			case 's':
				image_size = parse_size(optarg);
				break;
			default:
				return EXIT_FAILURE;
		}
	file = argv[optind];

	/* Open the image file. */
	int fd;
	if (!file)
		fd = STDIN_FILENO;
	else if ((fd = open(file, 0)) < 0)
		err(2, "open");
	struct stat stat;
	if (fstat(fd, &stat) < 0)
		err(2, "fstat");

	/* Map the image area. */
	if (image_size < (size_t)stat.st_size + STACK_SIZE)
		image_size = stat.st_size + STACK_SIZE;
	int pagesize = getpagesize();
	image_size = pagesize*(1 + (image_size - 1)/pagesize);
	void* stack_addr = (void*)(IMAGE_ADDR + image_size);
	if (mmap((void*)BASE_ADDR, BASE_SIZE + image_size,
		PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANONYMOUS |
		MAP_SHARED | MAP_FIXED_NOREPLACE | MAP_NORESERVE,
		0, 0) == MAP_FAILED)
		err(3, "mmap");
	if (read(fd, (void*)ENTRY_ADDR, stat.st_size) < 0)
		err(3, "read");
	close(fd);

	/* Initialize SDL. */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		errx(4, "SDL_Init: %s", SDL_GetError());

	/* Create an SDL window. */
	SDL_Window* window = SDL_CreateWindow(file,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		VIDEO_W, VIDEO_H, 0);
	if (!window)
		errx(4, "SDL_CreateWindow: %s", SDL_GetError());

	/* Grab the window's surface. */
	SDL_Surface* surface = SDL_GetWindowSurface(window);
	if (!surface)
		errx(4, "SDL_GetWindowSurface: %s", SDL_GetError());

	/* Create an SDL surface for the video buffer. */
	SDL_Surface* video = SDL_CreateRGBSurfaceFrom((void*)VIDEO_ADDR,
		VIDEO_W, VIDEO_H, VIDEO_D, VIDEO_D/CHAR_BIT*VIDEO_W,
		0x000000FF, 0x0000FF00, 0x00FF0000, 0);
	if (!video)
		errx(4, "SDL_CreateRGBSurfaceFrom: %s", SDL_GetError());

	/* Fork a parallel process to run the image. */
	pid_t pid;
	if ((pid = fork()) == 0) {
		/* Close all fds. */
		DIR* fds;
		if ((fds = opendir("/proc/self/fd")) == NULL)
			err(6, "opendir");
		struct dirent* dir;
		while ((dir = readdir(fds)) != NULL) {
			errno = 0;
			long fd = strtol(dir->d_name, &error, 10);
			if (*error || errno || fd == dirfd(fds))
				continue;
			close(fd);
		}
		closedir(fds);

		/* Set strict seccomp (only allow read(), write(), exit()). */
		if (syscall(SYS_seccomp, SECCOMP_SET_MODE_STRICT, 0, NULL) < 0)
			err(6, "seccomp");

		/* Call the entrypoint. */
		CALL_ENTRYPOINT;

		/* Exit on return. SYS_exit must be called explicitly to */
		/* avoid Glibc's cleanup stuff from killing the process. */
		syscall(SYS_exit, 0);
	}
	if (pid < 0)
		err(5, "fork");

	/* Loop for SDL events. */
	for (;;) {
		SDL_Event event;
		if (SDL_WaitEventTimeout(&event, 50))
			if (event.type == SDL_QUIT)
				break;

		/* Blit the surface and loop. */
		SDL_BlitSurface(video, NULL, surface, NULL);
		SDL_UpdateWindowSurface(window);
	}

	/* Kill the parallel process and reap it. */
	if (kill(pid, SIGKILL) < 0)
		err(6, "kill");
	int status;
	if (wait(&status) < 0)
		err(6, "wait");
	return WEXITSTATUS(status);
}
