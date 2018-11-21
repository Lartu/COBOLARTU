%define input_buffer_size 512 ;used for accept

digit_area: times 100 db 0 ;used to convert int to strings
timeval: ;Used for sleep
    tv_sec  dd 0
    tv_usec dd 0
input_buffer: times input_buffer_size db 0 ;used for accept
crlf: db 10, 13, 0 ;Used to print crlf
