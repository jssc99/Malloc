#include <stdlib.h>
#include <stdio.h>

#include <m_allocator.h>

void test_calloc(void)
{
   int i;
   char *ptr = malloc(10 * sizeof(int));
   m_show_info();

   for (i = 0; i < 10; i++)
      printf("%d ", ptr[i]);
   printf("\n%p\n", ptr);
   m_show_info();

   ptr = calloc(10, sizeof(int));

   for (i = 0; i < 10; i++)
      printf("%d ", ptr[i]);
   printf("\n%p\n", ptr);
   m_show_info();

   free(ptr);
   m_show_info();
}