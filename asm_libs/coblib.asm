;AUXILIAR SECTION

;Printnumber: imprime un número pasado por rax
number_to_string:
    push 0                      ;Pusheo un 0 al stack para marcar el fin
    mov rcx, digit_area         ;Pongo digitar area en rcx para guardar num
    cmp rax, 0                  ;Veo si el número es menor a 0
    jl .negativo                ;Si es negativo actúo de forma acorde
.loop:   
    mov rdx, 0                  ;Pongo rdx en 0 para dividir.
    mov rbx, 10                 ;Voy a usar 10 para dividir
    div rbx                     ;Divido rax por 10. Me queda en rdx el resto
    add rdx, 48                 ;Le sumo '0' al resto
    push rdx                    ;Pusheo a la pila la letra
    cmp rax, 0                  ;Veo si rax es mayor a cero...
    jne .loop                    ;...si todavía no es, loopeo.
.popear_todo:
    pop rax
    mov byte [rcx], al
    inc rcx
    cmp al, 0
    je .listo
    jmp .popear_todo
.listo:
    ret
.negativo:
    mov byte [rcx], '-'
    inc rcx
    imul rax, -1
    jmp .loop

; printnumber: ;Imprime el número pasado por rax
; ;//TODO rehacer esto de forma que acepte negativos
;     call printnumberaux
;     ret
; printnumberaux:
;     mov rcx, digitSpace
;     mov rbx, 0
;     mov [rcx], rbx
;     inc rcx
;     mov [digitSpacePos], rcx
; .loop:
;     mov rdx, 0
;     mov rbx, 10
;     div rbx
;     push rax
;     add rdx, 48
;     mov rcx, [digitSpacePos]
;     mov [rcx], dl
;     inc rcx
;     mov [digitSpacePos], rcx
;     pop rax
;     cmp rax, 0
;     jne .loop
; .loop2:
;     mov rcx, [digitSpacePos]
;     mov rax, 1
;     mov rdi, 1
;     mov rsi, rcx
;     mov rdx, 1
;     syscall
;     mov rcx, [digitSpacePos]
;     dec rcx
;     mov [digitSpacePos], rcx
;     cmp rcx, digitSpace
;     jge .loop2
;     ret
    
copystring: ;Copia de rbx a rax un string. Toma largo de rax por rdx.
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
    ret

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

sleep: ;toma el tiempo a dormir en milisegundos por rax
    ;Paso de milisegundos a segundos y nanosegundos
    mov rdx, 0
    mov rbx, 1000
    div rbx
    mov rcx, rax ;Tengo en rcx la cantidad de segundos
    imul rdx, 1000000 ;Paso el resto a nanosegundos
    
    mov dword [tv_sec], ecx ;Segundos
    mov dword [tv_usec], edx ;Nanosegundos
    mov eax, 162
    mov ebx, timeval
    mov ecx, 0
    int 0x80
    ret
    
userinput:
    mov rax, 3
    mov rbx, 0
    mov rcx, input_buffer
    mov rdx, input_buffer_size
    inc rdx
    int 80h
    mov rcx, input_buffer
    add rcx, rax
    dec rcx
    mov byte [rcx], 0
    ret
