
typedef struct Metadata
{
   size_t blockSize;
   char *addr;
   _Bool isOccupied;
   struct Metadata *next;
} Metadata;

Metadata* get_free_block(size_t size);