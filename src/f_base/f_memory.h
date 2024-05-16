
#ifndef F_MEMORY_H
#define F_MEMORY_H

#ifndef ARENA_RESERVE_SIZE
# define ARENA_RESERVE_SIZE Megabytes(64)
#endif
#ifndef ARENA_COMMIT_SIZE
# define ARENA_COMMIT_SIZE Kilobytes(64)
#endif

typedef struct Arena {
  u64 capacity;
  u64 commit_position;
  u64 alloc_position;
  u8* memory;
} Arena;

internal Arena arena_init();
internal Arena arena_init_sized(u64 size);

internal void* arena_push(Arena* arena, u64 size);
internal void  arena_pop(Arena* arena, u64 size);
internal void  arena_pop_to(Arena* arena, u64 pos);
internal void  arena_clear(Arena* arena);
internal void  arena_free(Arena* arena);

#define push_array_no_zero(a,T,c) (T*)arena_push((a), sizeof(T)*(c))
#define push_array(a,T,c) (T*)MemoryZero(push_array_no_zero(a,T,c), sizeof(T)*(c))

typedef struct Arena_Temp {
  Arena* arena;
  u64 temp_position;
} Arena_Temp;

internal Arena_Temp arena_temp_begin(Arena* arena);
internal void       arena_temp_end(Arena_Temp* temp);

#endif // F_MEMORY_H