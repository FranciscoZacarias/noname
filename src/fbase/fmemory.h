
#ifndef FMEMORY_H
#define FMEMORY_H

#define ARENA_MAX_MEMORY Gigabytes(1)
#define ARENA_COMMIT_SIZE Kilobytes(8)

typedef struct Arena {
  u64 capacity;
  u64 commit_position;
  u64 alloc_position;
  u8* memory;
} Arena;

function Arena arena_init();
function Arena arena_init_sized(u64 size);

function void* arena_push(Arena* arena, u64 size);
function void  arena_pop(Arena* arena, u64 pos);
function void  arena_pop_to(Arena* arena, u64 pos);

typedef struct ArenaTemp {
  Arena* arena;
  u64 position;
} ArenaTemp;

function ArenaTemp arena_temp_begin(Arena* arena);
function ArenaTemp arena_temp_end(ArenaTemp* temp);

#endif // FMEMORY_H