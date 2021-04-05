#define CALL_ENTRYPOINT \
	register void* rax __asm__("rax") = 0;                                \
	register void* rbx __asm__("rbx") = (void*)IMAGE_ADDR;                \
	register void* rcx __asm__("rcx") = (void*)-1;                        \
	register void* rdx __asm__("rdx") = 0;                                \
	register void* rsi __asm__("rsi") = (void*)ENTRY_ADDR;                \
	register void* rdi __asm__("rdi") = (void*)VIDEO_ADDR;                \
	register void* r8  __asm__("r8")  = 0;                                \
	register void* r9  __asm__("r9")  = 0;                                \
	register void* r10 __asm__("r10") = 0;                                \
	register void* r11 __asm__("r11") = 0;                                \
	register void* r12 __asm__("r12") = 0;                                \
	register void* r13 __asm__("r13") = 0;                                \
	register void* r14 __asm__("r14") = 0;                                \
	register void* r15 __asm__("r15") = 0;                                \
	static void* oldrbp;                                                  \
	static void* oldrsp;                                                  \
	__asm__(                                                              \
		"sahf\n\t"                                                    \
		"fninit\n\t"                                                  \
		"movq %%rbp, %[oldrbp]\n\t"                                   \
		"movq %%rsp, %[oldrsp]\n\t"                                   \
		"xorl %%ebp, %%ebp\n\t"                                       \
		"movq %[stack_addr], %%rsp\n\t"                               \
		"subq $8, %%rsp\n\t"                                          \
		"call *%%rsi\n\t"                                             \
		"movq %[oldrbp], %%rbp\n\t"                                   \
		"movq %[oldrsp], %%rsp\n\t"                                   \
	: [oldrbp]"=m"(oldrbp),                                               \
	  [oldrsp]"=m"(oldrsp),                                               \
	  "+X"(rax), "+X"(rbx), "+X"(rcx), "+X"(rdx),                         \
	  "+X"(r8),  "+X"(r9),  "+X"(r10), "+X"(r11),                         \
	  "+X"(r12), "+X"(r13), "+X"(r14), "+X"(r15)                          \
	: [stack_addr]"m"(stack_addr),                                        \
	  [entry_addr]"i"(ENTRY_ADDR),                                        \
	  "X"(rsi), "X"(rdi)                                                  \
	: "cc", "memory",                                                     \
	  "st",    "st(1)", "st(2)", "st(3)",                                 \
	  "st(4)", "st(5)", "st(6)", "st(7)",                                 \
	  "mm0", "mm1", "mm2", "mm3",                                         \
	  "mm4", "mm5", "mm6", "mm7",                                         \
	  "zmm0", "zmm1", "zmm2", "zmm3",                                     \
	  "zmm4", "zmm5", "zmm6", "zmm7"                                      \
	); __asm__("" :: "X"(rax) : "rsi", "rdi") /* Avoid 30 ops GCC limit. */
