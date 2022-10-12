#include <malloc.h> // Nécessaire pour les hooks
#include <m_allocator.h>
#include "metadata.h"

#define METASIZE sizeof(Metadata)

size_t make_it_byte_sized(size_t size)
{
   while (size % 8 != 0)
      size++;
   return size;
}

static Metadata *gHead = NULL;

Metadata *get_free_block(size_t size)
{
   for (Metadata *find = gHead; find != NULL; find = find->next)
      if (find->isOccupied == 0 && find->blockSize >= size)
         return find;
   return NULL;
}

Metadata *get_block_with_adr(void *ptr)
{
   for (Metadata *find = gHead; find != NULL; find = find->next)
      if (find->adr == ptr)
         return find;
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
   size = make_it_byte_sized(size);
   Metadata *freeBlock = get_free_block(size);
   if (freeBlock != NULL)
   {
      if (freeBlock->blockSize == size)
         freeBlock->isOccupied = 1;
      else
      {
         if (((int)freeBlock->blockSize - (int)size - (int)METASIZE) > 0)
         {
            Metadata *newMetadata = freeBlock + METASIZE + size;
            newMetadata->adr = freeBlock->adr + size + METASIZE;
            newMetadata->blockSize = (freeBlock->blockSize - size - METASIZE);
            newMetadata->isOccupied = 0;
            newMetadata->next = freeBlock->next;
            freeBlock->next = newMetadata;
         }
         freeBlock->blockSize = size;
         freeBlock->isOccupied = 1;
      }
      return freeBlock->adr;
   }
   else
   {
      Metadata *newMetadata = sbrk(METASIZE);
      newMetadata->adr = sbrk(size);
      newMetadata->blockSize = size;
      newMetadata->isOccupied = 1;
      newMetadata->next = NULL;
      if (gHead != NULL)
      {
         freeBlock = gHead;
         while (freeBlock->next != NULL)
            freeBlock = freeBlock->next;
         freeBlock->next = newMetadata;
      }
      else
         gHead = newMetadata;
      return newMetadata->adr;
   }
   return NULL;
}

void *m_realloc(void *ptr, size_t size)
{
   size = make_it_byte_sized(size);
   Metadata *realloc = get_block_with_adr(ptr);
   if (realloc->next == NULL)
   {
      realloc->blockSize = size;
   }
   else if (realloc->next != NULL && realloc->next->isOccupied == 0 && ((int)realloc->next->blockSize - (int)size + (int)realloc->blockSize) > 0)
   {
      realloc->next->blockSize -= size - realloc->blockSize;
      realloc->blockSize = size;
   }
   else
   {
      m_free(realloc->adr);
      ptr = m_malloc(size);
   }
   return ptr;
}

void *m_calloc(size_t nb, size_t size)
{
   size = make_it_byte_sized(size * nb);
   void *ptr = m_malloc(size);
   if (ptr != NULL)
   {
      Metadata *calloc = get_block_with_adr(ptr);
      for (int i = 0; i < (int)size; i++)
         calloc->adr[i] = 0;
   }
   return ptr;
}

void m_free(void *ptr)
{
   Metadata *delete = get_block_with_adr(ptr);
   delete->isOccupied = 0;

   for (Metadata *fusion = get_free_block(1); fusion != NULL; fusion = fusion->next)
      if (fusion->next != NULL && fusion->isOccupied == 0 && fusion->next->isOccupied == 0)
      {
         Metadata *nextOne = fusion->next;
         fusion->blockSize += nextOne->blockSize + METASIZE;
         fusion->next = nextOne->next;
      }
}

void m_show_info(void)
{
   int i = 0;
   for (Metadata *print = gHead; print != NULL; print = print->next, i++)
      printf("METADATA #%d | adr: %p | blockSize: %4lu | occupied: %3s | adrNextMeta: %14p | adrNext: %14p\n",
             i,
             print->adr, print->blockSize,
             print->isOccupied ? "yes" : "no",
             print->next,
             print->next ? print->next->adr : NULL);
   printf("\n");
}
