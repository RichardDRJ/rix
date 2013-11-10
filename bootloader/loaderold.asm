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
reservedSectors:        dw 0x0001
numberOfFATs:           db 0x02
rootEntries:            dw 0x0040
totalSectors:           dw 0x0FFF
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
fileSystem:             db "FAT16   " 

maxNumClusters:             dw 0x780
rootStart:              dw 0x0000

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

sectorbufferstart       dw 0x1000
sectorbuffer            dw 0x1000   ; A point in memory to which we will load the system from disk.
                                    ; Since we're in real mode the bootloader has no access
                                    ; to memory past 1MB, so here we cannot pass 0xFFFF.

FATstart                dw 0x1000


;**********************************************
;   The following is for a FAT32 disk
;   (65525 sectors or above).
;**********************************************

;SectorsPerHead          dw 0x0002
;HiddenSectors           dd 0x00000000
;TotalSectors            dd 0x00010020 
;BigSectorsPerFAT        dd 0x0000000F
;Flags                   dw 0x0000
;FSVersion               dw 0x0000
;RootDirectoryStart      dd 0x00000002
;FSInfoSector            dw 0x0001
;BackupBootSector        dw 0x0006

;TIMES 12                db 0            ; 12 reserved bytes set to 0.

;DriveNumber             db 0x00
;Signature               db 0x29
;VolumeID                dd 0xFFFFFFFF
;VolumeLabel             db "RDRJAWESOME"
;SystemID                db "FAT32   "


;********************************************** 



start:
    cli
    mov ax,cs               ; Setup segment registers
    mov ds,ax               ; Make DS correct
    mov es,ax               ; Make ES correct
    mov ss,ax               ; Make SS correct
    mov bp,7c00h
    mov sp,7c00h            ; Setup a stack

    sti

    call _getDriveParams

    call _loadFromFAT

    mov ax, [sectorbufferstart]
    shr ax, 4
    push ax
    push WORD 0
    retf

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
    mov ax, [sectorbuffer]
    mov [sectorbufferstart], ax

    pop dx
    pop cx
    pop bx
    pop ax
    ret

_loadFile:      ; ax is the starting cluster of the file.
    ;push bp
    ;mov bp, sp
    ;push ax    ; We don't need to push these registers
    ;push bx    ; because they're not used outside the function
                ; in such a way that our using them here would
                ; disrupt the working of the program.
                ; As such, I've elected not to push them because
                ; I need space!

    call _loadFAT

loadFileLoopStart:
    cmp ax, [maxNumClusters]
    ja endLoadFile

    push ax
    call _readCluster
    
    mov bx, [sectorbuffer]
    add bx, [sectorSize]
    mov [sectorbuffer], bx

    pop bx

    sub bx, 1
    imul bx, 0x0002
    add bx, [FATstart]
    mov ax, [bx]

    jmp short loadFileLoopStart
    
endLoadFile:
    ;pop bx
    ;pop ax
    ;pop bp
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
    ;push filenotfound
    ;call _writeToScreen
    ;pop bx
    ; Error and shut down.

FATloadend:
    pop dx
    pop bx
    pop cx
    pop ax
    ;pop bp
    ret

;_strcmp:
;    mov si, WORD[bp + 4]
;    mov di, WORD[bp + 6]
;    dec di
;
;strcmploopstart:
;    inc di
;    lodsb
;    cmp [di], al
;    jne strcmpneq
;    cmp al, 0
;    je strcmpeq
;
;strcmpneq:
;    mov ax, 1
;    ret
;strcmpeq:
;    mov ax, 0
;    ret

;_strcmp:            ; Compares two strings. Result is in ax.
;    push bp
;    mov bp, sp
;    push si
;
;strcmploopstart:
;    mov si, WORD[bp + 4]
;    mov al, [si]
;    mov si, WORD[bp + 6]
;    mov ah, [si]
;
;    cmp al, ah
;    jne strcmpneq
;
;    test al, 0
;    jz strcmpeq
;
;    inc WORD[bp + 4]
;    inc WORD[bp + 6]
;
;strcmpneq:
;    mov ax, 1
;    jmp strcmpend
;
;strcmpeq:
;    mov ax, 0
;
;strcmpend:
;    pop si
;    pop bp
;    ret

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

_readSector:                    ; Read sector specified in a variable pushed to the stack
                                ; from disk.

    mov dl, 0x80                ; Choose the drive.
    mov ah, 0x00                ; We want to reset the drive so we're
                                ; at the first sector.
    int 0x13                    ; Execute function 0 of interrupt 0x13.

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

;letteroffset dw 0x0000
;_writeToScreen:
;    push bp
;    mov bp, sp                  ;   Set up the stack frame.
;
;    mov ax, 0xb800
;    mov gs, ax                  ;   Set GS to point to video memory
;    ;xor bx, bx                  ;   Set BX to point to the first character
;    mov bx, [letteroffset]
;    cld                         ;   Clear the direction flag.
;    mov si, [bp + 4]            ;   Copy the pointer to the string into si.
;                                ;   The reason that this is [+ 4] is because
;                                ;   we're in 16-bit mode. 16 bits is two bytes
;                                ;   and each register we push to the stack is
;                                ;   16 bits, so the stack looks like this:
;                                ;
;                                ;       |          Text         |
;                                ;       |   instruction pointer |
;                                ;       |    old base pointer   |   <-- base pointer
;
;startWriteLoop:
;    lodsb                       ;   Load the current character in the string to al.
;    cmp al, 0x00                ;   Check if we've encountered a null character.
;    je endWrite                 ;   If we have, exit the loop.
;    mov byte [gs:bx], al        ;   Write the current character in the string.
;    inc bx                      ;   Set BX to point to the current attribute byte
;    mov byte [gs:bx], 0x0f      ;   Write the attribute byte
;    inc bx                      ;   Set BX to point to the second character
;    
;    cmp bx, 200
;    jl startWriteLoop
;    mov bx, 0
;    jmp startWriteLoop
;
;endWrite:
;    mov [letteroffset], bx
;    pop bp 
;    ret

filename: db "BFYTW   BIN ",0

times 510-($-$$) db 0
dw 0xAA55
