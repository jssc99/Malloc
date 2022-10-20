#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    void *brk = sbrk(0);
    printf("sbrk(0) = %p\n", sbrk(0));

    printf("brk = sbrk(3000): %p\n", brk = sbrk(3000));
    printf("sbrk(0) = %p\n", sbrk(0));

    printf("brk = sbrk(200): %p\n", brk = sbrk(200));
    printf("sbrk(0) = %p\n", sbrk(0));

    printf("brk = sbrk(-200): %p\n", brk = sbrk(-200));
    printf("sbrk(0) = %p\n", sbrk(0));

    printf("brk = sbrk(-3000): %p\n", brk = sbrk(-3000));
    printf("sbrk(0) = %p\n", sbrk(0));
    
}