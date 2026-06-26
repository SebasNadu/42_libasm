; macOS / Linux symbol prefix compatibility
%ifidn __OUTPUT_FORMAT__, macho64
    %define ft_list_push_front _ft_list_push_front
    %define malloc _malloc
%endif

global ft_list_push_front
extern malloc

section .text

ft_list_push_front:
    ; Guard: check if begin_list (rdi) is NULL
    test rdi, rdi
    jz .done

    ; Save parameters and align stack to 16 bytes
    push rdi                ; rsp - 8 (rsp ends in 0)
    push rsi                ; rsp - 8 (rsp ends in 8)
    sub rsp, 8              ; rsp - 8 (rsp ends in 0 — Aligned!)

    ; Call malloc to allocate a new node of size 16 (sizeof(t_list))
    mov rdi, 16
    call malloc             ; rax now holds pointer to new node

    ; Restore parameters and stack
    add rsp, 8
    pop rsi                 ; rsi = original data pointer
    pop rdi                 ; rdi = original begin_list pointer

    ; Check if malloc failed (NULL)
    test rax, rax
    jz .done

    ; Fill the new node and update list head
    mov [rax], rsi          ; new_node->data = data (offset 0)
    mov rdx, [rdi]          ; rdx = *begin_list (current head node)
    mov [rax + 8], rdx      ; new_node->next = *begin_list (offset 8)
    mov [rdi], rax          ; *begin_list = new_node

.done:
    ret
