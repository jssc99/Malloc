#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i;
    char *pointer = malloc(10 * sizeof(int));

    for (i = 0; i < 10; i++)
        printf("%d ", pointer[i]);
    printf("\n%p\n", pointer);
    
    pointer = calloc(10, sizeof(int));

    for (i = 0; i < 10; i++)
        printf("%d ", pointer[i]);
    printf("\n%p\n", pointer);

    free(pointer);
}