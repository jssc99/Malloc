#include <stdlib.h>
#include <stdio.h>

#include <m_allocator.h>

void test_realloc(void)
{
    char *ptr1 = malloc(sizeof(int));
    char *ptr2 = malloc(sizeof(int));
    m_show_info();

    *ptr1 = 10;
    printf("ptr1=%p - ptr2=%p\n", ptr1, ptr2);
    printf("*ptr1 = %d\n", *ptr1);
    m_show_info();

    ptr1 = realloc(ptr1, 8);
    printf("ptr1=%p - ptr2=%p\n", ptr1, ptr2);
    printf("*ptr1 = %d\n", *ptr1);
    m_show_info();

    free(ptr1);
    free(ptr2);
    m_show_info();
}