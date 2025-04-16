    .include "xc.inc"

    .text                       ;BP (put the following data in ROM(program memory))

    ; This is a library, thus it can *not* contain a _main function: the C file will
    ; define main().  However, we will need a .global statement to make available ASM
    ; functions to C code.
    ; All functions utilized outside of this file will need to have a leading 
    ; underscore (_) and be included in a comment delimited list below.
    .global _write_0, _write_1,_ETO_wait_1ms,_ETO_wait_100us

    _ETO_wait_100us:
	;1600 cycles
    ;2 cycles for call
    repeat #1593 ; 1 cycle
    nop ;1593 + 1
    return ;3 cycles

    ; = 1600 cycles

    _ETO_wait_1ms:
    ;16000
       ;2 cycles for call
    repeat #15993    ;1 cycle
    nop    ;n + 1 cycles
    return    ;3 cycles
     _write_0:
	inc LATA
       repeat #3;2
       nop

       clr LATA

       repeat #5;8
       nop
       return 
     _write_1:
	inc LATA;1
	repeat #8;
	nop

	clr LATA
	repeat #2;4
	nop 
	return


