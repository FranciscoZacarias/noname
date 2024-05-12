#ifndef F_STRING_H
#define F_STRING_H

typedef struct String {
  u64 size;
  u8* str;
} String;

typedef struct StringNode {
  struct StringNode* next;
  String value;
} StringNode;

typedef struct StringList {
  StringNode* first;
  StringNode* last;
  u64 node_count;
  u64 total_size;
} StringList;

#define StringLiteral(s) (String){sizeof(s)-1, (u8*)(s)}
function String string_new(u64 size, u8* str);
function String string_range(u8* first, u8* range);
function String string_pop_left(String str);
function b32 strings_match(String a, String b);

function StringList string_split(Arena* arena, String str, String split_character);
function void string_list_push(Arena* arena, StringList* list, String str);

function void print(String string);

#endif // F_STRING_H