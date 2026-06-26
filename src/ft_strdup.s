; macOS / Linux symbol prefix compatibility
%ifidn __OUTPUT_FORMAT__, macho64
    %define ft_strdup _ft_strdup
    %define ft_strlen _ft_strlen
    %define malloc _malloc
    %define ft_strcpy _ft_strcpy
%endif

global ft_strdup
extern ft_strlen
extern malloc
extern ft_strcpy

section .text

ft_strdup:
    push rdi                ; Save rdi (src pointer) and align stack to 16 bytes

    call ft_strlen          ; Get string length in rax
    inc rax                 ; Add 1 for null terminator

    mov rdi, rax            ; First argument for malloc: size
    call malloc             ; Call malloc, returns pointer in rax

    test rax, rax           ; Check if malloc returned NULL
    jz .malloc_error        ; If NULL, jump to error handler

    pop rsi                 ; Restore original src pointer into rsi (second arg of strcpy)
    mov rdi, rax            ; Move allocated pointer to rdi (first arg of strcpy)
    call ft_strcpy          ; Copy string, returns allocated pointer in rax
    ret                     ; Return

.malloc_error:
    pop rdi                 ; Clean up stack
    mov rax, 0              ; Return NULL (malloc sets errno to ENOMEM automatically)
    ret
