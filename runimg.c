/* runimg: utility process for running images. */

#define _GNU_SOURCE
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <linux/seccomp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

#define MINI_BUILD
#include "arch.h"

#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x100000
#endif

int main() {
	/* Map the image area. */
	struct stat st;
	if (fstat(3, &st) < 0)
		err(-1, "fstat");
	off_t image_size = st.st_size;
	if (mmap((void*)BASE_ADDR, image_size,
		PROT_EXEC | PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_FIXED_NOREPLACE | MAP_NORESERVE,
		3, 0) == MAP_FAILED)
		err(-1, "mmap");
	close(3);

	/* Close all remaining fds. */
	DIR* fds;
	if ((fds = opendir("/proc/self/fd")) == NULL)
		err(-1, "opendir");
	struct dirent* dir;
	while ((dir = readdir(fds)) != NULL) {
		errno = 0;
		char* error;
		long fd = strtol(dir->d_name, &error, 10);
		if (*error || errno ||
			fd == STDIN_FILENO ||
			fd == STDOUT_FILENO ||
			fd == STDERR_FILENO ||
			fd == dirfd(fds))
			continue;
		close(fd);
	}
	closedir(fds);

	/* Set strict seccomp (only allow read(), write(), exit()). */
	if (syscall(SYS_seccomp, SECCOMP_SET_MODE_STRICT, 0, NULL) < 0)
		err(-1, "seccomp");

	/* Call the entrypoint. */
	int pagesize = getpagesize();
	void* stack_addr = (void*)(BASE_ADDR + image_size - pagesize);
	CALL_ENTRYPOINT(stack_addr);
}
