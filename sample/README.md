# samples

This is a directory of some code samples.

Note on `libm`: in some installations, `libm.a` is actually a linker script
pointing to the real static libraries. If you get link errors regarding `libm`,
try replacing `-lm` with the real library, eg. `-l:libm-2.33.a`.
