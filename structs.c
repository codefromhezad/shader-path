
struct MaterialEntity {
    vec3 diffuseColor;
    float specular;
    float shininess;
    float reflection;
};

struct ObjectEntity {
    int objectType;
    vec3 origin;
    MaterialEntity material;

    // For spheres
    float radius;

    // For planes
    vec3 normal;
};

struct LightEntity {
    vec3 origin;
    vec3 color;
    float intensity;
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

struct Intersection {
    bool intersect;
    float distance;
    ObjectEntity object;
    RayEntity ray;
    vec3 intersectionPoint;
    vec3 normal;
};

Intersection dontIntersect() {
    Intersection intersect;
    intersect.intersect = false;
    return intersect;
}

Intersection doesIntersect(RayEntity ray, ObjectEntity object, float distance) {
    Intersection intersect;
    intersect.intersect = true;
    intersect.object = object;
    intersect.distance = distance;
    intersect.ray = ray;
    intersect.intersectionPoint = ray.origin + ray.direction * distance;

    return intersect;
}