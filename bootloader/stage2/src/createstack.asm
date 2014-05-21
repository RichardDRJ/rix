extern _load_kernel
global _start

_start:
    mov esp, [stack_end]    ; Set the stack to start at 12 KB in (and expand down, obviously).
    mov ebp, [stack_end]    ; Set the stack to start at 12 KB in (and expand down, obviously).

    cli
    call _load_kernel
    jmp $

stack_end: dd 0x00000FFF
