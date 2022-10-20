#include <malloc.h> // Nécessaire pour les hooks
#include <m_allocator.h>
#include "metadata.h"

static Metadata *gHead = NULL;
static int gHooked = 0;
static int freeblocks = 0;

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
   while (size % 8)
      size++;
   return size;
}

size_t total_size_of(Metadata *meta)
{
   return (sizeof(Metadata) + meta->blockSize);
}

void *get_addr_block(Metadata *meta)
{
   return (void *)(meta + 1);
}

Metadata *get_first_free_meta(void)
{
   for (Metadata *find = gHead; find != NULL; find = find->next)
      if (find->isOccupied == 0)
         return find;
   return NULL;
}

Metadata *get_smallest_free_meta(size_t size)
{
   Metadata *smallest = NULL;
   for (Metadata *find = gHead; find != NULL; find = find->next)
      if (find->isOccupied == 0 && find->blockSize >= size)
      {
         if (smallest && smallest->blockSize > find->blockSize)
            smallest = find;
         else if (!smallest)
            smallest = find;
      }
   return smallest;
}

Metadata *get_size_exact_free_meta(size_t size)
{
   Metadata *exact = get_smallest_free_meta(size);
   if (exact && exact->blockSize == size)
      return exact;
   else
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
   if (freeBlock->blockSize != size && freeBlock->blockSize - size >= sizeof(Metadata) + 8)
   {
      size_t sizeLeft = freeBlock->blockSize - size - sizeof(Metadata);
      Metadata *newMetadata = make_metadata(get_addr_block(freeBlock) + size,
                                            sizeLeft, 0, freeBlock->next);
      freeBlock->next = newMetadata;
      freeBlock->blockSize = size;
   }
   else
      freeblocks--;
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

void transfert_data(Metadata *old, Metadata *new, size_t size)
{
   char *addrRealloc = get_addr_block(old);
   char *addrPerfect = get_addr_block(new);
   for (unsigned int i = 0; i < size; i++)
      addrPerfect[i] = addrRealloc[i];
}

void *perfect_fit(Metadata *perfect, Metadata *realloc, size_t size)
{
   perfect->isOccupied = 1;
   transfert_data(realloc, perfect, size);
   m_free(get_addr_block(realloc));
   freeblocks--;
   return get_addr_block(perfect);
}

void free_space_next(Metadata *realloc, size_t size, long extention)
{
   Metadata *nextOne = realloc->next;
   Metadata *newMetadata = make_metadata((void *)nextOne + extention,
                                         nextOne->blockSize - extention, 0, nextOne->next);
   realloc->next = newMetadata;
   realloc->blockSize = size;
}

void *create_new_block(Metadata *realloc, size_t size, long extention)
{
   void *ptr = m_malloc(size);
   Metadata *newRealloc = get_meta_with_addr(ptr);
   if (extention > 0)
      extention = 0;
   transfert_data(realloc, newRealloc, realloc->blockSize + extention);
   m_free(get_addr_block(realloc));
   return ptr;
}

void *m_realloc(void *ptr, size_t size)
{
   size = make_it_byte_sized(size);
   Metadata *realloc = get_meta_with_addr(ptr);
   if (realloc->blockSize == size)
      return ptr;

   long extention = (long)size - (long)realloc->blockSize;
   Metadata *perfect = get_size_exact_free_meta(size);

   if (perfect)
      return perfect_fit(perfect, realloc, size);
   else if (realloc->next && !realloc->next->isOccupied && realloc->blockSize - size >= sizeof(Metadata) + 8)
      free_space_next(realloc, size, extention);
   else if (!realloc->next) // if end of list
   {
      sbrk(extention);
      realloc->blockSize = size;
   }
   else
      ptr = create_new_block(realloc, size, extention);
   return ptr;
}

void *m_calloc(size_t nb, size_t size)
{
   size_t _size = make_it_byte_sized(size * nb);
   void *ptr = m_malloc(_size);
   if (ptr)
   {
      char *addrBlockCalloc = get_addr_block(get_meta_with_addr(ptr));
      for (size_t i = 0; i < _size; i++)
         addrBlockCalloc[i] = 0;
   }
   return ptr;
}

void fusion(void)
{
   for (Metadata *fusion = get_first_free_meta(); fusion != NULL; fusion = fusion->next)
      if (fusion->next != NULL && !fusion->isOccupied && !fusion->next->isOccupied)
         while (fusion->next && !fusion->next->isOccupied)
         {
            fusion->blockSize += total_size_of(fusion->next);
            freeblocks--;
            fusion->next = fusion->next->next;
         }
}

void sbrk_placement(void)
{
   if (!gHead->next && !gHead->isOccupied)
   {
      gHead = sbrk(-(total_size_of(gHead)));
      gHead = NULL;
      freeblocks--;
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
         freeblocks--;
      }
   }
}

void m_free(void *ptr)
{
   Metadata *delete = get_meta_with_addr(ptr);
   if (delete)
   {
      delete->isOccupied = 0;
      freeblocks++;
   }

   if (freeblocks > 1)
      fusion();
   else if (freeblocks < 0)
      printf("error freeblocks");

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
   printf("actual sbrk(0) = %p\n", sbrk(0));
   printf("nb free blocks: %d\n\n", freeblocks);
}