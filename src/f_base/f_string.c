function String string_new(u64 size, u8* str) {
  String result = { size, str };
  return result;
}

function String string_range(u8* first, u8* range) {
  String result = (String){(u64)(range - first), first};
  return result;
}

function String string_pop_left(String str) {
  String result = string_new(str.size-1, str.str+1);
  return result;
}

function b32 strings_match(String a, String b) {
  if (a.size != b.size) {
    return false;
  }
  for(u32 i = 0; i < a.size; i++) {
    if (a.str[i] != b.str[i]) {
      return false;
    }
  }
  return true;
}

function StringList string_split(Arena* arena, String str, String split_character) {
  StringList result = { 0 };

  if (split_character.size != 1) {
    printf("string_split expects only one character in split_character. It got %s of size %llu\n", split_character.str, split_character.size);
    Assert(0);
  }

  u8* cursor = str.str;
  u8* end   = str.str + str.size;
  for(; cursor < end; cursor++) {
    u8 byte  = *cursor;
    if (byte == split_character.str[0]) {
      string_list_push(arena, &result, string_range(str.str, cursor));
      string_list_push(arena, &result, string_range(cursor, end));
      break;
    }
  }

  return result;
}

function void string_list_push(Arena* arena, StringList* list, String str) {
  StringNode* node = (StringNode*)arena_push(arena, sizeof(StringNode));
  
  node->value = str;
  if (!list->first && !list->last) {
    list->first = node;
    list->last  = node;
  } else {
    list->last->next = node;
    list->last       = node;
  }
  list->node_count += 1;
  list->total_size += node->value.size;
}

function void print(String string) {
  os_print_string(string);
}