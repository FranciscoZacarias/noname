/* date = January 28th 2024 8:00 pm */

typedef enum _ShaderErrorType {
	_ShaderErrorType_LinkProgram,
	_ShaderErrorType_CompileVertex,
	_ShaderErrorType_CompileFragment
} _ShaderErrorType;

function void _shader_check_errors(u32 shader, _ShaderErrorType shader_type) {
	int  success;
	char infoLog[1024];
	switch(shader_type) {
		case _ShaderErrorType_LinkProgram: {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if(!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				printf("Error %d linking shader program. Log: %s", success, infoLog);
				return;
			}
		} break;
		case _ShaderErrorType_CompileVertex:
		case _ShaderErrorType_CompileFragment: {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				printf("Error %d compiling vertex shader. Log: %s", success, infoLog);
				return;
			}
		} break;
	}
}

function Shader shader_create(const char* vertex_source, const char* fragment_source) {
	u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);
	_shader_check_errors(vertex_shader, _ShaderErrorType_CompileVertex);

	u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(fragment_shader);
	_shader_check_errors(fragment_shader, _ShaderErrorType_CompileFragment);

	Shader result = { 0 };
	result.id = glCreateProgram();
	glAttachShader(result.id, vertex_shader);
	glAttachShader(result.id, fragment_shader);
	glLinkProgram(result.id);
	_shader_check_errors(result.id, _ShaderErrorType_LinkProgram);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return result;
}

function void shader_use(Shader shader) {
	glUseProgram(shader.id);
}

function void shader_set_uniform_mat4fv(Shader shader, const char* uniform, Mat4f32 mat) {
	s32 uniform_location = glGetUniformLocation(shader.id, uniform);
	if (uniform_location == -1) {
		printf("Uniform %s not found\n", uniform);
	}
	glUniformMatrix4fv(uniform_location, 1, 1, &mat.data[0][0]);
}

function void shader_set_uniform_vec4fv(Shader shader, const char* uniform, Vec4f32 vec) {
	s32 uniform_location = glGetUniformLocation(shader.id, uniform);
	if (uniform_location == -1) {
		printf("Uniform %s not found\n", uniform);
	}
	glUniform4fv(uniform_location, 1, &vec.data[0]);
}

function void shader_set_uniform_vec3fv(Shader shader, const char* uniform, Vec3f32 vec) {
	s32 uniform_location = glGetUniformLocation(shader.id, uniform);
	if (uniform_location == -1) {
		printf("Uniform %s not found\n", uniform);
	}
	glUniform3fv(uniform_location, 1, &vec.data[0]);
}