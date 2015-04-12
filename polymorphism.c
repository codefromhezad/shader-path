Intersection getClosestIntersection(RayEntity ray) {
    Intersection closestIntersection = dontIntersect();
    closestIntersection.distance = PATH_INFINITY;

    Intersection actualIntersection;
    for(int i = 0; i < sceneNumObjects; i++) {
        if( sceneObjects[i].objectType == PATH_OBJECT_SPHERE ) { 
            actualIntersection = sphereIntersect(ray, sceneObjects[i]);
        }

        if( actualIntersection.intersect && actualIntersection.distance < closestIntersection.distance ) {
            closestIntersection = actualIntersection;
        }
    }

    return closestIntersection;
}

vec3 getNormal(Intersection intersect) {
    if( intersect.object.objectType == PATH_OBJECT_SPHERE ) {
        return sphereNormal(intersect.intersectionPoint, intersect.object);
    }
    return NULL_VECTOR;
}