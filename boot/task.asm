section .text

global perform_task_switch
extern current_task

perform_task_switch:
    ; 1. Save the CURRENT task's state onto its own stack!
    pushf           ; Push EFLAGS register
    pusha           ; Push all 8 general purpose registers
    
    ; 2. Get the pointer to the current_task struct in memory
    mov eax, [current_task]
    
    ; 3. Save the current stack pointers into the struct!
    ; (id is at offset 0. esp is at offset 4. ebp is at offset 8)
    mov [eax+4], esp
    mov [eax+8], ebp
    
    ; 4. Move to the next task in the linked list
    ; (next is at offset 16, assuming eip is still at offset 12 in your struct!)
    mov ebx, [eax+16]
    mov [current_task], ebx
    
    ; 5. THE SWITCH! Load the NEW task's stack pointers into the CPU!
    mov esp, [ebx+4]
    mov ebp, [ebx+8]
    
    ; 6. Restore the NEW task's state from its own stack!
    popa            ; Pop all 8 general purpose registers
    popf            ; Pop EFLAGS register
    
    ; 7. Because C++ called this function, 'ret' automatically pops the 
    ;    Instruction Pointer (EIP) off the stack and jumps to it!
    ret
