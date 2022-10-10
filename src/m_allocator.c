#include <malloc.h> // Nécessaire pour les hooks
#include <m_allocator.h>
#include "metadata.h"

static Metadata *metadata = NULL;

Metadata *get_free_block(size_t size)
{
   if (metadata != NULL)
   {
      while (metadata->blockSize < size)
      {
         if (metadata->isOccupied == 0)
            return metadata;
         if (metadata->next != NULL)
            metadata = metadata->next;
         else
            return NULL;
      }
   }
   return NULL;
}

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
   metadata = get_free_block(size);
   if (metadata == NULL)
   {
      Metadata newMetadata;
      newMetadata.adr = sbrk(size);
      newMetadata.blockSize = size;
      newMetadata.isOccupied = 1;
      if (metadata != NULL)
      {
         metadata->next = &newMetadata;
         metadata = metadata->next;
      }
      else
         metadata = &newMetadata;
   }
   else
      metadata->isOccupied = 1;
   return metadata->adr;
}

void *m_realloc(void *ptr, size_t size)
{
   return 0;
}

void *m_calloc(size_t nb, size_t size)
{
   return 0;
}

void m_free(void *ptr)
{
}

void m_show_info(void)
{
}
