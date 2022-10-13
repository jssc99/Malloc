#include <malloc.h> // Nécessaire pour les hooks
#include <m_allocator.h>
#include "metadata.h"

#define METASIZE sizeof(Metadata)

static Metadata *gHead = NULL;
static int gHooked = 0;

extern void *__libc_malloc(size_t);
extern void *__libc_calloc(size_t, size_t);
extern void *__libc_realloc(void *, size_t);
extern void __libc_free(void *);

void *malloc(size_t size) { return gHooked ? m_malloc(size) : __libc_malloc(size); }
void *calloc(size_t num, size_t size) { return gHooked ? m_calloc(num, size) : __libc_calloc(num, size); }
void *realloc(void *ptr, size_t size) { return gHooked ? m_realloc(ptr, size) : __libc_realloc(ptr, size); }
void free(void *ptr) { gHooked ? m_free(ptr) : __libc_free(ptr); }

void m_setup_hooks(void)
{
   gHooked = 1;
}

size_t make_it_byte_sized(size_t size)
{
   while (size % 8 != 0)
      size++;
   return size;
}

Metadata *get_free_block(size_t size)
{
   for (Metadata *find = gHead; find != NULL; find = find->next)
      if (find->isOccupied == 0 && find->blockSize >= size)
         return find;
   return NULL;
}

Metadata *get_block_with_addr(void *ptr)
{
   for (Metadata *find = gHead; find != NULL; find = find->next)
      if (find->addr == ptr)
         return find;
   return NULL;
}

void *m_malloc(size_t size)
{
   size = make_it_byte_sized(size);
   Metadata *freeBlock = get_free_block(size);
   if (freeBlock)
   {
      if (freeBlock->blockSize == size)
         freeBlock->isOccupied = 1;
      else
      {
         if (((int)freeBlock->blockSize - (int)size - (int)METASIZE) >= 8)
         {
            Metadata *newMetadata = freeBlock->addr + size;
            newMetadata->addr = freeBlock->addr + size + METASIZE;
            newMetadata->blockSize = (freeBlock->blockSize - size - METASIZE);
            newMetadata->isOccupied = 0;
            newMetadata->next = freeBlock->next;
            freeBlock->next = newMetadata;
            freeBlock->blockSize = size;
         }
         freeBlock->isOccupied = 1;
      }
      return freeBlock->addr;
   }
   else
   {
      Metadata *newMetadata = sbrk(METASIZE);
      newMetadata->addr = sbrk(size);
      newMetadata->blockSize = size;
      newMetadata->isOccupied = 1;
      newMetadata->next = NULL;
      if (gHead)
      {
         freeBlock = gHead;
         while (freeBlock->next)
            freeBlock = freeBlock->next;
         freeBlock->next = newMetadata;
      }
      else
         gHead = newMetadata;
      return newMetadata->addr;
   }
   return NULL;
}

void *m_realloc(void *ptr, size_t size)
{
   size = make_it_byte_sized(size);
   Metadata *realloc = get_block_with_addr(ptr);
   size_t extention = size - realloc->blockSize;
   if (!realloc->next)
   {
      sbrk(extention);
      realloc->blockSize = size;
   }
   else
   {
      ptr = m_malloc(size);
      Metadata *newRealloc = get_block_with_addr(ptr);
      if (extention > 0)
         extention = 0;
      for (size_t i = 0; i < realloc->blockSize + extention; i++)
         newRealloc->addr[i] = realloc->addr[i];
      m_free(realloc->addr);
   }
   return ptr;
}

void *m_calloc(size_t nb, size_t size)
{
   size_t _size = make_it_byte_sized(size * nb);
   void *ptr = m_malloc(_size);
   if (ptr)
   {
      Metadata *calloc = get_block_with_addr(ptr);
      for (size_t i = 0; i < _size; i++)
         calloc->addr[i] = 0;
   }
   return ptr;
}

void m_free(void *ptr)
{
   Metadata *delete = get_block_with_addr(ptr);
   delete->isOccupied = 0;

   for (Metadata *fusion = get_free_block(1); fusion != NULL; fusion = fusion->next)
      if (fusion->next != NULL && !fusion->isOccupied && !fusion->next->isOccupied)
      {
         fusion->blockSize += fusion->next->blockSize + METASIZE;
         fusion->next = fusion->next->next;
      }

   if (!gHead->next && !gHead->isOccupied)
   {
      gHead = sbrk(-(gHead->blockSize + METASIZE));
      gHead = NULL;
   }
   else
   {
      Metadata *last = gHead;
      Metadata *beforeLast = last - 1;
      while (last->next)
      {
         beforeLast = last;
         last = last->next;
      }
      if (!last->isOccupied)
      {
         sbrk(-(last->blockSize + METASIZE));
         beforeLast->next = NULL;
      }
   }
}

void m_show_info(void)
{
   Metadata *print = gHead;
   for (int i = 0; print != NULL; print = print->next, i++)
      printf("METADATA #%d | addr: %p | blockSize: %4lu | occupied: %3s | addrNextMeta: %14p | %8s: %14p\n",
             i,
             print->addr, print->blockSize,
             print->isOccupied ? "yes" : "no",
             print->next,
             print->next ? "addrNext" : "calc brk",
             print->next ? print->next->addr : (print->addr + print->blockSize));
   printf("sbrk(0) = %p\n\n", sbrk(0));
}
