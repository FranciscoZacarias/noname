
#ifndef F_MEMORY_H
#define F_MEMORY_H

#ifndef ARENA_RESERVE_SIZE
# define ARENA_RESERVE_SIZE Megabytes(64)
#endif
#ifndef ARENA_COMMIT_SIZE
# define ARENA_COMMIT_SIZE Kilobytes(64)
#endif

typedef struct Arena {
  u64 reserved;      // Reserved memory
  u64 commited;      // Commited memory
  u64 commit_size;   // Size for each commit on this arena
  u64 position;      // Current position of the arena
  u64 align;         // Arena's memory alignment
} Arena;

#define ARENA_HEADER_SIZE AlignPow2(sizeof(Arena), os_memory_get_page_size())

internal Arena* arena_init();
internal Arena* arena_init_sized(u64 reserve, u64 commit);

internal void* arena_push(Arena* arena, u64 size);
internal void  arena_pop(Arena* arena, u64 size);
internal void  arena_pop_to(Arena* arena, u64 pos);
internal void  arena_clear(Arena* arena);
internal void  arena_free(Arena* arena);

internal void arena_print(Arena *arena);

typedef struct Arena_Temp {
  Arena* arena;
  u64 temp_position;
} Arena_Temp;

internal Arena_Temp arena_temp_begin(Arena* arena);
internal void       arena_temp_end(Arena_Temp* temp);

#endif // F_MEMORY_H