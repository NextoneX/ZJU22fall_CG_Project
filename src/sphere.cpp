#include "sphere.h"

Sphere::Sphere(int latitude, int longtitude, float radiance):latitude_(latitude),longtitude_(longtitude){
    data = new float[(latitude+2)*(1+2*longtitude)*3];
    ver_count = 0;
    float delta_phi = 3.141593f / (1+latitude);
    float delta_theta = 3.141593f / longtitude;
    for(int i= 0; i <=latitude+1; i++){
        float phi = i * delta_phi;
        for(int j=0 ; j<2*longtitude+1;j++){
            float theta = j * delta_theta;
            data[ver_count*3 ]   = sin(phi) * sin(theta) * radiance;
            data[ver_count*3 +1] = sin(phi) * cos(theta) * radiance;
            data[ver_count*3 +2] = cos(phi) * radiance;
            ver_count++;
        }
    }

    indices = new unsigned int[6*(latitude+1)*(longtitude*2)];
    tri_count = 0;
    int gap=0;
    while(tri_count<(latitude+1)*(longtitude*2)){
        indices[tri_count*6]   = tri_count + gap + 0;
        indices[tri_count*6+1] = tri_count + gap + 1;
        indices[tri_count*6+2] = tri_count + gap + 2*longtitude + 1;
        indices[tri_count*6+3] = tri_count + gap + 1;
        indices[tri_count*6+4] = tri_count + gap + 2*longtitude + 1;
        indices[tri_count*6+5] = tri_count + gap + 2*longtitude + 2;
        tri_count++;
        if(tri_count%(2*longtitude)==0){
            gap++;
        }
    }

    // for(int i=0;i<ver_count;i++){
    //     std::cout<<i<<":"<<data[i*3]<<" "<<data[i*3+1]<<" "<<data[i*3+2]<<std::endl;
    // }
    // for(int i=0;i<(latitude+1)*(longtitude*2);i++){
    //     std::cout<<i<<":"<<indices[i*6]<<" "<<indices[i*6+1]<<" "<<indices[i*6+2]<<" "<<indices[i*6+3]<<" "<<indices[i*6+4]<<" "<<indices[i*6+5]<<" "<<std::endl;
    // }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (latitude+2)*(1+2*longtitude)*3*sizeof(float), data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*(latitude+1)*(longtitude*2) * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Position = glm::vec3( 0.0f,  0.0f,  0.0f);
    Color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
}

Sphere::~Sphere(){
    delete [] data;
    delete [] indices;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
void Sphere::draw(Shader &shader_ ,glm::mat4 & projection, glm::mat4 &view, glm::mat4 &model){
    shader_.use();
    shader_.setVec4("ourColor",Color);
    // shader_.set4Float("ourColor",0.0f,1.0f,0.0f,1.0f);
    shader_.set_3trans_matrix(glm::value_ptr(projection),glm::value_ptr(view),glm::value_ptr(model));
    glBindVertexArray(VAO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sun.EBO);
    glDrawElements(GL_TRIANGLES, 6*(latitude_+1)*(longtitude_*2), GL_UNSIGNED_INT, 0);
    
}