#version 120

varying vec3 f_barycentric;
varying vec3 f_frag_pos;
varying vec3 f_normal;

uniform vec4 u_color;

uniform float wire_width = 1.5;
uniform vec3 light_pos = vec3(1.0, 5.0, 1.0);
uniform vec3 light_color = vec3(1.0, 1.0, 1.0);
uniform vec3 ambient = vec3(0.25, 0.25, 0.25);


float edgeFactor(){
    vec3 d = fwidth(f_barycentric);
    vec3 a3 = smoothstep(vec3(0.0), d*wire_width, f_barycentric);
    return min(min(a3.x, a3.y), a3.z);
}

void main(){

    vec3 norm = normalize(f_normal);
    vec3 light_dir = normalize(light_pos - f_frag_pos);

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    // Calculate color based on normal direction
    float steepness = acos(dot(norm, vec3(0.0f, 1.0f, 0.0f)));

    vec3 out_color = vec3(0.0f, 0.0f, 0.0f);
    if (steepness > 0.9) {
        out_color =  vec3(0.3, 0.3, 0.3);
    } else if (steepness > 0.5f) {
        out_color =  vec3(0.1, 0.5, 0.1);
    } else {
        out_color =  vec3(0.05, 0.2, 0.05);
    }

    if (f_frag_pos.y > 1.5) {
        out_color =  vec3(0.8, 0.8, 0.8);
    }

    if (f_frag_pos.y < 0.3) {
        out_color =  vec3(0.3, 0.3, 0.8);
    }
    

    vec3 result = (ambient + diffuse) * out_color;

    gl_FragColor = vec4(result, 1.0);
    //color = vec4(norm, 1.0);
    //color = vec4(u_color * edgeFactor());
    
}
