[bits   16]
org 0x7c00

jmp short start
nop

;*********************************************
;       BIOS Parameter Block  "BPB"
;       Based on code found at
;   http://board.flatassembler.net/topic.php?t=14506
;       And info from
;   http://wiki.osdev.org/FAT
;*********************************************

OEM_ID                  db 'BFYTWOS '
sectorSize:             dw 0x0200
sectorsPerCluster:      db 0x01
reservedSectors:        dw 0x0003
numberOfFATs:           db 0x02
rootEntries:            dw 0x0040
totalSectors:           dw 0x5FFF
media:                  db 0xf8
sectorsPerFAT:          dw 0x000F
sectorsPerTrack:        dw 0x003D

headsPerCylinder:       dw 0x0002
hiddenSectors:          dd 0x00000000
totalSectorsBig:        dd 0x00000000
driveNumber:            db 0x00
unused:                 db 0x00
extBootSignature:       db 0x29
serialNumber:           dd 0xa0a1a2a3
volumeLabel:            db "RDRJAWESOME"
fileSystem:             db "FAT12   " 

maxNumClusters:         dw 0x780
rootStart:              dw 0x0000
;********************************************** 

;**********************************************
;   This is for reading from the disk.
;   Int 0x13 function 0x08 provides this
;   data and it's good to have somewhere
;   to store it.
;**********************************************
;numDrives               db 0x00
numHeads                db 0x00
numCylinders            dw 0x0000
numSectorsperTrack      db 0x00

sectorbufferstart       dw 0x5000
sectorbuffer            dw 0x5000   ; A point in memory to which we will load the system from disk.
                                    ; Since we're in real mode the bootloader has no access
                                    ; to memory past 1MB, so here we cannot pass 0xFFFF.

FATstart                dw 0x5000
FATtype                 db 0x00     ; Indicates what type of filesystem we're using. By default,   
                                    ; it's empty. We detect this later.


start:
    cli
    mov ax,cs               ; Setup segment registers
    mov ds,ax               ; Make DS correct
    mov es,ax               ; Make ES correct
    mov ss,ax               ; Make SS correct
    mov bp,7c00h
    mov sp,7c00h            ; Setup a stack

    sti

    push stage1loaded
    call _writeToScreen
    pop ax

    call _getDriveParams

    push loadingstage2
    call _writeToScreen
    pop ax

    call _readStage2

    push stage2loaded
    call _writeToScreen
    pop ax

    call _loadFromFAT

    push loadedfromFAT
    call _writeToScreen
    pop ax

    call _seta20

    call _memdetect

    push WORD 0x9000
    push di
   
    cli

    ;push DWORD 0x00000008
    ;push WORD [sectorbufferstart]
    ;push WORD 0x0000
    ;retf

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

    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; THIS IS BECOMING 0xFFFF9000 - WHAT THE BITCH?!
    ; Pass the start location...
    ; mov eax, 0x9000
    ; push eax
    ; ... and the end location of the memory table into the kernel.
    ; mov eax, 0xFFFF
    ; and ax, di
    ; push eax

    push WORD 0x08
    push WORD[sectorbufferstart]

    retf

_setGDT:
    xor eax, eax
    mov ax, ds
    shl eax, 4
    add eax, GDT
    mov [GDTR + 2], eax
    mov eax, GDT_End
    sub eax, GDT
    mov [GDTR], ax
    lgdt [GDTR]
    ret


_readStage2:
    push ax
    push WORD[sectorbuffer]     ; Save the current state of buffer locationage.
    mov WORD[sectorbuffer], 0x7e00
                                ; We want to copy stage 2 of the bootloader to the same
                                ; location as the one at which it is in the file.
    push WORD 2                 ; Copy 2 sectors (the length of the rest of the bootloader)...
    push WORD 1                 ; ... Starting from sector 1.
    call _readSector            ; Read stage 2.
    pop ax
    pop ax
    pop WORD[sectorbuffer]
    pop ax
    ret

_readSector:                    ; Read sector specified in a variable pushed to the stack
                                ; from disk.

    mov dl, 0x80                ; Choose the drive.
    mov ah, 0x00                ; We want to reset the drive so we're
                                ; at the first sector.

    push es
    push di
    push bx
    xor bx, bx
    mov es, bx
    xor di, di
    int 0x13                    ; Execute function 0 of interrupt 0x13.
    pop bx
    pop di
    pop es

    push bp
    mov bp, sp
    push ax
    push bx
    push cx
;    push dx

    sub sp, 8                   ; Create space for local variables for logical -> physical
                                ; disk address calculation.


    xor dx, dx                  ; When using div or idiv, the remainder is stored in dx. Good
                                ; for modulo.
    ;**********************************************
    ;   First, calculate the logical block address
    ;   divided by the number of sectors per track.
    ;   This is used in every calculation so it's
    ;   best to do it before and then reuse it.
    ;**********************************************
    mov ax, [bp + 4]            ; Get the logical sector number from the stack.
    mov bl, BYTE[numSectorsperTrack]
    div bx                          
    mov [bp - 16], ax                

    ;**********************************************
    ;   The sector number is just the remainder
    ;   from this operation + 1.
    ;**********************************************
    add dx, 1
    mov [bp - 12], dx

    ;**********************************************
    ;   Now calculate the head number.
    ;**********************************************
    xor ax, ax
    mov al, [numHeads]
    
    xor dx, dx                      ; Head number = (logical block address / sectors per track)
    mov bx, WORD[numCylinders]      ;                   % heads per cylinder.
    div bx
    
    xor dx,dx
    mov [bp - 14], ax
    mov ax, [bp - 16]
    div WORD[bp - 14]
    mov [bp - 10], dl            ; Store the head number in [bp - 2]. This is stored as a BYTE
                                ; instead of a WORD, because it is passed to function 0x02 as
                                ; a BYTE.

    ;**********************************************
    ;   Then, the sector number.
    ;**********************************************
    ;mov ax, [bp + 4]
    ;xor dx, dx
    ;mov bl, BYTE[numSectorsperTrack]
    ;div bx
    ;add dx, 1

    ;mov [bp - 4], dx            ; Store the sector number in [bp - 4].

    ;**********************************************
    ;   Finally, the cylinder number.
    ;**********************************************
    mov ax, [bp + 4]
    mov cx, [numSectorsperTrack]

    mov ax, [bp - 16]
    xor dx, dx
    div cx                      ; Divide ax by cx.
                                ; The cylinder number is now stored in ax.
    mov cx, ax                  ; CX := ( ( cylinder and 255 ) shl 8 ) or
                                ;   ( ( cylinder and 768 ) shr 2 ) or sector

    xchg ch, cl
    ;mov ch, cl
    ;mov cl, ah
    shl cl, 6
    
    or cx, [bp - 12]

    ;xor dx,dx
    mov dh, BYTE[bp - 10]

    mov bx, [sectorbuffer]

    mov al, [bp + 6]            ; Read the number of sectors specified.
    mov dl, 0x80
    mov ah, 0x02                ; Set to do read.

    int 0x13                    ; Call function 02h of interrupt 13h (read from disk).
                                ; If you've read my previous commits, I tried using 0x42
                                ; because it seemed simpler but it requires extensions which
                                ; caused problems when testing in Virtualbox. 

    add sp, 8

;    pop dx
    pop cx
    pop bx
    pop ax
    pop bp
    ret

_getDriveParams:
    mov dl, 0x80
    mov ah, 0x08

    int 0x13

    add dh, 1
    mov BYTE[numHeads], dh
    mov BYTE[numSectorsperTrack], cl
    and BYTE[numSectorsperTrack], 63
    mov BYTE[numCylinders], ch
    shr cl, 6
    or BYTE[numCylinders + 1], cl
    add WORD[numCylinders], 1

    ret

letteroffset dw 0x0000
_writeToScreen:
    push bp
    mov bp, sp                  ;   Set up the stack frame.

    mov ax, 0xb800
    mov gs, ax                  ;   Set GS to point to video memory
    ;xor bx, bx                  ;   Set BX to point to the first character
    mov bx, [letteroffset]
    cld                         ;   Clear the direction flag.
    mov si, [bp + 4]            ;   Copy the pointer to the string into si.
                                ;   The reason that this is [+ 4] is because
                                ;   we're in 16-bit mode. 16 bits is two bytes
                                ;   and each register we push to the stack is
                                ;   16 bits, so the stack looks like this:
                                ;
                                ;       |          Text         |
                                ;       |   instruction pointer |
                                ;       |    old base pointer   |   <-- base pointer

startWriteLoop:
    lodsb                       ;   Load the current character in the string to al.
    cmp al, 0x00                ;   Check if we've encountered a null character.
    je endWrite                 ;   If we have, exit the loop.
    mov byte [gs:bx], al        ;   Write the current character in the string.
    inc bx                      ;   Set BX to point to the current attribute byte
    mov byte [gs:bx], 0x0f      ;   Write the attribute byte
    inc bx                      ;   Set BX to point to the second character
    
    cmp bx, 200
    jl startWriteLoop
    mov bx, 0
    jmp startWriteLoop

endWrite:
    mov [letteroffset], bx
    pop bp 
    ret

stage1loaded: db "Stage 1 loaded",0
loadingstage2: db "Loading stage 2",0

times 510-($-$$) db 0
dw 0xAA55

_readCluster:                   ; [es:sectorbuffer] is the destination for the cluster.
    push bp
    mov bp, sp
    push ax
    push bx
    push dx

    xor dx, dx

    mov ax, [rootEntries]
    imul ax, 32
    div WORD[sectorSize]
    xor dx, dx
    ;xor bx, bx
    add ax, [rootStart]

    mov dl, [sectorsPerCluster]
    mov bx, [bp + 4]
    sub bx, 3
    add bx, dx
    add ax, bx

    push dx
    push ax
    call _readSector
    pop ax
    pop dx

    pop dx
    pop bx
    pop ax
    pop bp
    ret

_loadFAT:
    push ax
    push bx
    push cx
    push dx

    mov cx, [sectorsPerFAT]
    ;mov bx, [sectorbufferstart]
    ;mov [FATstart], bx

    mov bx, [reservedSectors]

loadFATloop:
    cmp cx, 0
    je endLoadFAT

    push WORD 0x0001
    push bx
    call _readSector
    pop bx
    pop dx

    inc bx
    mov ax, [sectorbuffer]
    add ax, [sectorSize]
    mov [sectorbuffer], ax
    dec cx
    jmp loadFATloop

endLoadFAT:
    mov bx, [sectorbuffer]
    mov [sectorbufferstart], bx
    pop dx
    pop cx
    pop bx
    pop ax
    ret

_getFSType:
    push ax
    push bx
    push dx

    xor dx, dx
    mov ax, [rootEntries]
    imul ax, 32
    add ax, [sectorSize]
    sub ax, 1
    div WORD [sectorSize]
    mov bx, ax

    mov ax, [totalSectors]
    xor dx, dx
    mov dl, [numberOfFATs]
    imul dx, [sectorsPerFAT]
    add dx, bx
    add dx, [reservedSectors]
    sub ax, dx

    xor dx, dx
    xor bx, bx
    mov bl, [sectorsPerCluster]
    div bx

    cmp ax, 4085
    jge morethan4085
    mov byte [FATtype], 12

    pop dx
    pop bx
    pop ax

    ret

morethan4085:
    mov byte [FATtype], 16

    pop dx
    pop bx
    pop ax
    ret


_loadFile:      ; ax is the starting cluster of the file.
    push bp
    mov bp, sp
    push ax
    push bx
    push dx

    call _getFSType

    call _loadFAT

    mov WORD [sectorbuffer], 0x1000         ; Set the start of memory to be the kernel's stuff.
    mov WORD [sectorbufferstart], 0x1000    ; We'll have the stack before the kernel (giving it
                                            ; 4 KB) because if we had the kernel at the start,
                                            ; it would overwrite the real mode interrupt table.

loadFileLoopStart:
    cmp ax, [maxNumClusters]
    ja endLoadFile

    push ax
    call _readCluster
    
    mov bx, [sectorbuffer]
    add bx, [sectorSize]
    mov [sectorbuffer], bx

    pop bx

    cmp byte [FATtype], 12
    je findFAT12cluster

    imul bx, 0x0002
    add bx, [FATstart]
    jmp clusterfindend

findFAT12cluster:
    mov ax, bx
    shr bx, 1
    add bx, ax
    add bx, [FATstart]

    mov ax, bx
    and ax, 1

    cmp ax, 0
    je clusternumupperpart
    shr bx, 4
    jmp clusterfindend

clusternumupperpart:
    and bx, 0x0FFF

clusterfindend:
    mov ax, [bx]
    jmp short loadFileLoopStart
    
endLoadFile:
    pop dx
    pop bx
    pop ax
    pop bp
    ret


_loadFromFAT:
    ;push bp
    ;mov bp, sp                  ; Set up the stack frame.
    push ax
    push cx
    push bx
    push dx

    xor dx,dx

    mov dl, [numberOfFATs]          ; Find the first sector of the root directory.
    imul dx, [sectorsPerFAT]        ;
    add dx, [reservedSectors]       ;

    mov [rootStart], dx

    mov cx, [rootEntries]

fileSearchLoopStart:
    push WORD 0x0001
    push dx
    call _readSector            ; Call the function to read the specified sector
                                ; from disk.

    pop dx
    pop ax

    xor bx, bx

fileSearchNoChangeSector:
    mov si, [sectorbufferstart]
    add si, bx
    lea di, [filename - 1]

strcmploopstart:
    inc di
    lodsb
    cmp [di], al
    jne strcmpneq
    cmp al, 0


    je fatFileFound
    jmp short strcmploopstart
 
strcmpneq:
    add bx, 32                  ; Increment bx by the size of an entry in the
                                ; root directory

    cmp bx, [sectorSize]

    
    jl fileSearchLoopStart

    inc dx

    cmp cx, 0x0000
    je fatFileNotFound
    dec cx

    jmp fileSearchNoChangeSector

fatFileFound:
    add bx, [sectorbufferstart]
    mov ax, [bx + 26]    ; Bytes 26 and 27 specify the first cluster of
                         ; the file.
    call _loadFile

    ;push filefound
    ;call _writeToScreen
    ;pop bx

    jmp short FATloadend

fatFileNotFound:
    push filenotfound
    call _writeToScreen
    pop bx
    ; Error and shut down.

FATloadend:
    pop dx
    pop bx
    pop cx
    pop ax
    ;pop bp
    ret


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

    call _checka20          ; Check if the a20 line is enabled, giving us access to more than 1MB of memory.
    cmp ax, ax
    jnz skipseta20

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

    skipseta20:

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
GDTR:                               ; GDT register data
    dw  GDT_End - GDT - 1           ; 16-bit size limit of GDT
    dd  GDT                         ; 32-bit linear address of GDT
GDT:
    dd  0x00000000, 0x00000000      ; Null descriptor
    dq  0x00cf9a000000ffff          ; Kernel privilege, code
    dq  0x00cf92000000ffff          ; Kernel privilege, data
    dq  0x00cffa000000ffff          ; User privilege, code
    dq  0x00cff2000000ffff          ; User privilege, data
    ;dd  0x0000FFFF, 0x00CF9C00      ; Code segment
    ;dd  0x0000FFFF, 0x00CF9200      ; Data segment

GDT_End:

;   Useful for debugging. Unfortunately, it needs to go because it increases
;   the file size by 69 bytes.

;numBuffer:  times 10 db 0
;_writeNumber:
;    push bp
;    mov bp, sp
;    mov cx, 10
;    mov ax, [bp + 4]
;    mov bx, 9

;numLoopStart:
;    dec bx
;    xor dx,dx
;    div cx
;    add dl, '0'
;    mov [bx + numBuffer], dl
;    cmp ax, 0
;    jg numLoopStart
;    cmp bx, 0
;    je numLoopEnd
    
;numFillStart:
;    xor dx, dx
;    add dl, '0'
;    dec bx
;    mov [bx + numBuffer], dl
;    cmp bx, 0
;    jne numFillStart

;numLoopEnd:
;    mov dx, numBuffer
;    push dx
;    call _writeToScreen
;    pop dx
;    pop bp
;    ret

_memdetect:
    ; Get information on memory size/availability.
    xor ax, ax
    mov es, ax
    mov ax, 0x9000
    mov di, ax
    xor ebx, ebx

memdetloop:
    mov DWORD[di + 20], 1
    mov ax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 24
    int 0x15
    add di, 24
    test ebx, ebx
    ; If the value returned in ebx is 0, it means that there is no more information
    ; to get on the memory.
    jnz memdetloop

    ret

_memsizedetect:
    xor ecx, ecx

    mov ax, 0xE881
    int 0x15
    jcxz useeax

    mov eax, ecx
    mov ebx, edx

    useeax:
    ret


filename: db "BFYTW   BIN ",0
stage2loaded: db "Stage 2 loaded.",0
loadedfromFAT: db "Loaded from FAT.",0
filenotfound: db "File not found.",0

times 1536-($-$$) db 0
