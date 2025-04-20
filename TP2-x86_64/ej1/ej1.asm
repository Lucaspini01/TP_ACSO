%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_node_create_asm
global string_proc_list_create_asm
global string_proc_list_concat_asm
global string_proc_list_add_node_asm

extern malloc
extern free
extern strlen
extern strcpy
extern strcat
extern str_concat

string_proc_list_create_asm:
    push rbp
    mov rbp, rsp
    mov rdi, 16
    call malloc
    mov QWORD [rax], 0
    mov QWORD [rax + 8], 0
    mov rsp, rbp
    pop rbp
    ret

string_proc_node_create_asm:
    push rdi
    push rsi
    mov rdi, 32
    call malloc
    pop rsi
    pop rdi
    test rax, rax
    je .malloc_failed
    mov QWORD [rax + 0], 0
    mov QWORD [rax + 8], 0
    mov BYTE  [rax + 16], dil
    mov QWORD [rax + 24], rsi
    ret
.malloc_failed:
    mov rax, 0
    ret

string_proc_list_add_node_asm:
    push rdi
    mov rdi, rsi
    mov rsi, rdx
    call string_proc_node_create_asm
    pop rdi
    test rax, rax
    je .return

    mov r12, rax
    mov r13, [rdi]
    test r13, r13
    jz .empty_list

    mov r14, [rdi + 8]
    mov [r12 + 8], r14
    mov [r14], r12
    mov [rdi + 8], r12
    jmp .return

.empty_list:
    mov [rdi], r12
    mov [rdi + 8], r12

.return:
    ret

string_proc_list_concat_asm:
    mov r12, rdx
    mov r13, [rdi]
    mov r14, rsi
    mov r15, rdx
.loop:
    test r13, r13
    je .done
    movzx rax, byte [r13 + 16]
    cmp rax, r14
    jne .next
    mov rdi, r12
    mov rsi, [r13 + 24]
    push r12
    push r13
    push r14
    push r15
    call str_concat
    pop r15
    pop r14
    pop r13
    pop r12
    cmp r12, r15
    je .skip_free
    mov rdi, r12
    push rax
    call free
    pop rax
.skip_free:
    mov r12, rax
.next:
    mov r13, [r13]
    jmp .loop
.done:
    mov rax, r12
    ret
