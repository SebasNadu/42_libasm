; macOS / Linux symbol prefix compatibility
%ifidn __OUTPUT_FORMAT__, macho64
    %define ft_list_size _ft_list_size
%endif

global ft_list_size

section .text

ft_list_size:
    mov rax, 0              ; Initialize total size counter to 0

.loop:
    test rdi, rdi           ; Is the current node pointer (rdi) NULL?
    jz .done                ; If yes (end of list), jump to done

    inc rax                 ; Increment count (rax++)
    mov rdi, [rdi + 8]      ; Load next node pointer (rdi = rdi->next)
    jmp .loop               ; Repeat loop

.done:
    ret                     ; Return count in eax (lower 32-bit of rax)
