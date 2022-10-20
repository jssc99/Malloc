#include <stdlib.h>
#include <stdio.h>

#include <m_allocator.h>

void test_malloc(void)
{
   char *ptr1 = malloc(128 * sizeof(char));
   char *ptr2 = malloc(64 * sizeof(char));
   m_show_info();

   free(ptr1);
   free(ptr2);
   m_show_info();

   char *ptr3 = malloc(128 * sizeof(char));
   m_show_info();

   free(ptr3);
   m_show_info();
}

void test_split(void)
{
   char *ptr1 = malloc(128 * sizeof(char));
   char *ptr2 = malloc(64 * sizeof(char));
   m_show_info();

   free(ptr1);
   m_show_info();

   char *ptr3 = malloc(60 * sizeof(char));
   m_show_info();

   free(ptr2);
   free(ptr3);
   m_show_info();
}

void test_fusion(void)
{
   char *ptr1 = malloc(128 * sizeof(char));
   char *ptr2 = malloc(64 * sizeof(char));
   m_show_info();

   free(ptr1);
   m_show_info();

   char *ptr3 = malloc(60 * sizeof(char));
   m_show_info();

   free(ptr3);
   m_show_info();
   
   free(ptr2);
   m_show_info();
}