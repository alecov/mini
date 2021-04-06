/*
 * x86 entrypoint:
 * - Do not constrain eax and allow it to spill;
 * - ebp cannot be constrained at all.
 */
#define CALL_ENTRYPOINT do {                                                  \
	register void* ebx __asm__("ebx") = (void*)IMAGE_ADDR;                \
	register void* ecx __asm__("ecx") = (void*)-1;                        \
	register void* edx __asm__("edx") = (void*)0;                         \
	register void* esi __asm__("esi") = (void*)ENTRY_ADDR;                \
	register void* edi __asm__("edi") = (void*)VIDEO_ADDR;                \
	__asm__(                                                              \
		"xorl %%eax, %%eax\n\t"                                       \
		"xorl %%ebp, %%ebp\n\t"                                       \
		"movl %[stack_addr], %%esp\n\t"                               \
		"sahf\n\t"                                                    \
		"fninit\n\t"                                                  \
		"jmpl *%%esi\n\t"                                             \
		"hlt\n\t"                                                     \
		:: "r"(ebx), "r"(ecx), "r"(edx),                              \
		   "r"(esi), "r"(edi),                                        \
		   [stack_addr]"g"(stack_addr)                                \
	);                                                                    \
	__builtin_unreachable();                                              \
} while (0)
