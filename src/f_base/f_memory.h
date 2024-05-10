
#ifdef F_MEMORY_HEADER

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
function ArenaTemp arena_temp_end(ArenaTemp* temp);

#undef F_MEMORY_HEADER
#endif // F_MEMORY_HEADER

#ifdef F_MEMORY_IMPLEMENTATION

function Arena arena_init() {
  Arena arena;
  MemoryZeroStruct(&arena, Arena);

  arena.capacity = ARENA_MAX_MEMORY;
  arena.memory   = os_memory_reserve(arena.capacity);
  arena.commit_position = 0;
  arena.alloc_position  = 0;

  return arena;
}

function Arena arena_init_sized(u64 size) {
  Arena arena;
  MemoryZeroStruct(&arena, Arena);

  arena.capacity = size;
  arena.memory   = os_memory_reserve(arena.capacity);
  arena.commit_position = 0;
  arena.alloc_position  = 0;

  return arena;
}

function void* arena_push(Arena* arena, u64 size) {
  void* memory = null;
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

function void  arena_pop(Arena* arena, u64 size) {
  if (size > arena->alloc_position) {
    printf("Warning :: Arena :: Trying to pop %lld bytes from arena with %lld allocated. Will pop %lld instead of %lld.\n", size, arena->alloc_position, arena->alloc_position, size);
    size = arena->alloc_position;
  }
  arena->alloc_position -= size;
}

function void  arena_pop_to(Arena* arena, u64 pos) {
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

function void  arena_clear(Arena* arena) {
  arena_pop(arena, arena->alloc_position);
}

function void  arena_free(Arena* arena) {
  os_memory_release(arena->memory, arena->capacity);
}

function ArenaTemp arena_temp_begin(Arena* arena) {
  ArenaTemp temp;
  temp.arena = arena;
  temp.temp_position = arena->alloc_position;
  return temp;
}

function ArenaTemp arena_temp_end(ArenaTemp* temp) {
  arena_pop_to(temp->arena, temp->temp_position);
}

#undef F_MEMORY_IMPLEMENTATION
#endif // F_MEMORY_IMPLEMENTATION