NASM = nasm
LINK.asm = $(NASM) $(NASMFLAGS)

CHMOD = chmod

CFLAGS = -O3 -g -Wall -Wextra

.PHONY: all
all: mini runimg runimg32 runimg64 sample

runimg.c:;
runimg%: runimg.c; $(LINK.c) -m$* $^ $(LOADLIBES) $(LDLIBS) -o $@

.PHONY: sample
sample: \
	$(patsubst %.c,%.img,$(wildcard sample/*.c)) \
	$(patsubst %.s,%.img,$(wildcard sample/*/*.s)) \
	$(patsubst %.S,%.img,$(wildcard sample/*/*.S)) \
	$(patsubst %.asm,%.img,$(wildcard sample/*/*.asm))

.PHONY: clean
clean:; $(RM) mini runimg runimg32 runimg64 sample/*.img sample/*/*.img

mini: LDLIBS = -lSDL2
sample/%.img: CPPFLAGS = -I.
sample/%.img: CFLAGS = -fno-pie -fno-math-errno -Os
sample/%.img: LDLIBS = -nostdlib -static -Tmini.ld -lm

sample/x86/%.img: ASFLAGS += -m32
sample/x86/%.img: CFLAGS += -m32
sample/x86-64/%.img: ASFLAGS += -m64
sample/x86-64/%.img: CFLAGS += -m64

sample/%.img: sample/%.c;   $(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@ && $(CHMOD) -x $@
sample/%.img: sample/%.s;   $(LINK.s) $^ $(LOADLIBES) $(LDLIBS) -o $@ && $(CHMOD) -x $@
sample/%.img: sample/%.S;   $(LINK.S) $^ $(LOADLIBES) $(LDLIBS) -o $@ && $(CHMOD) -x $@
sample/%.img: sample/%.asm; $(LINK.asm) $^ -o $@
