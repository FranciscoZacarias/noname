#define SHADER_SOURCE(code) "#version 330 core\n" #code

//////////////////////////////////////////////
// Vertex Shader
function const char* GET_VERTEX_SHADER() {
	return SHADER_SOURCE(
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec3 aColor;

		out vec3 line_color;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main() {
			gl_Position = projection * view * model * vec4(aPos, 1.0);
			line_color = aColor;
		}
	); // END: SHADER_SOURCE
}

//////////////////////////////////////////////
// Fragment Shader
function const char* GET_FRAGMENT_SHADER() {
	return SHADER_SOURCE(
		out vec4 FragColor;

		uniform vec3 color;

		void main() {
			FragColor = vec4(color, 1.0f);
		}
	); // END: SHADER_SOURCE
}

//////////////////////////////////////////////
// Screen Vertex Shader
function const char* GET_SCREEN_VERTEX_SHADER() {
	return SHADER_SOURCE(
		layout (location = 0) in vec2 aPos;

		void main() {
			gl_Position = vec4(aPos, 0.0, 1.0);
		}
	); // END: SHADER_SOURCE
}

//////////////////////////////////////////////
// Screen Fragment Shader
function const char* GET_SCREEN_FRAGMENT_SHADER() {
	return SHADER_SOURCE(
		out vec4 FragColor;

		uniform sampler2D screen_texture;
		uniform int window_width;
		uniform int window_height;

		void main() {
			vec2 tex_coords = gl_FragCoord.xy / vec2(window_width, window_height);
			FragColor = texture(screen_texture, tex_coords);
		}
	); // END: SHADER_SOURCE
}

function const char* GET_FRAGMENT_SHADER_LINE_COLOR_FROM_VERTEX() {
	return SHADER_SOURCE(
		out vec4 FragColor;
		in vec3 line_color;

		void main() {
			FragColor = vec4(line_color, 1.0f);
		}
	); // END: SHADER_SOURCE
}
