

const char* GET_VERTEX_SHADER() {
  
  return SHADER_SOURCE(
                       //////////////////////////////////////////////
                       // Vertex Shader start
                       
                       layout (location = 0) in vec3 Pos;
                       
                       void main() {
                         gl_Position = vec4(Pos.x, Pos.y, Pos.z, 1.0);
                       }
                       
                       // Vertex Shader end
                       //////////////////////////////////////////////
                       
                       );
}