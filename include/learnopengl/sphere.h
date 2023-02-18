#ifndef SPHERE_H
#define SPHERE_H
#include <glad/glad.h>
// #include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "shader.h"

class Sphere{
public:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int ver_count;
    unsigned int tri_count; 
    float* data = NULL;
    unsigned int * indices = NULL;
    int latitude_,longtitude_;
    glm::vec3 Position;
    glm::vec4 Color;

    bool switch_mode;

    Sphere(int latitude, int longtitude, float radiance);
    void draw(Shader &shader_ ,glm::mat4 & projection, glm::mat4 &view, glm::mat4 &model);
    ~Sphere();
};

#endif