/*Generated from libvalues.asm*/
add_asm_values("%define input_buffer_size 512 ;used for accept");
add_asm_values("");
add_asm_values("digit_area: times 100 db 0 ;used to convert int to strings");
add_asm_values("timeval: ;Used for sleep");
add_asm_values("tv_sec  dd 0");
add_asm_values("tv_usec dd 0");
add_asm_values("input_buffer: times input_buffer_size db 0 ;used for accept");
add_asm_values("crlf: db 10, 13, 0 ;Used to print crlf");
add_asm_values("");