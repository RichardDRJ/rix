[bits 16]

call _checka20          ; Check if the a20 line is enabled, giving us access to more than 1MB of memory.
cmp ax, ax
jnz skipenablea20
call _seta20
skipenablea20:

; Set data segment and code segment to span all memory in GDT (based on how the Linux kernel does it).
; We don't need segmentation. If we're going to use virtual memory management, paging sounds pretty swell.
; That said, for the purposes for which this is being written, no virtual memory management will be needed.
call _setGDT

; For 386 processors and above, to enter protected mode, we set the
; least significant bit of the control register.
mov eax, cr0
or eax, 1
mov cr0, eax

; To do the same on 286 processors, we need to do that to the
; machine status word.
smsw ax
or ax,1
lmsw ax

jmp $

_setGDT:
    lgdt [GDTR]

_checka20:
    push ds
    push es
    push si

    xor ax, ax
    mov es, ax

    not ax
    mov ds, ax

    mov si, 0x0500
    mov di, 0x0510

    mov WORD[es:si], 0x0000
    mov WORD[fs:di], 0xFFFF

    mov ax, [es:si]
    cmp [fs:di], ax

    je a20notenabled

    mov ax, 1

    pop si
    pop es
    pop ds
    ret
 
a20notenabled:
    pop si
    pop es
    pop ds
    xor ax, ax
    ret

_seta20:
    cli

    call a20wait
    mov al,0xAD
    out 0x64,al

    call a20wait
    mov al,0xD0
    out 0x64,al

    call a20wait2
    in al,0x60
    push eax

    call a20wait
    mov al,0xD1
    out 0x64,al

    call a20wait
    pop eax
    or al,2
    out 0x60,al

    call a20wait
    mov al,0xAE
    out 0x64,al

    call a20wait
    sti
    ret

a20wait:
    in al,0x64
    test al,2
    jnz a20wait
    ret


a20wait2:
    in al,0x64
    test al,1
    jz a20wait2
    ret

;   cli
;   Load GDT
;   Protected mode
;   Load 32-bit code
;   ???
;   Profit


;   Global descriptor table.
;   From https://staktrace.com/nuggets/index.php?id=11&replyTo=0&cutId=0
GDTR:                               ; GDT register data
    dw  GDT_End - GDT - 1           ; 16-bit size limit of GDT
    dd  GDT                         ; 32-bit linear address of GDT
GDT:
    dd  0x00000000, 0x00000000      ; Null descriptor
    dd  0x0000FFFF, 0x00CF9C00      ; Code segment
    dd  0x0000FFFF, 0x00CF9200      ; Data segment
GDT_End:
