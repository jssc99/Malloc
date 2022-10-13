
typedef struct Metadata
{
   // Add metadata information needed
   // e.g. size of the block, pointer, free/occupied
   char *addr;
   size_t blockSize;
   _Bool isOccupied;
   //struct Metadata *prev;
   struct Metadata *next;
} Metadata;

Metadata* get_free_block(size_t size);