; macOS / Linux symbol prefix compatibility
%ifidn __OUTPUT_FORMAT__, macho64
    %define ft_read _ft_read
    %define errno_func ___error
    %define SYS_READ 0x2000003
%else
    %define errno_func __errno_location
    %define SYS_READ 0
%endif

global ft_read
extern errno_func

section .text

ft_read:
    mov rax, SYS_READ       ; Load read system call number
    syscall                 ; Invoke kernel

%ifidn __OUTPUT_FORMAT__, macho64
    jc .mac_error           ; macOS uses the Carry Flag for errors
    ret                     ; Success: return bytes read in rax

.mac_error:
    push rax                ; Save positive error code to stack (aligns stack to 16 bytes!)
    call errno_func         ; Get pointer to errno (returned in rax)
    pop rdi                 ; Restore error code from stack into rdi
    mov [rax], edi          ; Store the error code (32-bit) in errno (*rax = edi)
    mov rax, -1             ; Set return value to -1
    ret
%else
    cmp rax, 0              ; Linux returns a negative value on error
    jl .linux_error
    ret                     ; Success: return bytes read in rax

.linux_error:
    neg rax                 ; Convert negative error code to positive (e.g. -9 -> 9)
    push rax                ; Save positive error code to stack (aligns stack to 16 bytes!)
    call errno_func         ; Get pointer to errno (returned in rax)
    pop rdi                 ; Restore error code from stack into rdi
    mov [rax], edi          ; Store the error code (32-bit) in errno (*rax = edi)
    mov rax, -1             ; Set return value to -1
    ret
%endif
