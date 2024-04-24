
#ifndef FMEMORY_H
#define FMEMORY_H

#define ARENA_MAX_MEMORY Gigabytes(1)
#define ARENA_COMMIT_SIZE Kilobytes(8)

typedef struct Arena {
  u64 capacity;
  u64 position;
  u8* memory;
} Arena;


Arena arena_create(u64 size);


#endif // FMEMORY_H