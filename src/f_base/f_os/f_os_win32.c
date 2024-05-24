#include <Windows.h>
#include <userenv.h>

global u64 Win32TicksOerSec = 1;
global u32 Win32ThreadContextIndex;

internal void os_init() {
  LARGE_INTEGER perf_freq = {0};
  if (QueryPerformanceFrequency(&perf_freq)) {
    Win32TicksOerSec = ((u64)perf_freq.HighPart << 32) | perf_freq.LowPart;
  }
  timeBeginPeriod(1);
	Win32ThreadContextIndex = TlsAlloc();
}

internal void* os_memory_reserve(u64 size) {
  void* result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
  return result;
}

internal b32 os_memory_commit(void* memory, u64 size) {
  b32 result = (VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

internal void  os_memory_decommit(void* memory, u64 size) {
  VirtualFree(memory, size, MEM_DECOMMIT);
}

internal void  os_memory_release(void* memory, u64 size) {
  VirtualFree(memory, 0, MEM_RELEASE);
}

internal u64 os_memory_get_page_size() {
  SYSTEM_INFO sysinfo = {0};
  GetSystemInfo(&sysinfo);
  return(sysinfo.dwPageSize);
}

internal HANDLE _win32_get_file_handle(String file_name) {
  HANDLE file_handle = CreateFileA(file_name.str, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file_handle == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    printf("Error: Failed to open file %s. Error: %lu\n", file_name.str, error);
    return NULL;
  }
  return file_handle;
}

internal b32 os_file_create(String file_name) {
  b32 result = 0;
  HANDLE file = CreateFileA(file_name.str, GENERIC_READ, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
  DWORD error = GetLastError();  
  if (error == ERROR_SUCCESS || error == ERROR_FILE_EXISTS) {
    result = 1;
  } else {
    // TODO(fz): We should send this error to user space
    printf("Error creating file %s with error: %lu\n", file_name.str, error);
  }
  CloseHandle(file);
  return result;
}

internal u64 os_file_get_last_modified_time(String file_name) {
  u32 result = 0;
  if (!os_file_exists(file_name)) {
    printf("Error: os_file_get_last_modified_time failed because file %s doesn't exist\n", file_name.str);
    return result;
  }
  WIN32_FILE_ATTRIBUTE_DATA file_attribute_data;
  if (GetFileAttributesExA(file_name.str, GetFileExInfoStandard, &file_attribute_data)) {
    FILETIME last_write_time = file_attribute_data.ftLastWriteTime;
    result = ((u64)last_write_time.dwHighDateTime << 32) | ((u64)last_write_time.dwLowDateTime);
  } 
  return result;
}

internal b32 os_file_exists(String file_name) {
  b32 result = 0;
  DWORD file_attributes = GetFileAttributesA(file_name.str);
  result = (file_attributes != INVALID_FILE_ATTRIBUTES && !(file_attributes & FILE_ATTRIBUTE_DIRECTORY));
  return result;
}

internal u32 os_file_size(String file_name) {
  u32 result = 0;
  if (!os_file_exists(file_name)) {
    printf("Error: os_file_exists failed because file %s doesn't exist\n", file_name.str);
    return result;
  }
  WIN32_FILE_ATTRIBUTE_DATA file_attribute_data;
  if (GetFileAttributesExA(file_name.str, GetFileExInfoStandard, &file_attribute_data)) {
    result = ((u64)file_attribute_data.nFileSizeHigh << 32) | ((u64)file_attribute_data.nFileSizeLow);
  }
  return result;
}

internal OS_File os_file_load_entire_file(Arena* arena, String file_name) {
  OS_File os_file = { 0 };
  if (!os_file_exists(file_name)) {
    printf("Error: os_file_load_entire_file failed because file %s doesn't exist\n", file_name.str);
    return os_file;
  }
  
  HANDLE file_handle = _win32_get_file_handle(file_name);
  if (file_handle == NULL) {
    return os_file;
  }
  
  u64 size = os_file_size(file_name);
  os_file.size = size;
  os_file.data = (u8*)arena_push(arena, size);
  MemoryZero(os_file.data, os_file.size);
  
  if (!ReadFile(file_handle, os_file.data, size, NULL, NULL)) {
    DWORD error = GetLastError();  
    printf("Error: %lu in os_file_load_entire_file::ReadFile\n", error);
  }
  
  CloseHandle(file_handle);
  return os_file;
}

internal void os_print_string(String string) {
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  WriteFile(handle, string.str, string.size, NULL, NULL);
  char newline = '\n';
  WriteFile(handle, &newline, 1, NULL, NULL);
}