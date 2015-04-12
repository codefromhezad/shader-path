vec3 sphereNormal(vec3 intersectPoint, ObjectEntity sphere) {
    return normalize(intersectPoint - sphere.origin);
}

Intersection sphereIntersect(RayEntity ray, ObjectEntity sphere) {
    float A = dot(ray.direction, ray.direction);
    vec3 originToCenterRay = ray.origin - sphere.origin;
    float B = dot(originToCenterRay, ray.direction) * 2.0;
    float C = dot(originToCenterRay, originToCenterRay) - sphere.radius * sphere.radius;

    float delta = B * B - 4.0 * A * C;

    if(delta > 0.0) {
        delta = sqrt(delta);
        float distance1 = (-B - delta) / (2.0 * A);
        float distance2 = (-B + delta) / (2.0 * A);
        
        if(distance2 > PATH_FLOAT_EPSILON) {
            if(distance1 < PATH_FLOAT_EPSILON) {
                if(distance2 < distance1) {
                    return doesIntersect(ray, sphere, distance2);
                }
            } else {
                if(distance1 < distance2) {
                    return doesIntersect(ray, sphere, distance1);
                }
            }
        }
    }

    return dontIntersect();
}