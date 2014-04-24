        .global _start

        .text
_start:
        # write(1, message, 13)
        mov     $1, %rax                # system call 1 is write
        mov     $1, %rdi                # file handle 1 is stdout
        mov     $message, %rsi          # address of string to output
        mov     $13, %rdx               # number of bytes
        syscall                         # invoke operating system to do the write

        # exit(1)
        mov     $60, %rax               # system call 60 is exit
        mov     $1, %rdi              # we want return code 1
        syscall                         # invoke operating system to exit
message:
        .ascii  "Hello, World\n"
