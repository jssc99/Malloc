#include <malloc.h> // Nécessaire pour les hooks
#include <m_allocator.h>
#include "metadata.h"

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

size_t total_size_of(Metadata *meta)
{
   return (sizeof(Metadata) + meta->blockSize);
}

char *get_addr_block(Metadata *meta)
{
   return (char *)(meta + 1);
}

Metadata *get_free_meta(size_t size)
{
   for (Metadata *find = gHead; find != NULL; find = find->next)
      if (find->isOccupied == 0 && find->blockSize >= size)
         return find;
   return NULL;
}

Metadata *get_smallest_free_meta(size_t size)
{
   Metadata *smallest = NULL;
   for (Metadata *find = gHead; find != NULL; find = find->next)
   {
      if (find->isOccupied == 0 && find->blockSize >= size)
      {
         if (smallest && smallest->blockSize > find->blockSize)
            smallest = find;
         else
            smallest = find;
      }
   }
   if (smallest)
      return smallest;
   return NULL;
}

Metadata *get_meta_with_addr(void *ptr)
{
   for (Metadata *find = gHead; find != NULL; find = find->next)
      if (get_addr_block(find) == ptr)
         return find;
   return NULL;
}

Metadata *make_metadata(void *addrMeta, size_t blockSize, _Bool occupied, Metadata *nextOne)
{
   Metadata *newMetadata = addrMeta;
   newMetadata->blockSize = blockSize;
   newMetadata->isOccupied = occupied;
   newMetadata->next = nextOne;
   return newMetadata;
}

char *split(Metadata *freeBlock, size_t size)
{
   long sizeLeft = (long)freeBlock->blockSize - (long)size - (long)sizeof(Metadata);
   if (freeBlock->blockSize != size && sizeLeft >= 8)
   {
      Metadata *newMetadata = make_metadata(get_addr_block(freeBlock) + size, sizeLeft, 0, freeBlock->next);
      freeBlock->next = newMetadata;
      freeBlock->blockSize = size;
   }
   freeBlock->isOccupied = 1;
   return get_addr_block(freeBlock);
}

char *new_end_block(size_t size)
{
   Metadata *newMetadata = make_metadata(sbrk(sizeof(Metadata) + size), size, 1, NULL);
   if (gHead)
   {
      Metadata *curr = gHead;
      while (curr->next)
         curr = curr->next;
      curr->next = newMetadata;
   }
   else
      gHead = newMetadata;
   return get_addr_block(newMetadata);
}

void *m_malloc(size_t size)
{
   size = make_it_byte_sized(size);
   Metadata *freeBlock = get_smallest_free_meta(size);
   if (freeBlock)
      return split(freeBlock, size);
   else
      return new_end_block(size);
   return NULL;
}

void *m_realloc(void *ptr, size_t size)
{
   size = make_it_byte_sized(size);
   Metadata *realloc = get_meta_with_addr(ptr);
   if (realloc->blockSize == size)
      return ptr;
      
   long extention = (long)size - (long)realloc->blockSize;
   if (!realloc->next)
   {
      sbrk(extention);
      realloc->blockSize = size;
   }
   else if (realloc->next && !realloc->next->isOccupied && (long)realloc->next->blockSize - extention >= 8)
   {
      Metadata *nextOne = realloc->next;
      Metadata *newMetadata = make_metadata(get_addr_block(nextOne) + extention, nextOne->blockSize - extention, 0, nextOne->next);
      realloc->next = newMetadata;
      realloc->blockSize = size;
      // free(nextOne);
   }
   else
   {
      ptr = m_malloc(size);
      Metadata *newRealloc = get_meta_with_addr(ptr);
      if (extention > 0)
         extention = 0;
      char *addrRealloc = get_addr_block(realloc);
      char *addrNewRealloc = get_addr_block(newRealloc);
      for (size_t i = 0; i < realloc->blockSize + extention; i++)
         addrNewRealloc[i] = addrRealloc[i];
      m_free(get_addr_block(realloc));
   }
   return ptr;
}

void *m_calloc(size_t nb, size_t size)
{
   size_t _size = make_it_byte_sized(size * nb);
   void *ptr = m_malloc(_size); 
    if (ptr)
    {
       Metadata *calloc = get_meta_with_addr(ptr);
       char *addrCalloc = get_addr_block(calloc);
       for (size_t i = 0; i < _size; i++)
          addrCalloc[i] = 0;
    }
   return ptr;
}

void fusion(void) // improve?
{
   for (Metadata *fusion = get_free_meta(1); fusion != NULL; fusion = fusion->next)
      if (fusion->next != NULL && !fusion->isOccupied && !fusion->next->isOccupied)
      {
         fusion->blockSize += total_size_of(fusion->next);
         fusion->next = fusion->next->next;
      }
}

void sbrk_placement(void)
{
   if (!gHead->next && !gHead->isOccupied)
   {
      gHead = sbrk(-(total_size_of(gHead)));
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
         sbrk(-(total_size_of(last)));
         beforeLast->next = NULL;
      }
   }
}

void m_free(void *ptr)
{
   Metadata *delete = get_meta_with_addr(ptr);
   if (delete)
      delete->isOccupied = 0;

   fusion();
   sbrk_placement();
}

void m_show_info(void)
{
   Metadata *print = gHead;
   for (int i = 0; print != NULL; print = print->next, i++)
      printf("METADATA #%d | addrMeta: %p | addrBlock: %p | size: %4lu | free: %3s \n\t    | nextMeta: %14p | %s: %14p\n",
             i, print,
             get_addr_block(print),
             print->blockSize,
             !print->isOccupied ? "yes" : "no",
             print->next,
             print->next ? "nextblock" : "calc sbrk",
             print->next ? get_addr_block(print->next) : (get_addr_block(print) + print->blockSize));
   printf("actual sbrk(0) = %p\n\n", sbrk(0));
}
