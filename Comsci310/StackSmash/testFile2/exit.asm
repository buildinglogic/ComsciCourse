;exit.asm
[SECTION .text]
global _start
_start:
        xor eax, eax       ;exit is syscall 1
        mov al, 1       ;exit is syscall 1
        xor ebx,ebx     ;zero out ebx
        int 0x80