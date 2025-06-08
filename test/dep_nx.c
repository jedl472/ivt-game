#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

void original_function() {
    printf("Hello world\n");
}

int main() {
    // Allocate memory with execute permissions
    void *exec_mem = mmap(NULL, 4096,
                          PROT_READ | PROT_WRITE | PROT_EXEC,
                          MAP_ANON | MAP_PRIVATE, -1, 0);
    
    if (exec_mem == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("exex_mem: %x\n", exec_mem);

    // Copy machine code of the function (this assumes position-independent code)
    memcpy(exec_mem, (void *)original_function, 100);  // 100 is a guess here
    printf("memcpy reached\n"); 
    // Cast to function pointer and call
    void (*func)() = (void (*)())exec_mem;
    
    printf("cast reached\n"); 
    func();  // executes the copied code
    
    printf("execution reached\n"); 
    return 0;
}
