
global u64 Win32TicksOerSec = 1;
global u32 Win32ThreadContextIndex;

function void os_init(void) {
  LARGE_INTEGER perf_freq = {0};
  if (QueryPerformanceFrequency(&perf_freq)) {
      Win32TicksOerSec = ((u64)perf_freq.HighPart << 32) | perf_freq.LowPart;
  }
  timeBeginPeriod(1);
	Win32ThreadContextIndex = TlsAlloc();
}

function void* os_memory_reserve(u64 size) {
  void* result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
  return result;
}

function b32 os_memory_commit(void* memory, u64 size) {
  b32 result = (VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

function void  os_memory_decommit(void* memory, u64 size) {
  VirtualFree(memory, size, MEM_DECOMMIT);
}

function void  os_memory_release(void* memory, u64 size) {
  VirtualFree(memory, 0, MEM_RELEASE);
}

function void  os_thread_context_set(void* ctx) {
	TlsSetValue(Win32ThreadContextIndex, ctx);
}

function void* os_thread_context_get() {
	return TlsGetValue(Win32ThreadContextIndex);
}
