function void hotload_shader_programs(Arena* arena, Renderer* renderer) {
	if (FirstEntry) {
		DefaultVertexShaderLastModified   = os_file_get_last_modified_time(StringLiteral(DEFAULT_VERTEX_SHADER));
		DefaultFragmentShaderLastModified = os_file_get_last_modified_time(StringLiteral(DEFAULT_FRAGMENT_SHADER));
		ScreenVertexShaderLastModified    = os_file_get_last_modified_time(StringLiteral(SCREEN_VERTEX_SHADER));
		ScreenFragmentShaderLastModified  = os_file_get_last_modified_time(StringLiteral(SCREEN_FRAGMENT_SHADER));
		FirstEntry = false;
		return;
	}

	Arena_Temp arena_temp = arena_temp_begin(arena);

	// Default shader 
	{
		u64 default_vertex_shader_last_moditifed  = os_file_get_last_modified_time(StringLiteral(DEFAULT_VERTEX_SHADER));
		u64 default_fragment_shader_last_modified = os_file_get_last_modified_time(StringLiteral(DEFAULT_FRAGMENT_SHADER));

		if (DefaultVertexShaderLastModified   != default_vertex_shader_last_moditifed || 
				DefaultFragmentShaderLastModified != default_fragment_shader_last_modified) {

			renderer_recompile_default_shader(arena, renderer);

			DefaultVertexShaderLastModified   = default_vertex_shader_last_moditifed;
			DefaultFragmentShaderLastModified = default_fragment_shader_last_modified;
		}
	}

	// Screen shader
	{
		u64 screen_vertex_shader_last_moditifed = os_file_get_last_modified_time(StringLiteral(SCREEN_VERTEX_SHADER));
		u64 screen_fragment_shader_last_modified = os_file_get_last_modified_time(StringLiteral(SCREEN_FRAGMENT_SHADER));

		if (ScreenVertexShaderLastModified   != screen_vertex_shader_last_moditifed || 
				ScreenFragmentShaderLastModified != screen_fragment_shader_last_modified) {

			renderer_recompile_screen_shader(arena, renderer);

			ScreenVertexShaderLastModified   = screen_vertex_shader_last_moditifed;
			ScreenFragmentShaderLastModified = screen_fragment_shader_last_modified;
		}
	}

	arena_temp_end(&arena_temp);
}

// Assumes OS_File is allocated
function String _file_get_next_line(OS_File file, u32* cursor) {
	String result;
	result.size = 0;
	result.str  = file.data + *cursor;
	while (true) {
		if (*(file.data + *cursor) == '\n' || *cursor >= file.size) {
			++(*cursor);		
			break;
		}
		++result.size;
		++(*cursor);
	}
	if (result.str[result.size-1] == '\r') {
		--result.size;
	}
	return result;
}

function f32 cast_string_to_f32(String str, f32* value) {
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
			return false;
		}
	}

	if (decimal_position != -1) {
		*value = *value / (f32)pow(10, decimal_position);
	}

	return true;
}

function void hotload_variables(Arena* arena) {
	Arena_Temp arena_temp = arena_temp_begin(arena);

	u64 variables_tweak_last_modified = os_file_get_last_modified_time(StringLiteral(VARIABLES_TWEAK_FILE));
	if (VariablesTweakFileLastModified == variables_tweak_last_modified) {
		return;
	}
	VariablesTweakFileLastModified = variables_tweak_last_modified;

	u64 size = os_file_size(StringLiteral(VARIABLES_TWEAK_FILE));
	OS_File file = os_file_load_entire_file(arena_temp.arena, StringLiteral(VARIABLES_TWEAK_FILE));
	if (file.size == 0) {
		printf("Variables not loaded.\n");
		return;
	}

	u32 cursor = 0;
	u32 line_count = 0;
	while (true) {
		line_count += 1;
		String line = _file_get_next_line(file, &cursor);

		if (line.str[0] == '#') {
			continue;
		}

		String_List list = string_split(arena_temp.arena, line, StringLiteral(":"));
		String key   = list.first->value;
		String value = string_pop_left(list.last->value);

		while(value.size > 0 && value.str[0] == ' ') {
			value = string_pop_left(value);
		}

		if (strings_match(key, StringLiteral("camera_speed"))) {
			f32 parsed_value;
			if (!cast_string_to_f32(value, &parsed_value)) {
				printf("Parsing error. Line: %lu. Value: '%s' :: %s.\n \n", line_count, value.str, VARIABLES_TWEAK_FILE);
				break;
			}
			CAMERA_SPEED = parsed_value;
		}

		if (cursor >= file.size) {
			break;
		}
	}

	printf("Variables.hotload loaded!\n");

	arena_temp_end(&arena_temp);
}