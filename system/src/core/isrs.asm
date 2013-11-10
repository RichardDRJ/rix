; Based on code from http://www.osdever.net/bkerndev/Docs/irqs.htm

global _isr0
global _isr1
global _isr2
global _isr3
global _isr4
global _isr5
global _isr6
global _isr7
global _isr8
global _isr9
global _isr10
global _isr11
global _isr12
global _isr13
global _isr14
global _isr15
global _isr16
global _isr17
global _isr18
global _isr19
global _isr20
global _isr21
global _isr22
global _isr23
global _isr24
global _isr25
global _isr26
global _isr27
global _isr28
global _isr29
global _isr30
global _isr31

extern _isrhandler

; This is a stub that we have created for IRQ based ISRs. This calls
; '_irq_handler' in our C code. We need to create this in an 'irq.c'
isrcommonstub:
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
    mov eax, _isrhandler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

; 0x00: ISR0
_isr0:
	cli
	push dword 0
	push dword 0x00000000
	jmp isrcommonstub

; 0x01: ISR1
_isr1:
	cli
	push dword 0
	push dword 0x00000001
	jmp isrcommonstub

; 0x02: ISR2
_isr2:
	cli
	push dword 0
	push dword 0x00000002
	jmp isrcommonstub

; 0x03: ISR3
_isr3:
	cli
	push dword 0
	push dword 0x00000003
	jmp isrcommonstub

; 0x04: ISR4
_isr4:
	cli
	push dword 0
	push dword 0x00000004
	jmp isrcommonstub

; 0x05: ISR5
_isr5:
	cli
	push dword 0
	push dword 0x00000005
	jmp isrcommonstub

; 0x06: ISR6
_isr6:
	cli
	push dword 0
	push dword 0x00000006
	jmp isrcommonstub

; 0x07: ISR7
_isr7:
	cli
	push dword 0
	push dword 0x00000007
	jmp isrcommonstub

; 0x08: ISR8
_isr8:
	cli
	push dword 0x00000008
	jmp isrcommonstub

; 0x09: ISR9
_isr9:
	cli
	push dword 0
	push dword 0x00000009
	jmp isrcommonstub

; 0x0a: ISR10
_isr10:
	cli
	push dword 0x0000000a
	jmp isrcommonstub

; 0x0b: ISR11
_isr11:
	cli
	push dword 0x0000000b
	jmp isrcommonstub

; 0x0c: ISR12
_isr12:
	cli
	push dword 0x0000000c
	jmp isrcommonstub

; 0x0d: ISR13
_isr13:
	cli
	push dword 0x0000000d
	jmp isrcommonstub

; 0x0e: ISR14
_isr14:
	cli
	push dword 0x0000000e
	jmp isrcommonstub

; 0x0f: ISR15
_isr15:
	cli
	push dword 0
	push dword 0x0000000f
	jmp isrcommonstub

; 0x10: ISR16
_isr16:
	cli
	push dword 0
	push dword 0x00000010
	jmp isrcommonstub

; 0x11: ISR17
_isr17:
	cli
	push dword 0
	push dword 0x00000011
	jmp isrcommonstub

; 0x12: ISR18
_isr18:
	cli
	push dword 0
	push dword 0x00000012
	jmp isrcommonstub

; 0x13: ISR19
_isr19:
	cli
	push dword 0
	push dword 0x00000013
	jmp isrcommonstub

; 0x14: ISR20
_isr20:
	cli
	push dword 0
	push dword 0x00000014
	jmp isrcommonstub

; 0x15: ISR21
_isr21:
	cli
	push dword 0
	push dword 0x00000015
	jmp isrcommonstub

; 0x16: ISR22
_isr22:
	cli
	push dword 0
	push dword 0x00000016
	jmp isrcommonstub

; 0x17: ISR23
_isr23:
	cli
	push dword 0
	push dword 0x00000017
	jmp isrcommonstub

; 0x18: ISR24
_isr24:
	cli
	push dword 0
	push dword 0x00000018
	jmp isrcommonstub

; 0x19: ISR25
_isr25:
	cli
	push dword 0
	push dword 0x00000019
	jmp isrcommonstub

; 0x1a: ISR26
_isr26:
	cli
	push dword 0
	push dword 0x0000001a
	jmp isrcommonstub

; 0x1b: ISR27
_isr27:
	cli
	push dword 0
	push dword 0x0000001b
	jmp isrcommonstub

; 0x1c: ISR28
_isr28:
	cli
	push dword 0
	push dword 0x0000001c
	jmp isrcommonstub

; 0x1d: ISR29
_isr29:
	cli
	push dword 0
	push dword 0x0000001d
	jmp isrcommonstub

; 0x1e: ISR30
_isr30:
	cli
	push dword 0
	push dword 0x0000001e
	jmp isrcommonstub

; 0x1f: ISR31
_isr31:
	cli
	push dword 0
	push dword 0x0000001f
	jmp isrcommonstub

