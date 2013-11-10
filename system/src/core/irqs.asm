; Based on code from http://www.osdever.net/bkerndev/Docs/irqs.htm

global _irq0
global _irq1
global _irq2
global _irq3
global _irq4
global _irq5
global _irq6
global _irq7
global _irq8
global _irq9
global _irq10
global _irq11
global _irq12
global _irq13
global _irq14
global _irq15
global _irq0x80

global _idtload
extern _idtp

_idtload:
    lidt [_idtp]
    ret

; 0x20: IRQ0
_irq0:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x00000020
jmp irq_common_stub

; 0x21: IRQ1
_irq1:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x00000021
jmp irq_common_stub

; 0x22: IRQ2
_irq2:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x00000022
jmp irq_common_stub

; 0x23: IRQ3
_irq3:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x00000023
jmp irq_common_stub

; 0x24: IRQ4
_irq4:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x00000024
jmp irq_common_stub

; 0x25: IRQ5
_irq5:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x00000025
jmp irq_common_stub

; 0x26: IRQ6
_irq6:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x00000026
jmp irq_common_stub

; 0x27: IRQ7
_irq7:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x00000027
jmp irq_common_stub

; 0x28: IRQ8
_irq8:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x00000028
jmp irq_common_stub

; 0x29: IRQ9
_irq9:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x00000029
jmp irq_common_stub

; 0x2A: IRQ10
_irq10:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x0000002A
jmp irq_common_stub

; 0x2B: IRQ11
_irq11:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x0000002B
jmp irq_common_stub

; 0x2C: IRQ12
_irq12:
cli
push dword 0     ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x0000002C
jmp irq_common_stub

; 0x2D: IRQ13
_irq13:
cli
push dword 0    ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x0000002D
jmp irq_common_stub

; 0x2E: IRQ14
_irq14:
cli
push dword 0    ; Note that these don't push an error code on the stack:
                ; We need to push a dummy error code
push dword 0x0000002E
jmp irq_common_stub

; 0x2F: IRQ15
_irq15:
cli
push dword 0
push dword 0x0000002F
jmp irq_common_stub

; For system calls. The argument byte refers to which
; system function we want.
; 0x80: IRQ0x80
_irq0x80:
cli
push dword 0
push dword 0x00000080
jmp irq_common_stub


extern _irqhandler

irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, _irqhandler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
