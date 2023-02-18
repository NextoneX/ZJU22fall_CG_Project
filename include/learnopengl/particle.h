#ifndef PARTICLE_H
#define PARTICLE_H
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sphere.h"
#include <vector>

#define PARTICLE_NUM 4000
#define particle_size 1

#define pos_ glm::vec3(0.0f,0.0f,0.0f)
#define col_ glm::vec3(0.0f,0.0f,0.0f)

class Particle{
public:
    glm::vec3 pos;
    glm::vec3 col;
    // glm::vec3 direction;
    // glm::vec3 gravity;
    float r;//radius
    float birth_time;
    glm::vec3 v;
    bool active;

    // float fade;
    // bool active;
    Particle(glm::vec3 pos_in = pos_ ,glm::vec3 col_in = col_ ,float r_in = 0.01);
    void setpos(glm::vec3 pos_in);
    void setdirection(glm::vec3 direct_in);
    ~Particle();
};



class Particle_Flow{
public:
    // std::vector<Particle>particles;
    Particle particles[PARTICLE_NUM];
    glm::vec3 pos;
    glm::vec3 col;
    glm::vec3 direction;
    float noise;
    float birth_time;
    float last_time;//持续时间
    float pre_time;//计算delta
    float force;
    float pos_r_v_tb[PARTICLE_NUM*8];
    // unsigned int VAO;
    // unsigned int VBO;
    Particle_Flow(glm::vec3 pos,glm::vec3 col,glm::vec3 direction,float force,float last_time);
    void draw_flow(Shader &shader_,glm::mat4 &proj,glm::mat4 &view,glm::mat4 &model);
    void Flow();//没用
    void trans_pos(glm::vec3 pos_in);
    void trans_force(float force_in);
    void trans_direction(glm::vec3 direct_in);
    void trans_noise(float noise_in);
    void set_next_birth();
    ~Particle_Flow();
};



#endif