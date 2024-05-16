
internal Arena arena_init() {
  Arena arena;
  MemoryZeroStruct(&arena);

  arena.capacity = ARENA_MAX_MEMORY;
  arena.memory   = os_memory_reserve(arena.capacity);
  arena.commit_position = 0;
  arena.alloc_position  = 0;

  return arena;
}

internal Arena arena_init_sized(u64 size) {
  Arena arena;
  MemoryZeroStruct(&arena);

  arena.capacity = size;
  arena.memory   = os_memory_reserve(arena.capacity);
  arena.commit_position = 0;
  arena.alloc_position  = 0;

  return arena;
}

internal void* arena_push(Arena* arena, u64 size) {
  void* memory = NULL;
  size = AlignUpPow2(size, DEFAULT_ALIGNMENT);

  if (arena->alloc_position + size > arena->commit_position) {
    u64 commit_size = size;
    commit_size += ARENA_COMMIT_SIZE - 1;
    commit_size -= commit_size % ARENA_COMMIT_SIZE;
    if (arena->commit_position >= arena->capacity) {
      printf("ERROR :: Arena :: Arena out of memory!\n");
      Assert(0);
    } else {
      if (os_memory_commit(arena->memory + arena->commit_position, commit_size)) {
        arena->commit_position += commit_size;
      } else {
        printf("Warning :: Arena :: Unable to commit %lld to arena.\n", size);
      }
    }
  }
  memory = arena->memory + arena->alloc_position;
  arena->alloc_position += size;

  return memory;
}

internal void  arena_pop(Arena* arena, u64 size) {
  if (size > arena->alloc_position) {
    printf("Warning :: Arena :: Trying to pop %lld bytes from arena with %lld allocated. Will pop %lld instead of %lld.\n", size, arena->alloc_position, arena->alloc_position, size);
    size = arena->alloc_position;
  }
  arena->alloc_position -= size;
}

internal void  arena_pop_to(Arena* arena, u64 pos) {
  if (pos > arena->capacity) {
    printf("Warning :: Arena :: Trying to pop over arena's capacity. Will pop only to %lld instead of %lld", arena->capacity, pos);
    pos = arena->capacity;
  } else if (pos < 0) {
    printf("Warning :: Arena :: Trying to pop negative values. Will pop to 0");
    pos = 0;
  }
  MemoryZero(arena->memory + pos, arena->alloc_position);
  arena->alloc_position = pos;
}

internal void  arena_clear(Arena* arena) {
  arena_pop(arena, arena->alloc_position);
}

internal void  arena_free(Arena* arena) {
  os_memory_release(arena->memory, arena->capacity);
}

internal Arena_Temp arena_temp_begin(Arena* arena) {
  Arena_Temp temp;
  temp.arena = arena;
  temp.temp_position = arena->alloc_position;
  return temp;
}

internal void arena_temp_end(Arena_Temp* temp) {
  arena_pop_to(temp->arena, temp->temp_position);
}
