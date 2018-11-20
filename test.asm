global _start
;VALUES SECTION
section .data
__str1: db "Let's print the first ", 0
__str2: db " fibonacci number(s) after 0 and 1", 10, "", 0
__str3: db "", 10, "", 0
__str4: db "[1;32mDone![0m", 10, "", 0
digitSpace: times 100 db 0
digitSpacePos times 8 db 0
;DATA SECTION
__varN: dq 0
__varI: dq 0
__varJ: dq 0
__varSWAP: dq 0
;PROCEDURE SECTION
section .text
_start:
mov qword [__varN], 10
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str1
call strlen
syscall
mov rax, [__varN]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str2
call strlen
syscall
mov qword [__varI], 1
mov rax, [__varI]
mov qword [__varJ], rax
_while1:
mov rax, [__varN]
mov rbx, 0
cmp rax, rbx
jle _endWhile1
mov rax, [__varJ]
mov qword [__varSWAP], rax
mov rax, [__varI]
add rax, [__varSWAP]
mov [__varSWAP], rax
mov rax, [__varJ]
mov qword [__varI], rax
mov rax, [__varSWAP]
mov qword [__varJ], rax
mov rax, [__varJ]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str3
call strlen
syscall
mov rax, [__varN]
sub rax, 1
mov [__varN], rax
jmp _while1
_endWhile1:
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str4
call strlen
syscall
mov rax, 60 ;Exit
mov rdi, 0 ;Exit code (0)
syscall
%include "coblib.asm"

