#include <stdlib.h>
#include <stdio.h>

#include <m_allocator.h>

void test_alloc()
{ 
    // MALLOC
   m_show_info();/*
   char *array1 = m_malloc(128 * sizeof(char));
   m_show_info();
   char *array2 = m_malloc(64 * sizeof(char));
   m_show_info();
   free(array1);
   m_show_info();
   char *array3 = m_malloc(16 * sizeof(char));
   m_show_info();
   char *array5 = m_malloc(80 * sizeof(char));
   m_show_info();*/
   char *array4 = m_malloc(96 * sizeof(char));
   m_show_info();/*
   free(array3);
   m_show_info();*/
   free(array4);
   m_show_info();/*
   free(array5);
   m_show_info();*/
   char *array6 = m_malloc(94 * sizeof(char));
   m_show_info();
}

int main()
{
   printf("START\n");
   m_setup_hooks();
   test_alloc();

   return 0;
}
