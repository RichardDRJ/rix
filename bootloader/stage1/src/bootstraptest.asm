[bits 32]

mov eax, message
push eax
call _writeToScreen
pop eax

jmp $

_setPictureMode:
    ret

_writeToScreen:
    push ebp
    mov ebp, esp                  ; Set up the stack frame.
    
    mov ebx, 0xb8000
;    xor bx, bx                  ; Set BX to point to the first character
    
    mov esi, DWORD [ebp + 8]        ; Copy the pointer to the string into esi.

    startLoop:

    mov eax, [esi]
    inc esi
    cmp al, 0                   ; Check if we've reached a null char.
    je endWrite                 ; If we have, exit the loop.

    mov byte [ebx], al        ; Write the current character in the string.
    inc ebx                      ; Set BX to point to the current attribute byte
    mov byte [ebx], 0x0f      ; Write the attribute byte
    inc ebx                      ; Set BX to point to the second character

    jmp startLoop

    endWrite:
    pop ebp
    ret


message: db "Holy hell, the bootloader works!",0
