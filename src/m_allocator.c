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

Metadata *make_metablock(char *addr, size_t blockSize, _Bool occupied, Metadata *nextOne)
{
   Metadata *newMetadata;
   newMetadata = addr;
   newMetadata->addr = newMetadata + 1;
   newMetadata->blockSize = blockSize;
   newMetadata->isOccupied = occupied;
   newMetadata->next = nextOne;
   return newMetadata;
}

char *split(Metadata *freeBlock, size_t size)
{
   if (freeBlock->blockSize == size)
      freeBlock->isOccupied = 1;
   else
   {
      long sizeLeft = (long)freeBlock->blockSize - (long)size - (long)METASIZE;
      if (sizeLeft >= 8)
      {
         Metadata *newMetadata = make_metablock(freeBlock->addr + size, sizeLeft, 0, freeBlock->next);
         freeBlock->next = newMetadata;
         freeBlock->blockSize = size;
      }
      freeBlock->isOccupied = 1;
   }
   return freeBlock->addr;
}

void *m_malloc(size_t size)
{
   size = make_it_byte_sized(size);
   Metadata *freeBlock = get_free_block(size);
   if (freeBlock)
      return split(freeBlock, size);
   else
   {
      Metadata *newMetadata = make_metablock(sbrk(METASIZE + size), size, 1, NULL);
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
   long extention = (long)size - (long)realloc->blockSize;
   Metadata *freeBlock = get_free_block(size);
   if (!realloc->next && !freeBlock)
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

void fusion()
{
   for (Metadata *fusion = get_free_block(1); fusion != NULL; fusion = fusion->next)
      if (fusion->next != NULL && !fusion->isOccupied && !fusion->next->isOccupied)
      {
         fusion->blockSize += fusion->next->blockSize + METASIZE;
         fusion->next = fusion->next->next;
      }
}

void sbrk_placement()
{
   if (!gHead->next && !gHead->isOccupied)
   {
      gHead = sbrk(-(gHead->blockSize + METASIZE));
      gHead = NULL;
   }
   else
   {
      Metadata *last = gHead;
      Metadata *beforeLast = last;
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

void m_free(void *ptr)
{
   Metadata *delete = get_block_with_addr(ptr);
   delete->isOccupied = 0;

   fusion();
   sbrk_placement();
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
   printf("actual sbrk(0) = %p\n\n", sbrk(0));
}
