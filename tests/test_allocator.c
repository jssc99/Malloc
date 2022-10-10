#include <stdlib.h>
#include <stdio.h>
 
#include <m_allocator.h>
 
void test_alloc()
{
   // MALLOC
   int* a = malloc(sizeof(int));
   *a = 10;
   printf("(malloc) a = %d\n", *a);
}
 
int main()
{
   m_setup_hooks();
   test_alloc();
 
   return 0;
}
