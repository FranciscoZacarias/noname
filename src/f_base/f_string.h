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

//~ String Functions

#define StringLiteral(s) (String){sizeof(s)-1, (u8*)(s)}
internal String string_new(u64 size, u8* str);
internal String string_fmt(String fmt, ...);
internal String string_range(u8* first, u8* range);
internal String string_pop_left(String str);
internal String string_pop_right(String str);
internal b32 strings_match(String a, String b);

internal String_List string_split(Arena* arena, String str, String split_character);
internal void string_list_push(Arena* arena, String_List* list, String str);

internal b32 cast_string_to_f32(String str, f32* value);
internal b32 cast_string_to_s32(String str, s32* value);
internal b32 cast_string_to_b32(String str, b32* value);

//~ Char Functions

internal b32 char_is_alpha(u8 c);
internal b32 char_is_alpha_upper(u8 c);
internal b32 char_is_alpha_lower(u8 c);
internal b32 char_is_digit(u8 c);
internal b32 char_is_symbol(u8 c);
internal b32 char_is_space(u8 c);
internal u8  char_to_upper(u8 c);
internal u8  char_to_lower(u8 c);

#endif // F_STRING_H