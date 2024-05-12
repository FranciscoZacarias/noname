
#ifndef F_MEMORY_H
#define F_MEMORY_H

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
function void  arena_pop(Arena* arena, u64 size);
function void  arena_pop_to(Arena* arena, u64 pos);
function void  arena_clear(Arena* arena);
function void  arena_free(Arena* arena);

#define push_array_no_zero(a,T,c) (T*)arena_push((a), sizeof(T)*(c))
#define push_array(a,T,c) (T*)MemoryZero(push_array_no_zero(a,T,c), sizeof(T)*(c))

typedef struct ArenaTemp {
  Arena* arena;
  u64 temp_position;
} ArenaTemp;

function ArenaTemp arena_temp_begin(Arena* arena);
function void      arena_temp_end(ArenaTemp* temp);

#endif // F_MEMORY_H