; macOS / Linux symbol prefix compatibility
%ifidn __OUTPUT_FORMAT__, macho64
    %define ft_list_remove_if _ft_list_remove_if
    %define free _free
%endif

global ft_list_remove_if
extern free

section .text

ft_list_remove_if:
    ; Guards: check if begin_list is NULL
    test rdi, rdi
    jz .done

    ; Check if *begin_list is NULL
    mov r8, [rdi]
    test r8, r8
    jz .done

    ; Check if cmp is NULL
    test rdx, rdx
    jz .done

    ; Push 6 callee-saved registers and adjust stack pointer for 16-byte alignment
    push r12
    push r13
    push r14
    push r15
    push rbx
    push rbp
    sub rsp, 8              ; RSP ends in 0 (Perfect 16-byte alignment!)

    ; Initialize loop registers
    mov r12, rdi            ; r12 = begin_list
    mov r13, rsi            ; r13 = data_ref
    mov r14, rdx            ; r14 = cmp
    mov r15, rcx            ; r15 = free_fct
    mov rbx, r8             ; rbx = curr = *begin_list
    mov rbp, 0              ; rbp = prev = NULL

.loop:
    test rbx, rbx           ; Is curr (rbx) NULL?
    jz .restore_and_done    ; If yes, we are finished!

    ; Call cmp(curr->data, data_ref)
    mov rdi, [rbx]          ; rdi = curr->data (offset 0)
    mov rsi, r13            ; rsi = data_ref
    call r14                ; Call cmp callback

    test eax, eax           ; If result != 0, no match
    jnz .no_match

    ; --- Match found: remove curr (rbx) ---
    mov r8, [rbx + 8]       ; r8 = curr->next (offset 8)

    ; Relink pointers
    test rbp, rbp           ; If prev (rbp) == NULL, we are deleting the head
    jnz .prev_not_null

    mov [r12], r8           ; *begin_list = curr->next
    jmp .relink_done

.prev_not_null:
    mov [rbp + 8], r8       ; prev->next = curr->next

.relink_done:
    ; We must save to_remove (rbx) and curr->next (r8) on the stack
    ; to keep them safe across free_fct and free calls.
    ; Pushing 2 registers (16 bytes total) maintains our 16-byte alignment!
    push rbx                ; Push to_remove (stored at [rsp + 8])
    push r8                 ; Push curr->next (stored at [rsp])

    ; Call free_fct(to_remove->data) if free_fct is not NULL
    test r15, r15
    jz .skip_free_data

    mov rdi, [rbx]          ; rdi = to_remove->data
    call r15                ; Call free_fct callback

.skip_free_data:
    ; Call free(to_remove)
    mov rdi, [rsp + 8]      ; Retrieve saved to_remove pointer from the stack
    call free               ; Free the node struct itself

    ; Restore stack and set curr to curr->next
    pop rbx                 ; rbx = curr = curr->next
    pop r8                  ; Pop dummy register to restore stack level
    jmp .loop               ; Check next node with the same prev

.no_match:
    mov rbp, rbx            ; prev = curr
    mov rbx, [rbx + 8]      ; curr = curr->next
    jmp .loop

.restore_and_done:
    ; Restore stack and callee-saved registers
    add rsp, 8
    pop rbp
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12

.done:
    ret
