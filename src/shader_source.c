#define SHADER_SOURCE(code) "#version 330 core\n" #code

//////////////////////////////////////////////
// Vertex Shader
function const char* GET_VERTEX_SHADER() {
	return SHADER_SOURCE(
			layout (location = 0) in vec3 aPos;
			layout (location = 1) in vec3 aColor;

			out vec3 axis_color;

			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 projection;

			void main() {
				gl_Position = projection * view * model * vec4(aPos, 1.0);
				axis_color = aColor;
			}
		);
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
		);
}

function const char* GET_FRAGMENT_SHADER_COLOR_FROM_VERTEX() {
	return SHADER_SOURCE(
			out vec4 FragColor;
			in vec3 axis_color;

			void main() {
		    FragColor = vec4(axis_color, 1.0f);
			}
		);
}
