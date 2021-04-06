NASM = nasm
LINK.asm = $(NASM) $(NASMFLAGS)

CHMOD = chmod

CFLAGS = -O3 -g

.PHONY: all
all: mini sample

.PHONY: sample
sample: $(patsubst %.asm,%.img,$(wildcard sample/*/*.asm))

.PHONY: clean
clean:; $(RM) mini sample/*/*.img

mini: LDLIBS = -lSDL2
sample/%.img: sample/%.asm; $(LINK.asm) $^ -o $@
