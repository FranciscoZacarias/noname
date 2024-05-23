#ifndef F_THREAD_CONTEXT_H
#define F_THREAD_CONTEXT_H

typedef struct Thread_Context {
  Arena* arenas[2];
} Thread_Context;

C_LINKAGE thread_static Thread_Context* ThreadContextThreadLocal = 0;

internal void thread_context_init_and_attach(Thread_Context* thread_context);
internal void thread_context_free();
internal Thread_Context* thread_context_get_equipped();

internal Arena* thread_context_get_scratch(Arena** conflicts, u64 count);

#define scratch_begin(conflicts, count) arena_temp_begin(thread_context_get_scratch((conflicts), (count)))
#define scratch_end(scratch) arena_temp_end(scratch)

#endif // F_THREAD_CONTEXT_H