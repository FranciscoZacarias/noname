#ifdef F_STRING_HEADER

typedef struct String8Const {
  u64 size;
  u8* str;
} String8Const;
typedef String8Const String;

typedef struct String8Node {
  struct String8Node* next;
  String8Const string;
} String8Node;

typedef struct String8List {
  String8Node* first;
  String8Node* last;
  u64 node_count;
  u64 total_size;
} String8List;

function String8Const string8(u64 size, u8* str);
#define StringLiteral(s) string8(sizeof(s)-1, (u8*)(s))

#undef F_STRING_HEADER
#endif // F_STRING_HEADER

#ifdef F_STRING_IMPLEMENTATION

function String8Const string8(u64 size, u8* str) {
  String8Const result = { size, str };
  return result;
}

#undef F_STRING_IMPLEMENTATION
#endif // F_STRING_IMPLEMENTATION