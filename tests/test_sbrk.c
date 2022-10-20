#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    void *brk = sbrk(0);
    printf("brk = sbrk(0): %p\n", brk);
    printf("sbrk(0) = %p\n", sbrk(0));

    brk = sbrk(3000);
    printf("brk = sbrk(3000): %p\n", brk);
    printf("sbrk(0) = %p\n", sbrk(0));

    brk = sbrk(200);
    printf("brk = sbrk(200): %p\n", brk);
    printf("sbrk(0) = %p\n", sbrk(0));

    brk = sbrk(-200);
    printf("brk = sbrk(-200): %p\n", brk);
    printf("sbrk(0) = %p\n", sbrk(0));

    brk = sbrk(-3000);
    printf("brk = sbrk(-3000): %p\n", brk);
    printf("sbrk(0) = %p\n", sbrk(0));

    return 0;
}