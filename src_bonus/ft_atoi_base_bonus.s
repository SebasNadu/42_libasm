; macOS / Linux symbol prefix compatibility
%ifidn __OUTPUT_FORMAT__, macho64
    %define ft_atoi_base _ft_atoi_base
%endif

global ft_atoi_base

section .text

ft_atoi_base:
    ; Guard: check if str (rdi) or base (rsi) is NULL
    test rdi, rdi
    jz .error
    test rsi, rsi
    jz .error

    ; 1. Validate base and get radix length
    call .validate_base
    test rax, rax           ; If radix is 0, base is invalid!
    jz .error

    mov rdx, rax            ; rdx = radix (base length)

    ; 2. Skip leading whitespaces in str (rdi)
.skip_whitespace:
    mov cl, BYTE [rdi]
    test cl, cl
    jz .error               ; If empty string, return 0

    cmp cl, ' '
    je .next_whitespace
    cmp cl, 9               ; '\t'
    je .next_whitespace
    cmp cl, 10              ; '\n'
    je .next_whitespace
    cmp cl, 11              ; '\v'
    je .next_whitespace
    cmp cl, 12              ; '\f'
    je .next_whitespace
    cmp cl, 13              ; '\r'
    je .next_whitespace
    jmp .parse_sign

.next_whitespace:
    inc rdi                 ; str++
    jmp .skip_whitespace

    ; 3. Parse consecutive signs (+ or -)
.parse_sign:
    mov r8, 1               ; r8 = sign multiplier = 1

.sign_loop:
    mov cl, BYTE [rdi]
    cmp cl, '+'
    je .sign_plus
    cmp cl, '-'
    je .sign_minus
    jmp .parse_digits

.sign_plus:
    inc rdi                 ; str++
    jmp .sign_loop

.sign_minus:
    neg r8                  ; Toggle sign multiplier (sign = -sign)
    inc rdi                 ; str++
    jmp .sign_loop

    ; 4. Accumulate numerical digits
.parse_digits:
    mov r9, 0               ; r9 = result = 0

.digit_loop:
    mov al, BYTE [rdi]      ; al = current character *str
    test al, al             ; Is *str '\0'?
    jz .finish

    ; Save loop variables on stack before calling subroutine
    push rdi
    push rdx
    push r8
    push r9

    ; Call .get_char_index (uses al and rsi)
    call .get_char_index    ; rax = digit value (or -1)

    ; Restore loop variables
    pop r9                  ; r9 = result
    pop r8                  ; r8 = sign
    pop rdx                 ; rdx = radix
    pop rdi                 ; rdi = str

    cmp rax, -1             ; Did we find the digit in base?
    je .finish              ; If no, stop parsing

    ; result = result * radix + digit_value
    imul r9, rdx            ; r9 = result * radix
    add r9, rax             ; r9 = result + digit_value

    inc rdi                 ; str++
    jmp .digit_loop

.finish:
    ; Return result * sign multiplier
    imul r9, r8             ; r9 = result * sign
    mov rax, r9             ; Return final value in rax
    ret

.error:
    mov rax, 0              ; Return 0 on error
    ret


; ==============================================================================
; SUBROUTINE: .validate_base (rsi = base)
; Returns radix in rax, or 0 if base is invalid.
; ==============================================================================
.validate_base:
    ; Base must have at least 2 chars
    test rsi, rsi
    jz .invalid_base
    cmp BYTE [rsi], 0
    je .invalid_base
    cmp BYTE [rsi + 1], 0
    je .invalid_base

    mov rax, 0              ; rax = index i = 0

.outer_val_loop:
    mov cl, BYTE [rsi + rax] ; cl = base[i]
    test cl, cl             ; Is base[i] '\0'?
    jz .val_success         ; If yes, base is valid!

    ; Check invalid characters (whitespace, +, -)
    cmp cl, ' '
    je .invalid_base
    cmp cl, '+'
    je .invalid_base
    cmp cl, '-'
    je .invalid_base
    cmp cl, 9               ; '\t'
    je .invalid_base
    cmp cl, 10              ; '\n'
    je .invalid_base
    cmp cl, 11              ; '\v'
    je .invalid_base
    cmp cl, 12              ; '\f'
    je .invalid_base
    cmp cl, 13              ; '\r'
    je .invalid_base

    ; Check for duplicate character (base[j] == base[i])
    mov r8, rax             ; r8 = j = i
    inc r8                  ; r8 = j = i + 1

.inner_val_loop:
    mov dl, BYTE [rsi + r8]  ; dl = base[j]
    test dl, dl
    jz .inner_val_done

    cmp dl, cl               ; Does base[j] == base[i]?
    je .invalid_base         ; If yes, duplicate found!

    inc r8                  ; j++
    jmp .inner_val_loop

.inner_val_done:
    inc rax                 ; i++
    jmp .outer_val_loop

.val_success:
    ret                     ; Returns radix (base length) in rax

.invalid_base:
    mov rax, 0              ; Returns 0
    ret


; ==============================================================================
; SUBROUTINE: .get_char_index (al = char c, rsi = base)
; Returns index in rax, or -1 if char not found.
; ==============================================================================
.get_char_index:
    mov r10, 0              ; r10 = index = 0

.index_loop:
    mov cl, BYTE [rsi + r10] ; cl = base[index]
    test cl, cl
    jz .index_not_found

    cmp cl, al              ; Does base[index] == c?
    je .index_found

    inc r10                 ; index++
    jmp .index_loop

.index_not_found:
    mov rax, -1
    ret

.index_found:
    mov rax, r10            ; Return index in rax
    ret
