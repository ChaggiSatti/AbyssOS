global jump_usermode
extern test_user_function ; This is a C++ function we will write next!

section .text
jump_usermode:
    cli  ; Turn off hardware interrupts so we don't crash while building the fake stack

    ; 1. Set the Data Segment registers to our new Ring 3 Data GDT Entry
    ; (Our entry is 0x20. We add 3 to tell the CPU it is Ring 3 -> 0x23)
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 2. Build the fake interrupt stack!
    push 0x23                ; SS (Stack Segment)
    push esp                 ; ESP (We'll just reuse the current stack)
    pushf                    ; EFLAGS (Push current flags)
    push 0x1B                ; CS (Ring 3 Code Entry is 0x18 + 3 = 0x1B)
    push test_user_function  ; EIP (The C++ function we want to run in Ring 3!)

    ; 3. Execute the fake return!
    iret
