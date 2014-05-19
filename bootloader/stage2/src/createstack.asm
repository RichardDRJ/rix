extern _load_kernel
global _ptrmemtablestart
global _ptrmemtableend
global _start

_start:
    pop word [_ptrmemtableend]
    pop word [_ptrmemtablestart]

    mov esp, [stack_end]    ; Set the stack to start at 12 KB in (and expand down, obviously).
    mov ebp, [stack_end]    ; Set the stack to start at 12 KB in (and expand down, obviously).


    cli
    call _load_kernel
    jmp $

stack_end: dd 0x00000FFF

_ptrmemtablestart: dd 0x00000000
_ptrmemtableend: dd 0x00000000
_ptrmemsize: dd 0x00000000