#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdio.h>

int main()
{
    // Vos tests ici
    void *brk;

    brk = sbrk(0x3100);
    printf("New break value after sbrk( 0x3100 ) \t%p\n", brk);

    brk = sbrk(0x0200);
    printf("New break value after sbrk( 0x0200 ) \t%p\n", brk);
}
