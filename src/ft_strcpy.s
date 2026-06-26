; macOS / Linux symbol prefix compatibility
%ifidn __OUTPUT_FORMAT__, macho64
    %define ft_strcpy _ft_strcpy
%endif

global ft_strcpy

section .text

ft_strcpy:
    mov rax, rdi            ; Save dest pointer (rdi) to rax (our return value)
    mov rcx, 0              ; Initialize index i = 0

.loop:
    mov dl, BYTE [rsi + rcx]    ; Load src[i] into 8-bit register dl
    mov BYTE [rdi + rcx], dl    ; Copy dl into dest[i]
    cmp dl, 0                   ; Was the copied character the null terminator?
    je .done                    ; If yes, jump to .done
    inc rcx                     ; i++
    jmp .loop                   ; Repeat loop

.done:
    ret                         ; Return rax (which points to dest)
