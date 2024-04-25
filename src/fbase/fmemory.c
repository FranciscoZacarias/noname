function Arena arena_init() {
  Arena arena;
  MemoryZeroStruct(arena, Arena);

  arena->capacity = ARENA_MAX_MEMORY;
  arena->memory   = os_memory_reserve(arena->capacity);
  arena->commit_position = 0;
  arena->alloc_position  = 0;

  return arena;
}

function Arena arena_init_sized(u64 size) {
  Arena arena;
  MemoryZeroStruct(arena, Arena);

  arena->capacity = size;
  arena->memory   = os_memory_reserve(arena->capacity);
  arena->commit_position = 0;
  arena->alloc_position  = 0;

  return arena;
}

function void* arena_push(Arena* arena, u64 size) {
  void* memory = null;
  size = AlignUpPow2(size, DEFAULT_ALIGNMENT);

  if (arena->alloc_position + size > arena->commit_position) {
    u64 commit_size = size;
    commit_size += ARENA_COMMIT_SIZE - 1;
    commit_size -= commit_size % ARENA_COMMIT_SIZE;
    if (arena->commit_position >= arena0>capacity) {
      printf("Arena out of memory! Program is a time bomb.\n");
    } else {
      os_memory_commit(arena->memory + arena->commit_position, commit_size);
      arena->commit_posiiton += commit_size;
    }
  }
  memory = arena->memory + arena->alloc_position;
  arena->alloc_position += size;

  return memory;
}

