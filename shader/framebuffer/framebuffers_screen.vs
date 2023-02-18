#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
uniform float time;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    float strength = 0.01;
    gl_Position.x += cos(time * 20) * strength;        
    gl_Position.y += cos(time * 25) * strength; 
}  