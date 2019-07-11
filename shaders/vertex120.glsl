#version 120

attribute vec3 position;
attribute vec3 barymetric;
attribute vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 f_barycentric;
varying vec3 f_frag_pos;
varying vec3 f_normal;

void main(){
    f_normal = normal;
    f_barycentric = barymetric;
    f_frag_pos = vec3(model * vec4(position, 1.0));
    gl_Position = projection * view * model * vec4(position, 1);
}
