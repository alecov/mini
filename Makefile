NASM = nasm
LINK.asm = $(NASM) $(NASMFLAGS)

CHMOD = chmod

CFLAGS = -O3 -g

.PHONY: all
all: mini sample

.PHONY: sample
sample: \
	$(patsubst %.c,%.img,$(wildcard sample/*.c)) \
	$(patsubst %.s,%.img,$(wildcard sample/*/*.s)) \
	$(patsubst %.asm,%.img,$(wildcard sample/*/*.asm))

.PHONY: clean
clean:; $(RM) mini sample/*.img sample/*/*.img

mini: LDLIBS = -lSDL2
sample/%.img: CPPFLAGS = -I.
sample/%.img: CFLAGS = -nostdlib -fno-pie -fno-math-errno -Os
sample/%.img: LDLIBS = -Tmini.ld -lm

sample/%.img: sample/%.c;   $(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@ && $(CHMOD) -x $@
sample/%.img: sample/%.s;   $(LINK.s) $^ $(LOADLIBES) $(LDLIBS) -o $@ && $(CHMOD) -x $@
sample/%.img: sample/%.asm; $(LINK.asm) $^ -o $@
