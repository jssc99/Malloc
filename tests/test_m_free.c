#include <stdlib.h>
#include <stdio.h>

#include <m_allocator.h>

void test_free(void)
{
   char *ptr1 = malloc(128 * sizeof(char));
   char *ptr2 = malloc(64 * sizeof(char));
   char *ptr3 = malloc(6400 * sizeof(char));
   m_show_info();

   free(ptr3);
   m_show_info();

   free(ptr1);
   m_show_info();

   free(ptr2);
   m_show_info();
}