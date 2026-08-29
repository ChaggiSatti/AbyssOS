section .text

; --- MACROS TO GENERATE 32 EXCEPTION HANDLERS ---
%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    push 0   
    push %1 
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    push %1
    jmp isr_common_stub
%endmacro

; Generate ISR 0 to 31
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
ISR_NOERRCODE 128

; Send EOI (IRQ 12) to the PIC!
; --- THE COMMON C++ CALLER ---
extern fault_handler

isr_common_stub:
    pusha        ; Save all registers
    push esp     ; Pass a pointer to the stack to our C++ function!
    call fault_handler
    add esp, 4   ; Clean up the pointer we just pushed
    popa         ; Restore registers
    add esp, 8   ; Clean up the pushed error code and pushed ISR number
    iret

; --- Interrupt 32 (Timer IRQ0) ---
global isr32
extern isr32_handler
extern current_task
isr32:
    pusha
    call isr32_handler

    mov eax, [current_task]
    mov [eax+4], esp
    mov [eax+8], ebp
    
    mov ebx, [eax+16]
    mov [current_task], ebx

    mov esp, [ebx+4]
    mov ebp, [ebx+8]

    popa
    iret

; --- Interrupt 33 (Keyboard IRQ1) ---
global isr33
extern isr33_handler
isr33:
    pusha
    call isr33_handler
    popa
    iret

global isr44
extern isr44_handler
isr44:
    pusha
    call isr44_handler
    popa
    iret
