# mini

A mini machine for writing software renderers.

mini maps a raw image to a fixed memory map and runs it. It exposes a
framebuffer area (also at a fixed address) for the code to mess with it.

mini runs code in a parallel process under strict seccomp mode — image code
cannot do anything else besides drawing, getting hot and crashing.

## Build

`make`. Linux only. Requires SDL 2.0.

Currently only x86 and x86-64 are supported.

## Run

`./mini image.img`

Use `--image-size=size` to allocate more VA space for the image.

See the `sample` directory for some code samples.

## Images

See `arch.h` and `arch/*.h` for the memory map and initial register states.
Crashing images are fair game.

Some support for C code is provided. You need to use the `mini.ld` linker
script and compile (at least) with `-nostdlib` and `-fno-pie`.
