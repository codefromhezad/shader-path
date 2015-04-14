Intersection getClosestIntersection(RayEntity ray) {
    Intersection closestIntersection = dontIntersect();
    closestIntersection.distance = PATH_INFINITY;

    Intersection actualIntersection;
    for(int i = 0; i < sceneNumObjects; i++) {
        if( sceneObjects[i].objectType == PATH_OBJECT_SPHERE ) { 
            actualIntersection = sphereIntersect(ray, sceneObjects[i]);
        }

        if( sceneObjects[i].objectType == PATH_OBJECT_PLANE ) { 
            actualIntersection = planeIntersect(ray, sceneObjects[i]);
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

    if( intersect.object.objectType == PATH_OBJECT_PLANE ) {
        return planeNormal(intersect.intersectionPoint, intersect.object);
    }

    return NULL_VECTOR;
}

bool lightPositionIsVisibleFrom(RayEntity pointToLightRay) {
    Intersection intersect = getClosestIntersection(pointToLightRay);

    return ! intersect.intersect;
}

vec3 getLightContributionColor(Intersection intersection) {

    vec3 finalAmbiantColor = u_ambiant_color;
    vec3 finalDiffuseColor = vec3(0.0, 0.0, 0.0);
    vec3 finalSpecularColor = vec3(0.0, 0.0, 0.0);

    vec3 intersectionNormal = getNormal(intersection);
    vec3 adaptedIntersectionPoint = intersection.intersectionPoint + intersectionNormal * PATH_FLOAT_EPSILON;

    RayEntity pointToLightRay;
    pointToLightRay.origin = adaptedIntersectionPoint;

    for(int i = 0; i < sceneNumLights; i++) {
        vec3 o = u_lights[i].origin;
        vec3 c = u_lights[i].color;
        float intensity = u_lights[i].intensity;
        
        pointToLightRay.direction = normalize(o - adaptedIntersectionPoint);

        if( lightPositionIsVisibleFrom(pointToLightRay) ) {

            float diffuseFactor = clamp(dot(intersectionNormal, pointToLightRay.direction), 0.0, 1.0);

            if( diffuseFactor > PATH_FLOAT_EPSILON ) {
                vec3 diffuseColor = intensity * diffuseFactor * c * intersection.object.material.diffuseColor;
                diffuseColor = clamp(diffuseColor, 0.0, 1.0);
                
                finalDiffuseColor = finalDiffuseColor + diffuseColor;
            }

            vec3 lightReflect = reflect(-pointToLightRay.direction, intersectionNormal);
            float specularFactor = clamp(dot( intersectionNormal, lightReflect ), 0.0, 1.0);

            if( specularFactor > PATH_FLOAT_EPSILON ) {
                vec3 specularColor = intensity * c * intersection.object.material.specular * pow(specularFactor, intersection.object.material.shininess);
                specularColor = clamp(specularColor, 0.0, 1.0);

                finalSpecularColor = finalSpecularColor + specularColor;
            }
        }
    }

    return finalAmbiantColor + finalDiffuseColor + finalSpecularColor;
}