/***************************  Test programs  ***************************/

char div_by_zero [] =   {   0xba, 0x00, 0x00, 0x00, 0x00,         // mov 0x0, edx
                            0xb8, 0xfa, 0x00, 0x00, 0x00,         // mov 0xfa, eax
                            0xb9, 0x00, 0x00, 0x00, 0x00,         // mov 0x0, ecx
                            0xf7, 0xf1                            // div ecx ---> DIV BY 0
                        };    
                        
char div_by_not_zero [] =   {   0xba, 0x00, 0x00, 0x00, 0x00,         // mov 0x0, edx
                                0xb8, 0xfa, 0x00, 0x00, 0x00,         // mov 0xfa, eax
                                0xb9, 0x00, 0x00, 0x00, 0x01,         // mov != 0x0, ecx
                                0xf7, 0xf1                            // div ecx ---> DIV BY 0
                            };    
                            

// movb   $0x41,0x1000
char write_A_on_1000 [] = { 0xc6, 0x05, 0x00, 0x10, 0x00, 0x00, 0x41 };

char syscall_test [] = {0xb8, 0x02, 0x00, 0x00, 0x00, 0xcd, 0x80 };


/***********************************************************************/ 
 
void load_flat_binary_at (char* code, int code_len, void* memloc);
void run_flat_binary (void* memloc);
