; macOS / Linux symbol prefix compatibility
%ifidn __OUTPUT_FORMAT__, macho64
    %define ft_list_sort _ft_list_sort
%endif

global ft_list_sort

section .text

ft_list_sort:
    ; Guards: check if begin_list is NULL
    test rdi, rdi
    jz .done

    ; Check if *begin_list is NULL
    mov r8, [rdi]
    test r8, r8
    jz .done

    ; Check if cmp is NULL
    test rsi, rsi
    jz .done

    ; Push 5 registers to save them and align stack to 16 bytes
    push r12
    push r13
    push r14
    push r15
    push rbx                ; Dummy push to achieve 16-byte alignment

    ; Initialize loop variables
    mov r12, r8             ; r12 = head pointer (*begin_list)
    mov r13, rsi            ; r13 = cmp function pointer
    mov r14, r12            ; r14 = outer loop pointer i = head

.outer_loop:
    test r14, r14           ; Is i == NULL?
    jz .restore_and_done    ; If yes, sorting is complete!

    mov r15, [r14 + 8]      ; r15 = inner loop pointer j = i->next

.inner_loop:
    test r15, r15           ; Is j == NULL?
    jz .next_outer          ; If yes, move to next outer iteration

    ; Call cmp(i->data, j->data)
    ; cmp expects first arg in rdi, second arg in rsi
    mov rdi, [r14]          ; rdi = i->data
    mov rsi, [r15]          ; rsi = j->data
    call r13                ; Call cmp

    ; cmp returns result in eax. If result <= 0, no swap is needed
    cmp eax, 0
    jle .next_inner

    ; Swap the data pointers (offset 0) between i and j
    mov r8, [r14]           ; r8 = i->data
    mov r9, [r15]           ; r9 = j->data
    mov [r14], r9           ; i->data = r9
    mov [r15], r8           ; j->data = r8

.next_inner:
    mov r15, [r15 + 8]      ; j = j->next
    jmp .inner_loop

.next_outer:
    mov r14, [r14 + 8]      ; i = i->next
    jmp .outer_loop

.restore_and_done:
    ; Restore callee-saved registers in reverse order
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12

.done:
    ret
