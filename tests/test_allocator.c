#include <stdlib.h>
#include <stdio.h>

#include <m_allocator.h>

#include "test_m_malloc.c"
#include "test_m_realloc.c"
#include "test_m_calloc.c"
#include "test_m_free.c"

void test_allocator(void)
{
   printf("ARRAY1 MALLOC 128\n");
   char *array1 = m_malloc(128 * sizeof(char));
   m_show_info();

   printf("ARRAY2 MALLOC 64\n");
   char *array2 = m_malloc(64 * sizeof(char));
   m_show_info();

   printf("ARRAY1 FREE\n");
   m_free(array1);
   m_show_info();

   printf("ARRAY3 MALLOC 15\n");
   char *array3 = m_malloc(15 * sizeof(char));
   m_show_info();

   printf("ARRAY5 MALLOC 79\n");
   char *array5 = m_malloc(79 * sizeof(char));
   m_show_info();

   printf("ARRAY4 MALLOC 95\n");
   char *array4 = m_malloc(95 * sizeof(char));
   m_show_info();

   printf("ARRAY3 REALLOC 130\n");
   array3 = m_realloc(array3, 130 * sizeof(char));
   m_show_info();

   printf("ARRAY3 REALLOC 15\n");
   array3 = m_realloc(array3, 15 * sizeof(char));
   m_show_info();

   printf("ARRAY3 REALLOC 15\n");
   array3 = m_realloc(array3, 15 * sizeof(char));
   m_show_info();

   printf("ARRAY3 FREE\n");
   m_free(array3);
   m_show_info();

   printf("ARRAY2 FREE\n");
   m_free(array2);
   m_show_info();

   printf("ARRAY4 FREE\n");
   m_free(array4);
   m_show_info();

   printf("ARRAY6 MALLOC 94\n");
   char *array6 = m_malloc(94 * sizeof(char));
   m_show_info();

   printf("ARRAY7 CALLOC 119\n");
   char *array7 = m_calloc(120, sizeof(char));
   array7[1] = 18;
   array7[100] = 69;
   array7[140] = 'A';
   m_show_info();

   printf("CALLOC ARRAY7 MEM:\n");
   for (int i = 0; i < 120; i++)
      printf("%3d ", array7[i]);
   printf("\n\n");

   printf("ARRAY5 FREE\n");
   m_free(array5);
   m_show_info();

   printf("ARRAY7 REALLOC 102\n");
   array7 = m_realloc(array7, 100 * sizeof(char));
   m_show_info();

   printf("REALLOC ARRAY7 MEM:\n");
   for (int i = 0; i < 100; i++)
      printf("%3d ", array7[i]);
   printf("\n\n");

   printf("ARRAY6 REALLOC 130\n");
   array6 = m_realloc(array6, 130 * sizeof(char));
   m_show_info();

   printf("ARRAY7 REALLOC 150\n");
   array7 = m_realloc(array7, 150 * sizeof(char));
   m_show_info();

   printf("ARRAY7 REALLOC 180\n");
   array7 = m_realloc(array7, 180 * sizeof(char));
   m_show_info();

   printf("REALLOC ARRAY7 MEM:\n");
   for (int i = 0; i < 150; i++)
      printf("%3d ", array7[i]);
   printf("\n\n");

   printf("ARRAY8 CALLOC 135\n");
   char *array8 = m_calloc(135, sizeof(char));
   m_show_info();

   printf("CALLOC ARRAY8 MEM:\n");
   for (int i = 0; i < 136; i++)
      printf("%3d ", array8[i]);
   printf("\n\n");

   printf("ARRAY6 FREE\n");
   m_free(array6);
   m_show_info();

   printf("ARRAY8 FREE\n");
   m_free(array8);
   m_show_info();

   printf("ARRAY7 FREE\n");
   m_free(array7);
   m_show_info();
}

int main()
{
   printf("START\n");
   m_setup_hooks();

   int test = 6;

   switch (test)
   {
   case 0:
      test_allocator();
      printf("\n\n\nSTART 2\n");
      test_allocator();
      break;

   case 1:
      test_malloc();
      break;

   case 2:
      test_realloc();
      break;

   case 3:
      test_calloc();
      break;

   case 4:
      test_split();
      break;

   case 5:
      test_fusion();
      break;

   case 6:
      test_free();
      break;

   default:
      break;
   }

   return 0;
}
