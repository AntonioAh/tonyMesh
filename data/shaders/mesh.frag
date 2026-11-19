#version 410 core
in vec3 vNormal;

uniform vec3 color;

out vec4 fragColor;

void main(){
    vec3 n = normalize(vNormal);
    const vec3 lightDir = vec3(0.0, 0.0, 1.0);
    float diffuse = abs(dot(n, lightDir));
    const float ambient = 0.15;
    fragColor = vec4(color * (ambient + (1.0-ambient)*diffuse), 1.0);
}
