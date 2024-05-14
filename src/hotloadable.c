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

function b32 cast_string_to_b32(String str, b32* value) {
	b32 result = true;
	if (strings_match(str, StringLiteral("false"))) {
		*value = false;
	} else if (strings_match(str, StringLiteral("true"))) {
		*value = true;
	} else {
		result = false;
	}
	return result;
}

function b32 cast_string_to_f32(String str, f32* value) {
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

function b32 cast_string_to_s32(String str, s32* value) {
	*value = 0.0f;
	for (u64 i = 0; i < str.size; i++) {
		if (str.str[i] >= '0'  && str.str[i] <= '9') {
			*value = *value * 10.0f + (str.str[i] - '0');
		} else {
			return false;
		}
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

		if (line.size == 0 || line.str[0] == '#') {
			continue;
		}

		// Parse section
		if (line.str[0] == '\\') {
			line = string_pop_left(line);
			if (strings_match(line, StringLiteral("Startup"))) {
				if (StartupVariablesLoaded) {
					// Exhaust lines until next header
					while (true) {
						line = _file_get_next_line(file, &cursor);
						if (line.str[0] == '\\') {
							break;
						} else if (cursor >= file.size) {
							printf("Variables.hotload loaded!\n");
							break;
						}
					}
				} else {
					StartupVariablesLoaded = true;
				}
			}
			continue;
		}

		// Parse variables
		String_List list = string_split(arena_temp.arena, line, StringLiteral(":"));
		String key   = list.first->value;
		String value = string_pop_left(list.last->value);

		while(value.size > 0 && value.str[0] == ' ') {
			value = string_pop_left(value);
		}

		if (strings_match(key, StringLiteral("camera_speed"))) {
			f32 parsed_value;
			if (!cast_string_to_f32(value, &parsed_value)) {
				printf("Error parsing f32. Line: %lu. Value: '%s' :: %s.\n \n", line_count, value.str, VARIABLES_TWEAK_FILE);
				continue;
			}
			HotloadableCameraSpeed = parsed_value;
		} else if (strings_match(key, StringLiteral("cube_border_thickness"))) {
			f32 parsed_value;
			if (!cast_string_to_f32(value, &parsed_value)) {
				printf("Error parsing f32. Line: %lu. Value: '%s' :: %s.\n \n", line_count, value.str, VARIABLES_TWEAK_FILE);
				continue;
			}
			HotloadableCubeBorderThickness = parsed_value * 0.01;
		} else if (strings_match(key, StringLiteral("wireframe_mode"))) {
			b32 parsed_value;
			if (!cast_string_to_b32(value, &parsed_value)) {
				printf("Error parsing b32. Line: %lu. Value: '%s' :: %s.\n \n", line_count, value.str, VARIABLES_TWEAK_FILE);
				continue;
			}
			HotloadableEnableWireframeMode = parsed_value;
		} else if (strings_match(key, StringLiteral("enable_culling"))) {
			b32 parsed_value;
			if (!cast_string_to_b32(value, &parsed_value)) {
				printf("Error parsing b32. Line: %lu. Value: '%s' :: %s.\n \n", line_count, value.str, VARIABLES_TWEAK_FILE);
				continue;
			}
			HotloadableEnableCulling = parsed_value;
		} else if (strings_match(key, StringLiteral("window_width"))) {
			s32 parsed_value;
			if (!cast_string_to_s32(value, &parsed_value)) {
				printf("Error parsing s32. Line: %lu. Value: '%s' :: %s.\n \n", line_count, value.str, VARIABLES_TWEAK_FILE);
				continue;
			}
			WindowWidth = parsed_value;
		} else if (strings_match(key, StringLiteral("window_height"))) {
			s32 parsed_value;
			if (!cast_string_to_s32(value, &parsed_value)) {
				printf("Error parsing s32. Line: %lu. Value: '%s' :: %s.\n \n", line_count, value.str, VARIABLES_TWEAK_FILE);
				continue;
			}
			WindowHeight = parsed_value;
		} else {
			printf("Variable not loaded: "); print(line);
		}

		if (cursor >= file.size) {
			printf("Variables.hotload loaded!\n");
			break;
		}
	}

	arena_temp_end(&arena_temp);
}