#pragma vs
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;
out vec3 v_normal;
out vec4 v_color;
out vec3 fragment_position;
void main() {
    mat4 worldview = view * world;
    v_normal = transpose(inverse(mat3(worldview))) * normal;
    v_color = color;
    // TODO check if world is correct to use below, original said model
    fragment_position = vec3(world * vec4(position, 1.0));
    // fragment_position = position;
    // gl_Position = proj * worldview * vec4(position, 1.0);
    gl_Position = world * view * proj * vec4(position, 1.0);
};
#pragma end

#pragma fs
#version 330 core
in vec3 v_normal;
in vec4 v_color;
in vec3 fragment_position;
struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform vec3 view_position;
uniform Light light;
// const vec3 LIGHT = vec3(1.0, 1.0, 1.0);
void main() {
    vec3 norm = normalize(v_normal);
    vec3 light_direction = normalize(light.position - fragment_position);
    vec3 view_direction = normalize(view_position - fragment_position);
    vec3 reflection_direction = reflect(-light_direction, norm);
    vec3 ambient = light.ambient; 
    vec3 diffuse = light.diffuse * max(dot(norm, light_direction), 0.0);
    vec3 specular = light.specular * pow(max(dot(view_direction, reflection_direction), 0.0), 32);
    gl_FragColor = vec4((ambient + diffuse + specular), 1.0) * v_color;
};

#pragma end
