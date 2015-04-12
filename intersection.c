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