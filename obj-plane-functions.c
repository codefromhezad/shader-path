vec3 planeNormal(vec3 intersectPoint, ObjectEntity plane) {
    return plane.normal;
}

Intersection planeIntersect(RayEntity ray, ObjectEntity plane) {
    float denom = dot(plane.normal, ray.direction);

	if( abs(denom) > PATH_FLOAT_EPSILON ) {
	    float t = dot(plane.origin - ray.origin, plane.normal) / denom;

	    if (t >= PATH_FLOAT_EPSILON) {
	    	return doesIntersect(ray, plane, t);
	    }
	}

    return dontIntersect();
}