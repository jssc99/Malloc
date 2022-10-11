#include <malloc.h> // Nécessaire pour les hooks
#include <m_allocator.h>
#include "metadata.h"

static Metadata *gHead = NULL;

Metadata *get_free_block(size_t size)
{
   for (Metadata *find = gHead; find != NULL; find = find->next)
      if (find->isOccupied == 0 && find->blockSize >= size)
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
   Metadata *freeBlock = get_free_block(size);
   Metadata *newMetadata = sbrk(sizeof(Metadata));
   if (freeBlock != NULL)
   {
      if (freeBlock->blockSize == size)
         freeBlock->isOccupied = 1;
      else
      {
         if (((int)freeBlock->blockSize - (int)size - (int)sizeof(Metadata)) > 0)
         {
            newMetadata->adr = freeBlock->adr + size;
            newMetadata->blockSize = (freeBlock->blockSize - size - sizeof(Metadata));
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
   }
   return newMetadata->adr;
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
   for (Metadata *delete = gHead; delete != NULL; delete = delete->next)
      if (delete->adr == ptr)
         delete->isOccupied = 0;
   for (Metadata *fusion = gHead; fusion != NULL; fusion = fusion->next)
      if (fusion->next != NULL && fusion->isOccupied == 0 && fusion->next->isOccupied == 0)
      {
         Metadata *nextOne = fusion->next;
         fusion->blockSize += nextOne->blockSize;
         fusion->next = nextOne->next;
         m_free(nextOne);
      }
}

void m_show_info(void)
{
   for (Metadata *print = gHead; print != NULL; print = print->next)
      printf("METADATA | adr : %p, blockSize : %3lu, occupied : %3s, adrNextMeta : %p, adrNext : %p\n",
             print->adr, print->blockSize,
             print->isOccupied ? "yes" : "no",
             print->next,
             print->next ? print->next->adr : NULL);
   printf("\n");
}
