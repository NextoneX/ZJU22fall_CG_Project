#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;   
        
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}