NASM = nasm
COMPILE.asm = $(NASM) $(OUTPUT_OPTION) $<

.PHONY: all
all: mini sample

.PHONY: sample
sample: $(patsubst %.asm,%.img,$(wildcard sample/*.asm))

.PHONY: clean
clean:; $(RM) mini sample/*.img

mini: LDLIBS = -lSDL2
sample/%.img: sample/%.asm; $(COMPILE.asm)
