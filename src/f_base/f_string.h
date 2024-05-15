#ifndef F_STRING_H
#define F_STRING_H

typedef struct String {
  u64 size;
  u8* str;
} String;

typedef struct String_Node {
  struct String_Node* next;
  String value;
} String_Node;

typedef struct String_List {
  String_Node* first;
  String_Node* last;
  u64 node_count;
  u64 total_size;
} String_List;

#define StringLiteral(s) (String){sizeof(s)-1, (u8*)(s)}
function String string_new(u64 size, u8* str);
function String string_range(u8* first, u8* range);
function String string_pop_left(String str);
function String string_pop_right(String str);
function b32 strings_match(String a, String b);

function String_List string_split(Arena* arena, String str, String split_character);
function void string_list_push(Arena* arena, String_List* list, String str);

function b32 cast_string_to_f32(String str, f32* value);
function b32 cast_string_to_s32(String str, s32* value);
function b32 cast_string_to_b32(String str, b32* value);

function void print(String string);

#endif // F_STRING_H