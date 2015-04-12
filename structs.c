struct MaterialEntity {
    vec4 diffuseColor;
    float specular;
    float shininess;
};

struct ObjectEntity {
    int objectType;
    vec3 origin;
    float radius;
    MaterialEntity material;
};

struct RayEntity {
    vec3 origin;
    vec3 direction;
};

struct CameraEntity {
    vec3 origin;
    mat3 coordinateSystem;
    vec2 fov;
    float nearClipPlaneDist;
};
