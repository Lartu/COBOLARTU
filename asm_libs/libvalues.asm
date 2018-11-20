%define input_buffer_size 512

digit_area: times 100 db 0
timeval:
    tv_sec  dd 0
    tv_usec dd 0
input_buffer: times input_buffer_size db 0
