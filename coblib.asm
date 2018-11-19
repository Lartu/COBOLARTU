printnumber: ;Imprime el número pasado por rax
;//TODO rehacer esto de forma que acepte negativos
    call printnumberaux
    ret
printnumberaux:
    mov rcx, digitSpace
    mov rbx, 0
    mov [rcx], rbx
    inc rcx
    mov [digitSpacePos], rcx
.loop:
    mov rdx, 0
    mov rbx, 10
    div rbx
    push rax
    add rdx, 48
    mov rcx, [digitSpacePos]
    mov [rcx], dl
    inc rcx
    mov [digitSpacePos], rcx
    pop rax
    cmp rax, 0
    jne .loop
.loop2:
    mov rcx, [digitSpacePos]
    mov rax, 1
    mov rdi, 1
    mov rsi, rcx
    mov rdx, 1
    syscall
    mov rcx, [digitSpacePos]
    dec rcx
    mov [digitSpacePos], rcx
    cmp rcx, digitSpace
    jge .loop2
    ret
    
copystring: ;Copia de rbx a rax un string. Toma largo de rax por rdx.
    push rcx
    .loop:
    cmp rdx, 0
    je .ret
    cmp byte [rbx], 0
    je .ret
    mov cl, [rbx]
    mov [rax], cl
    inc rbx
    inc rax
    dec rdx
    jmp .loop
    .ret:
    pop rcx
    ret
