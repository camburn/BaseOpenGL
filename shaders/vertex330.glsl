#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 barymetric;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 f_barycentric;
out vec3 f_frag_pos;
out vec3 f_normal;

void main(){
    f_normal = normal;
    f_barycentric = barymetric;
    f_frag_pos = vec3(model * vec4(position, 1.0));
    gl_Position = projection * view * model * vec4(position, 1);
}
