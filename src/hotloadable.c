function void hotload_shader_programs(Arena* arena) {
	ArenaTemp arena_temp = arena_temp_begin(arena);

	// Default shader vertex
	String default_shader_vertex_path = StringLiteral(DEFAULT_SHADER_VERTEX);
	u64 default_shader_vertex_last_modified = os_file_get_last_modified_time(default_shader_vertex_path);
	if (DefaultShaderVertexLastModified != default_shader_vertex_last_modified) {
		OSFile default_shader_vertex = os_file_load_entire_file(arena_temp.arena, default_shader_vertex_path);

		printf("Vertex shader changed!\n");

		DefaultShaderVertexLastModified = default_shader_vertex_last_modified;
	}

	arena_temp_end(&arena_temp);
}

// Assumes OSFile is allocated
function String _file_get_next_line(OSFile file, u32* cursor) {
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

function f32 cast_string_to_f32(String str) {
	f32 result = 0.0f;
	s32 decimal_position = -1;

	for (u64 i = 0; i < str.size; i++) {
		if (str.str[i] >= '0'  && str.str[i] <= '9') {
			result = result * 10.0f + (str.str[i] - '0');
			if (decimal_position != -1) {
				decimal_position += 1;
			}
		} else if (str.str[i] == '.') {
			decimal_position = 0;
		} else {
			printf("String was casted to f32 but invalid character was found. String: %s\n", str.str);
			Assert(0);
		}
	}

	if (decimal_position != -1) {
		result = result / (f32)pow(10, decimal_position);
	}

	return result;
}

function void hotload_variables(Arena* arena) {
	ArenaTemp arena_temp = arena_temp_begin(arena);

	u64 variables_tweak_last_modified = os_file_get_last_modified_time(StringLiteral(VARIABLES_TWEAK_FILE));
	if (VariablesTweakFileLastModified == variables_tweak_last_modified) {
		return;
	}
	VariablesTweakFileLastModified = variables_tweak_last_modified;

	u64 size = os_file_size(StringLiteral(VARIABLES_TWEAK_FILE));
	OSFile file = os_file_load_entire_file(arena_temp.arena, StringLiteral(VARIABLES_TWEAK_FILE));
	if (file.size == 0) {
		printf("Variables not loaded.\n");
		return;
	}

	u32 cursor = 0;
	while (true) {
		String line = _file_get_next_line(file, &cursor);

		if (line.str[0] == '#') {
			continue;
		}

		StringList list = string_split(arena_temp.arena, line, StringLiteral(":"));
		String key   = list.first->value;
		String value = string_pop_left(list.last->value);

		if (strings_match(key, StringLiteral("camera_speed"))) {
			f32 parsed_value = cast_string_to_f32(value);
			CAMERA_SPEED = parsed_value;
		}

		if (cursor >= file.size) {
			break;
		}
	}

	printf("Variables.hotload loaded!\n");

	arena_temp_end(&arena_temp);
}