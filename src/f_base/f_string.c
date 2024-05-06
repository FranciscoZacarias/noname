
function String8Const string8(u64 size, u8* str) {
  String8Const result = { size, str };
  return result;
}