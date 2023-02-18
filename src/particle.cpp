#include <learnopengl/particle.h>
Particle::Particle(glm::vec3 pos_in,glm::vec3 col_in,float r_in){
    pos = pos_in;
    col = col_in;
    r = r_in;
    birth_time = glfwGetTime();
    v = glm::vec3(0.0f,0.0f,0.0f);
    active = true;
}
Particle::~Particle(){
    ;
}
Particle_Flow::~Particle_Flow(){
    ;
}

Particle_Flow::Particle_Flow(glm::vec3 pos_in,glm::vec3 col_in,glm::vec3 direction_in,float force_in,float last_time_in){
    direction = glm::normalize(direction_in);
    force = force_in;
    birth_time = glfwGetTime();
    pre_time = -1;
    last_time = last_time_in;
    pos = pos_in;
    col = col_in;
    noise = 0.5;
    // std::cout<<birth_time<<" "<<last_time<<std::endl;
    for(int i = 0;i<PARTICLE_NUM;i++){
        particles[i] = Particle(pos,col,particle_size);
        particles[i].v = glm::vec3(force*(direction.x+noise*(rand()%100)/100.0),force*(direction.y+noise*(rand()%100)/100.0),force*(direction.z+noise*(rand()%100)/100.0));
        //std::cout<<particles[i].v.x<<" "<<particles[i].v.y<<" "<<particles[i].v.z<<std::endl;
        particles[i].birth_time = birth_time + (rand()%100)/100.0*last_time;
        pos_r_v_tb[8*i] = particles[i].pos.x;
        pos_r_v_tb[8*i+1] = particles[i].pos.y;
        pos_r_v_tb[8*i+2] = particles[i].pos.z;
        pos_r_v_tb[8*i+3] = particles[i].r;
        pos_r_v_tb[8*i+4] = particles[i].v.x;
        pos_r_v_tb[8*i+5] = particles[i].v.y;
        pos_r_v_tb[8*i+6] = particles[i].v.z;
        pos_r_v_tb[8*i+7] = particles[i].birth_time;
    }
    
    // for(int i=0;i<8*PARTICLE_NUM;i++){
    //     std::cout<<pos_r_v_tb[i]<<std::endl;
    // }
}
void Particle_Flow::draw_flow(Shader &shader_,glm::mat4 &proj,glm::mat4 &view,glm::mat4 &model){
    
    shader_.use();
    shader_.set_3trans_matrix(glm::value_ptr(proj),glm::value_ptr(view),glm::value_ptr(model));
    shader_.setFloat("t_cur",glfwGetTime());
    shader_.setVec4("ourColor",glm::vec4(col, 1.0f));
    unsigned int VAO_part;
    glGenVertexArrays(1,&VAO_part);
    glBindVertexArray(VAO_part);
    

    unsigned int VBO_part;
    glGenBuffers(1,&VBO_part);
    glBindBuffer(GL_ARRAY_BUFFER,VBO_part);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8*PARTICLE_NUM,pos_r_v_tb,GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1,0,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    // glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    // glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(4*sizeof(float)));
    // glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(7*sizeof(float)));
    // glEnableVertexAttribArray(4);

    glDrawArrays(GL_POINTS,0,PARTICLE_NUM);

    glDeleteVertexArrays(1, &VAO_part);
    glDeleteBuffers(1,&VBO_part);
}
void Particle_Flow::trans_pos(glm::vec3 pos_in){
    pos = pos_in;
    for(int i=0;i<PARTICLE_NUM;i++){
        particles[i].pos = pos_in;
        pos_r_v_tb[8*i] = particles[i].pos.x;
        pos_r_v_tb[8*i+1] = particles[i].pos.y;
        pos_r_v_tb[8*i+2] = particles[i].pos.z;
        // std::cout<<pos_r_v_tb[8*i] <<" "<<pos_r_v_tb[8*i+1]<<" "<<pos_r_v_tb[8*i+2]<<std::endl;  
    }
    
}

void Particle_Flow::set_next_birth(){
    float current_time = glfwGetTime();
    // std::cout<<current_time<<" "<<birth_time<<" "<<last_time<<std::endl;
    if(current_time>=birth_time+last_time){
        birth_time = current_time;
        // std::cout<<"trans_birth"<<std::endl;
        for(int i=0;i<PARTICLE_NUM;i++){
            particles[i].birth_time = birth_time + (rand()%100)/100.0*last_time;
            pos_r_v_tb[8*i+7] = particles[i].birth_time;
        }
    }
    
}
void Particle_Flow::trans_force(float force_in){
    force = force_in;
    for(int i = 0;i<PARTICLE_NUM;i++){
        particles[i].v = glm::vec3(force*(direction.x+noise*(rand()%100)/100.0),force*(direction.y+noise*(rand()%100)/100.0),force*(direction.z+noise*(rand()%100)/100.0));
        pos_r_v_tb[8*i+4] = particles[i].v.x;
        pos_r_v_tb[8*i+5] = particles[i].v.y;
        pos_r_v_tb[8*i+6] = particles[i].v.z;
    }
}
void Particle_Flow::trans_noise(float noise_in){
    noise = noise_in;
    for(int i = 0;i<PARTICLE_NUM;i++){
        particles[i].v = glm::vec3(force*(direction.x+noise*(rand()%100)/100.0),force*(direction.y+noise*(rand()%100)/100.0),force*(direction.z+noise*(rand()%100)/100.0));
        //std::cout<<particles[i].v.x<<" "<<particles[i].v.y<<" "<<particles[i].v.z<<std::endl;
        pos_r_v_tb[8*i+4] = particles[i].v.x;
        pos_r_v_tb[8*i+5] = particles[i].v.y;
        pos_r_v_tb[8*i+6] = particles[i].v.z;
    }
}
void Particle_Flow::trans_direction(glm::vec3 direct_in){
    direction = direct_in;
    for(int i=0;i<PARTICLE_NUM;i++){
        particles[i].v = glm::vec3(force*(direction.x+noise*(rand()%100)/100.0),force*(direction.y+noise*(rand()%100)/100.0),force*(direction.z+noise*(rand()%100)/100.0));
        pos_r_v_tb[8*i+4] = particles[i].v.x;
        pos_r_v_tb[8*i+5] = particles[i].v.y;
        pos_r_v_tb[8*i+6] = particles[i].v.z;
        // std::cout<<pos_r_v_tb[8*i] <<" "<<pos_r_v_tb[8*i+1]<<" "<<pos_r_v_tb[8*i+2]<<std::endl;  
    }
    
}

void Particle_Flow::Flow(){
    // std::cout<<"enter flow"<<pre_time<<std::endl;
    // float g_y = 0.98/2;

    // float cur_time = glfwGetTime();
    // if(pre_time == -1){
    //     pre_time = cur_time;
    //     return;
    // }
    // float delta_time = cur_time-pre_time;
    // pre_time = cur_time;
    // int flag = 0;
    // valid++;
    // if(valid != 3){
    //     return;
    // }
    // valid = 0;
    // for(int i=0;i<PARTICLE_NUM;i++){
    //     //std::cout<<particles[i].birth_time<<" "<<last_time<<" "<<cur_time<<std::endl;
    //     if(particles[i].birth_time+last_time<cur_time || particles[i].birth_time>cur_time){
    //         continue;
    //     }
    //     particles[i].v.y -= g_y*delta_time;
    //     particles[i].pos.x += particles[i].v.x*delta_time;
    //     particles[i].pos.y += particles[i].v.y*delta_time;
    //     particles[i].pos.z += particles[i].v.z*delta_time;
    //     //std::cout<<particles[i].pos.x<<" "<<particles[i].pos.y<<" "<<particles[i].pos.z<<std::endl;
    //     flag = 1;
    // }
    // if(flag == 0){
    //     active = false;
    // }
        
    // }
}