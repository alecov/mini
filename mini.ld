OUTPUT_FORMAT(binary);

MEMORY { IMAGE : ORIGIN = 0x00400000, LENGTH = -1 }
SECTIONS { .text : { *(.text); } > IMAGE }
