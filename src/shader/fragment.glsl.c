

const char* GET_FRAGMENT_SHADER() {
  
  return SHADER_SOURCE(
                       //////////////////////////////////////////////
                       // Fragment  Shader start
                       
                       out vec4 FragColor;
                       
                       void main() {
                         FragColor = vec4(0.9f, 1.0f, 0.9f, 1.0);
                       }
                       
                       // Fragment Shader End
                       //////////////////////////////////////////////
                       
                       );
}