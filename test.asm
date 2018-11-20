global _start
%include "coblib.asm"
;VALUES SECTION
section .data
__str1: db "Vamos a hacer un [1;32mDisan Count[0m desde [1;34m", 0
__str2: db "[0m hasta [1;31m0[0m!", 10, "", 0
__str3: db " 👉 ", 0
__str4: db " es [1;33mpar[0m!", 10, "", 0
__str5: db "Listo! 😁", 10, "", 0
digitSpace: times 100 db 0
digitSpacePos times 8 db 0
;DATA SECTION
__varI: dq 0
__varRES: dq 0
;PROCEDURE SECTION
section .text
_start:
mov qword [__varI], 10
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str1
call strlen
syscall
mov rax, [__varI]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str2
call strlen
syscall
_while1:
mov rax, [__varI]
mov rbx, 0
cmp rax, rbx
jle _endWhile1
mov rdx, 0
mov rax, [__varI]
mov rbx, 2
idiv rbx
mov [__varRES], rdx
_if1:
mov rax, [__varRES]
mov rbx, 0
cmp rax, rbx
jne _elseIf1
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str3
call strlen
syscall
mov rax, [__varI]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str4
call strlen
syscall
_elseIf1:
_endIf1:
mov rax, [__varI]
sub rax, 1
mov [__varI], rax
jmp _while1
_endWhile1:
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str5
call strlen
syscall
mov rax, 60 ;Exit
mov rdi, 0 ;Exit code (0)
syscall
mov rax, 60 ;Exit
mov rdi, 0 ;Exit code (0)
syscall
;AUXILIAR SECTION
strlen: ;String length counter (string = rsi, length = rdx)
push rsi
mov rdx, 0
.loop:
cmp byte [rsi], 0
je .ret
inc rdx
inc rsi
jmp .loop
.ret:
pop rsi
ret

