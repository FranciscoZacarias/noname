#ifndef F_OS_H
#define F_OS_H

internal void os_init(void);

// Memory ---

internal void* os_memory_reserve(u64 size);
internal b32   os_memory_commit(void* memory, u64 size);
internal void  os_memory_decommit(void* memory, u64 size);
internal void  os_memory_release(void* memory, u64 size);

// Threading ---

typedef u64 thread_func(void* context); 

typedef struct OS_Thread {
	u64 v[1];
} OS_Thread;

internal OS_Thread os_thread_create(thread_func* start, void* context);
internal void os_thread_wait_for_join(OS_Thread* other);
internal void os_thread_wait_for_join_all(OS_Thread** threads, u32 count);
internal void os_thread_wait_for_join_any(OS_Thread** threads, u32 count);

// File handling --- 

typedef struct OS_File {
	u64 size;
	u8* data;
} OS_File;

internal b32 os_file_create(String file_name);
internal b32 os_file_exists(String file_name);
internal u32 os_file_size(String file_name);
internal u64 os_file_get_last_modified_time(String file_name);

// Logging ---

internal void os_print_string(String string);

#endif // F_OS_H