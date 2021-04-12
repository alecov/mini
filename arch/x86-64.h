/*
 * x86-64 entrypoint:
 * - Do not constrain rbp and allow it to spill.
 */
#define CALL_ENTRYPOINT(stack) do {                                           \
	register void* rax __asm__("rax") = (void*)0;                         \
	register void* rbx __asm__("rbx") = (void*)IMAGE_ADDR;                \
	register void* rcx __asm__("rcx") = (void*)-1;                        \
	register void* rdx __asm__("rdx") = (void*)0;                         \
	register void* rsi __asm__("rsi") = (void*)ENTRY_ADDR;                \
	register void* rdi __asm__("rdi") = (void*)VIDEO_ADDR;                \
	register void* r8  __asm__("r8")  = (void*)0;                         \
	register void* r9  __asm__("r9")  = (void*)0;                         \
	register void* r10 __asm__("r10") = (void*)0;                         \
	register void* r11 __asm__("r11") = (void*)0;                         \
	register void* r12 __asm__("r12") = (void*)0;                         \
	register void* r13 __asm__("r13") = (void*)0;                         \
	register void* r14 __asm__("r14") = (void*)0;                         \
	register void* r15 __asm__("r15") = (void*)0;                         \
	__asm__(                                                              \
		"xorl %%ebp, %%ebp\n\t"                                       \
		"movq %[stack_addr], %%rsp\n\t"                               \
		"sahf\n\t"                                                    \
		"fninit\n\t"                                                  \
		"jmpq *%%rsi\n\t"                                             \
		"hlt\n\t"                                                     \
		:: "r"(rax), "r"(rbx), "r"(rcx), "r"(rdx),                    \
		   "r"(rsi), "r"(rdi),                                        \
		   "r"(r8),  "r"(r9),  "r"(r10), "r"(r11),                    \
		   "r"(r12), "r"(r13), "r"(r14), "r"(r15),                    \
		   [stack_addr]"m"(stack)                                     \
	);                                                                    \
	__builtin_unreachable();                                              \
} while (0)
