#version 330 core
#define g 0.98
#define last_time 30
layout (points) in;
layout (triangle_strip,max_vertices = 40) out;

in float r[];
uniform float t_cur;
in float t_birth[];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
in vec3 v[];

void main(){
    float delta_t = t_cur - t_birth[0];
    if(delta_t>=0 && delta_t <= last_time){
        vec4 centre = gl_in[0].gl_Position;
        float _x = v[0].x*(delta_t);
        float _y = v[0].y*(delta_t)-0.5*g*(delta_t)*(delta_t);
        float _z = v[0].z*(delta_t);
        vec4 translate = vec4(_x,_y,_z,0);

        float ra = 0.01;
        int d = 20;
        float delta_alpha = 2*3.1415926535/d;
        float alpha = 0;
        for(int i = 0;i<2*d+1;i++){
            // if(i%2==0){
                gl_Position = (projection*view*model*(centre+translate)+vec4(cos(alpha)*ra,sin(alpha)*ra,0,0));
                EmitVertex();
                alpha += delta_alpha;
            // }else{
                gl_Position = (projection*view*model*(centre+translate));
                EmitVertex();
            // }
        }
        EndPrimitive();
    }
}
