
internal void thread_context_init_and_equip(Thread_Context* thread_context) {
  MemoryZeroStruct(thread_context);
  Arena **arena_ptr = thread_context->arenas;
  for (u64 i = 0; i < ArrayCount(thread_context->arenas); i += 1, arena_ptr += 1){
    *arena_ptr = arena_init();
  }
  ThreadContextThreadLocal = thread_context;
}

internal void thread_context_free() {
  for(u64 i = 0; i < ArrayCount(ThreadContextThreadLocal->arenas); i += 1) {
    arena_release(ThreadContextThreadLocal->arenas[i]);
  }
}

internal Thread_Context* thread_context_get_equipped() {
  return ThreadContextThreadLocal;
}

internal Arena* thread_context_get_scratch(Arena** conflicts, u64 count) {
  Thread_Context *thread_context = thread_context_get_equipped();
  
  Arena *result = 0;
  Arena **arena_ptr = thread_context->arenas;
  for (u64 i = 0; i < ArrayCount(thread_context->arenas); i += 1, arena_ptr += 1){
    Arena **conflict_ptr = conflicts;
    b32 has_conflict = 0;
    for (u64 j = 0; j < count; j += 1, conflict_ptr += 1){
      if (*arena_ptr == *conflict_ptr){
        has_conflict = 1;
        break;
      }
    }
    if (!has_conflict){
      result = *arena_ptr;
      break;
    }
  }
  
  return result;
}