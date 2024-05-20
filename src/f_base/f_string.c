internal String string_new(u64 size, u8* str) {
  String result = { size, str };
  return result;
}

internal String string_fmt(String fmt, ...) {
  
}

internal String string_range(u8* first, u8* range) {
  String result = (String){(u64)(range - first), first};
  return result;
}

internal String string_pop_left(String str) {
  String result = string_new(str.size-1, str.str+1);
  return result;
}

internal String string_pop_right(String str) {
  String result = string_new(str.size-1, str.str);
  return result;
}

internal b32 strings_match(String a, String b) {
  if (a.size != b.size) {
    return 0;
  }
  for(u32 i = 0; i < a.size; i++) {
    if (a.str[i] != b.str[i]) {
      return 0;
    }
  }
  return 1;
}

internal String_List string_split(Arena* arena, String str, String split_character) {
  String_List result = { 0 };
  
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

internal void string_list_push(Arena* arena, String_List* list, String str) {
  String_Node* node = (String_Node*)arena_push(arena, sizeof(String_Node));
  
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

internal b32 cast_string_to_b32(String str, b32* value) {
	b32 result = 1;
	if (strings_match(str, StringLiteral("false"))) {
		*value = 0;
	} else if (strings_match(str, StringLiteral("true"))) {
		*value = 1;
	} else {
		result = 0;
	}
	return result;
}

internal b32 cast_string_to_f32(String str, f32* value) {
	*value = 0.0f;
	s32 decimal_position = -1;
  
	for (u64 i = 0; i < str.size; i++) {
		if (str.str[i] >= '0'  && str.str[i] <= '9') {
			*value = *value * 10.0f + (str.str[i] - '0');
			if (decimal_position != -1) {
				decimal_position += 1;
			}
		} else if (str.str[i] == '.') {
			decimal_position = 0;
		} else {
			return 0;
		}
	}
  
	if (decimal_position != -1) {
		*value = *value / (f32)pow(10, decimal_position);
	}
  
	return 1;
}

internal b32 cast_string_to_s32(String str, s32* value) {
	*value = 0.0f;
	for (u64 i = 0; i < str.size; i++) {
		if (str.str[i] >= '0'  && str.str[i] <= '9') {
			*value = *value * 10.0f + (str.str[i] - '0');
		} else {
			return 0;
		}
	}
	return 1;
}

internal b32 char_is_alpha(u8 c) {
  return char_is_alpha_upper(c) || char_is_alpha_lower(c);
}

internal b32 char_is_alpha_upper(u8 c) {
  return c >= 'A' && c <= 'Z';
}

internal b32 char_is_alpha_lower(u8 c) {
  return c >= 'a' && c <= 'z';
}

internal b32 char_is_digit(u8 c) {
  return c >= '1' && c <= '9';
}

internal b32 char_is_symbol(u8 c) {
  return (c == '~' || c == '!'  || c == '$' || c == '%' || c == '^' ||
          c == '&' || c == '*'  || c == '-' || c == '=' || c == '+' ||
          c == '<' || c == '.'  || c == '>' || c == '/' || c == '?' ||
          c == '|' || c == '\\' || c == '{' || c == '}' || c == '(' ||
          c == ')' || c == '\\' || c == '[' || c == ']' || c == '#' ||
          c == ',' || c == ';'  || c == ':' || c == '@');
}

internal b32 char_is_space(u8 c) {
  return c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\v' || c == '\n';
}

internal u8  char_to_upper(u8 c) {
  return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

internal u8  char_to_lower(u8 c) {
  return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'A')) : c;
}