#version 330 core
layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 aTexCoord;
layout (location = 1) in float r_in;
layout (location = 2) in vec3 v_in;
layout (location = 3) in float birth_t_in;
// out vec2 TexCoord;
out vec3 v;
out float r;
out float t_birth;

void main(){
    //gl_Position = projection*view*model*vec4(aPos,1.0);
    gl_Position = vec4(aPos,1);
    gl_PointSize = 0.2;
    v = v_in;
    r = r_in;
    t_birth = birth_t_in;
    // TexCoord = vec2(aTexCoord.x,1.0-aTexCoord.y);
}