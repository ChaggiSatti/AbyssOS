global load_page_directory
global enable_paging

section .text

; void load_page_directory(uint32_t* pointer)
load_page_directory:
    push ebp
    mov ebp, esp
    
    mov eax, [ebp+8]   ; Grab the pointer we passed from C++
    mov cr3, eax       ; Load it into the magical CR3 register
    
    mov esp, ebp
    pop ebp
    ret

; void enable_paging()
enable_paging:
    push ebp
    mov ebp, esp
    ; Enable PSE (Page Size Extension) so we can use 4MB pages
    mov eax, cr4
    or  eax, 0x10 
    mov cr4, eax

    mov eax, cr0
    or eax, 0x80000000 ; Flip the 31st bit (the Paging Bit) to 1
    mov cr0, eax       ; Push it back into the CPU! (Paging turns ON right here)
    
    mov esp, ebp
    pop ebp
    ret
