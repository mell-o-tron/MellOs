void load_flat_binary_at (char* code, int code_len, void* memloc){
    
    char* thing = (char*) memloc;
    
    int preamble_len = 0;
    int postamble_len = 1;
    
    char *preamble;                     // empty for now
    char postamble [] = {0xc3};         // ret
    
    int i = 0;
    for (; i < preamble_len; i++){
        thing[i] = preamble[i];
    }
    
    for (; i < code_len + preamble_len; i++){
        thing[i] = code[i - preamble_len];
    }
    
    for (; i < code_len + preamble_len + postamble_len; i++){
        thing[i] = postamble[i - code_len - preamble_len];
    }
    
    return;
}


void run_flat_binary (void* memloc){
     
     __asm__("call %0" :: "m" (memloc));
}
