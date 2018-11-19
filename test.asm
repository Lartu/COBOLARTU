global _start
%include "coblib.asm"
;VALUES SECTION
section .data
__str1: db "Hola, soy [1;33mlartu[0m. Tengo [1;31m", 0
__str2: db "[0m años.", 10, "", 0
__str3: db "Voy a [1;36mmultiplicar[0m algo...", 10, "", 0
__str4: db "50 x 50 da... [1;34m", 0
__str5: db "[0m", 10, "", 0
__str6: db "12345 x 123456 da... [1;34m", 0
__str7: db "[0m", 10, "", 0
__str8: db "Voy a [1;35mDIVIDIR[0m algo...", 10, "", 0
__str9: db "984621 / 123 da... [1;34m", 0
__str10: db "[0m", 10, "", 0
__str11: db "416 / 419 da... [1;34m", 0
__str12: db "[0m y sobra ", 0
__str13: db "", 10, "", 0
__str14: db "7 módulo 2 da... [1;34m", 0
__str15: db "[0m", 10, "", 0
digitSpace: times 100 db 0
digitSpacePos times 8 db 0
;DATA SECTION
__varNOMBRE: times 50 db 0
__varNOMBRE_len: dq 50
__varOTRONOMBRE: times 50 db 0
__varOTRONOMBRE_len: dq 50
__varEDAD: dq 0
__varNUM: dq 0
__varREM: dq 0
__varDECIMAL: dq 0
;PROCEDURE SECTION
section .text
_start:
mov rax, __varNOMBRE
mov rbx, __str1
mov rdx, [__varNOMBRE_len]
call copystring
mov rax, __varOTRONOMBRE
mov rbx, __varNOMBRE
mov rdx, [__varOTRONOMBRE_len]
call copystring
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __varOTRONOMBRE
call strlen
syscall
mov qword [__varEDAD], 13
mov rax, 50
add rax, [__varEDAD]
mov [__varEDAD], rax
mov rax, [__varEDAD]
sub rax, 40
mov [__varEDAD], rax
mov rax, [__varEDAD]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str2
call strlen
syscall
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str3
call strlen
syscall
mov rax, 50
imul rax, 50
mov [__varNUM], rax
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str4
call strlen
syscall
mov rax, [__varNUM]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str5
call strlen
syscall
mov rax, 12345
imul rax, 123456
mov [__varNUM], rax
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str6
call strlen
syscall
mov rax, [__varNUM]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str7
call strlen
syscall
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str8
call strlen
syscall
mov rdx, 0
mov rax, 984621
mov rbx, 123
idiv rbx
mov [__varNUM], rax
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str9
call strlen
syscall
mov rax, [__varNUM]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str10
call strlen
syscall
mov rdx, 0
mov rax, 416
mov rbx, 419
idiv rbx
mov [__varDECIMAL], rax
mov [__varREM], rdx
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str11
call strlen
syscall
mov rax, [__varDECIMAL]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str12
call strlen
syscall
mov rax, [__varREM]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str13
call strlen
syscall
mov rdx, 0
mov rax, 7
mov rbx, 2
idiv rbx
mov [__varNUM], rdx
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str14
call strlen
syscall
mov rax, [__varNUM]
call printnumber
mov rax, 1 ; write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, __str15
call strlen
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

