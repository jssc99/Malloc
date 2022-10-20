#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *ptr1 = malloc(sizeof(int));
    char *ptr2 = malloc(sizeof(int));

    *ptr1 = 10;
    printf("ptr1=%p - ptr2=%p\n", ptr1, ptr2);
    printf("*ptr1 = %d\n", *ptr1);

    ptr1 = realloc(ptr1, 10);
    printf("ptr1=%p - ptr2=%p\n", ptr1, ptr2);
    printf("*ptr1 = %d\n", *ptr1);

    free(ptr1);
    free(ptr2);
    
}