#include <malloc.h> // Nécessaire pour les hooks
#include <m_allocator.h>
#include "metadata.h"

static Metadata* metadata = NULL;

void *malloc_hook(size_t size, const void *caller) { return m_malloc(size); }
void *realloc_hook(void *ptr, size_t size, const void *caller) { return m_realloc(ptr, size); }
void free_hook(void *ptr, const void *caller) { return m_free(ptr); }

void m_setup_hooks(void)
{
   __malloc_hook = malloc_hook;
   __realloc_hook = realloc_hook;
   __free_hook = free_hook;
}

void *m_malloc(size_t size) 
{
   return (void *)sbrk(size);
}

void *m_realloc(void *ptr, size_t size)
{
}

void *m_calloc(size_t nb, size_t size)
{
}

void m_free(void *ptr)
{
}

void m_show_info(void)
{
}
