; macOS / Linux symbol prefix compatibility
%ifidn __OUTPUT_FORMAT__, macho64
    %define ft_strlen _ft_strlen
%endif

global ft_strlen

section .text

ft_strlen:
    mov rax, 0             ; Initialize counter/index to 0

.loop:
    cmp BYTE [rdi + rax], 0 ; Compare current character str[rax] to '\0'
    je .done               ; If equal (null-terminator), jump to done
    inc rax                ; Increment counter (rax++)
    jmp .loop              ; Jump back to the start of the loop

.done:
    ret                    ; Return with length in rax
