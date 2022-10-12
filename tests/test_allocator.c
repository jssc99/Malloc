#include <stdlib.h>
#include <stdio.h>

#include <m_allocator.h>

void test_alloc()
{
   // MALLOC
   printf("ARRAY1 MALLOC\n");
   char *array1 = m_malloc(128 * sizeof(char));
   m_show_info();
   printf("ARRAY2 MALLOC\n");
   char *array2 = m_malloc(64 * sizeof(char));
   m_show_info();
   printf("ARRAY1 FREE\n");
   free(array1);
   m_show_info();
   printf("ARRAY3 MALLOC\n");
   char *array3 = m_malloc(15 * sizeof(char));
   m_show_info();
   printf("ARRAY5 MALLOC\n");
   char *array5 = m_malloc(79 * sizeof(char));
   m_show_info();
   printf("ARRAY4 MALLOC\n");
   char *array4 = m_malloc(95 * sizeof(char));
   m_show_info();
   printf("ARRAY4 REALLOC\n");
   array4 = m_realloc(array4, 130 * sizeof(char));
   m_show_info();
   printf("ARRAY3 FREE\n");
   free(array3);
   m_show_info();
   printf("ARRAY4 FREE\n");
   free(array4);
   m_show_info();
   printf("ARRAY5 FREE\n");
   free(array5);
   m_show_info();
   printf("ARRAY6 MALLOC\n");
   char *array6 = m_malloc(94 * sizeof(char));
   m_show_info();
   printf("ARRAY7 CALLOC\n");
   char *array7 = m_calloc(119, sizeof(char));
   m_show_info();
   array7[5] = 1;
   array7[119] = 1;
   printf("CALLOC ARRAY7 MEM:\n");
   for (int i = 0; i < 120 * (int)sizeof(char); i++)
      printf("%d ", array7[i]);
   printf("\n\n");
   printf("ARRAY2 FREE\n");
   free(array2);
   m_show_info();
   printf("ARRAY6 REALLOC\n");
   array6 = m_realloc(array6, 130 * sizeof(char));
   m_show_info();
   printf("ARRAY6 FREE\n");
   free(array6);
   m_show_info();
   printf("ARRAY7 FREE\n");
   free(array7);
   m_show_info();
}

int main()
{
   printf("START\n");
   m_setup_hooks();
   test_alloc();

   return 0;
}
