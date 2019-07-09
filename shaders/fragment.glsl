#version 330 core

in vec3 f_barycentric;

out vec4 color;

uniform vec4 u_color;

uniform float wire_width = 1.5;


float edgeFactor(){
    vec3 d = fwidth(f_barycentric);
    vec3 a3 = smoothstep(vec3(0.0), d*wire_width, f_barycentric);
    return min(min(a3.x, a3.y), a3.z);
}

void main(){
    //color = vec4(f_barycentric, 1.0);
    color = vec4(u_color * edgeFactor());
    
}