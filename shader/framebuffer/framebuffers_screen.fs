#version 330 core
out vec4 color;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2      offsets[9];
uniform float     blur_kernel[9];
void main()
{
    // vec3 col = texture(screenTexture, TexCoords).rgb;
    // color = vec4(col, 1.0);
    color = vec4(0.0f);
    vec3 sample[9];
    for(int i = 0; i < 9; i++)
        sample[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    for(int i = 0; i < 9; i++)
        color += vec4(sample[i] * blur_kernel[i], 0.0f);
    color.a = 1.0f;
}