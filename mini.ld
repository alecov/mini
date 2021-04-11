OUTPUT_FORMAT(binary);

MEMORY { IMAGE ("rwx"): ORIGIN = 0x00400000, LENGTH = -1 }

SECTIONS {
	init : { *(.init) *(SORT(.init*)); } > IMAGE
	text : { *(.text) *(SORT(.text*)); } > IMAGE
	fini : { *(.fini) *(SORT(.fini*)); } > IMAGE

	/* rodata comes first so data can grow forwards. */
	rodata : { *(.rodata) *(SORT(.rodata*)); } > IMAGE
	data : { *(.data) *(SORT(.data*)); } > IMAGE
	rest : { *(SORT(*)); } > IMAGE
}
