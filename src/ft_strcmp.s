; macOS / Linux symbol prefix compatibility
%ifidn __OUTPUT_FORMAT__, macho64
    %define ft_strcmp _ft_strcmp
%endif

global ft_strcmp

section .text

ft_strcmp:
    ; No initialization needed. rdi = s1, rsi = s2

.loop:
    mov al, BYTE [rdi]      ; Load character from s1 (*rdi) into al
    mov dl, BYTE [rsi]      ; Load character from s2 (*rsi) into dl
    cmp al, dl              ; Compare the two characters
    jne .diff               ; If they are not equal, jump to calculate difference
    cmp al, 0               ; Are we at the end of the strings? (null-terminator)
    je .diff                ; If yes (both are '\0'), difference is 0, jump to exit
    inc rdi                 ; Increment s1 pointer to next character (s1++)
    inc rsi                 ; Increment s2 pointer to next character (s2++)
    jmp .loop               ; Repeat

.diff:
    movzx eax, al           ; Zero-extend 8-bit al to 32-bit eax
    movzx edx, dl           ; Zero-extend 8-bit dl to 32-bit edx
    sub eax, edx            ; eax = eax - edx (returns the difference)
    ret                     ; Return result in eax
