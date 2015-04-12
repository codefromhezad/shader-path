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

vec4 getLightContributionColor(Intersection intersection) {

    vec4 finalAmbiantColor = u_ambiant_color;
    vec4 finalDiffuseColor = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 finalSpecularColor = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 intersectionNormal = getNormal(intersection);

    for(int i = 0; i < sceneNumLights; i++) {
        vec3 o = u_lights_origin[i];
        vec4 c = u_lights_color[i];
        float intensity = u_lights_intensity[i];
        
        vec3 adaptedIntersectionPoint = intersection.intersectionPoint + intersectionNormal * PATH_FLOAT_EPSILON;
        
        RayEntity pointToLightRay;
        pointToLightRay.origin = adaptedIntersectionPoint;
        pointToLightRay.direction = normalize(o - adaptedIntersectionPoint);

        if( lightPositionIsVisibleFrom(pointToLightRay) ) {

            MaterialEntity objectMaterial = intersection.object.material;

            float diffuseFactor = clamp(dot(intersectionNormal, pointToLightRay.direction), 0.0, 1.0);

            if( diffuseFactor > PATH_FLOAT_EPSILON ) {
                vec4 diffuseColor = intensity * diffuseFactor * c * objectMaterial.diffuseColor;
                diffuseColor = clamp(diffuseColor, 0.0, 1.0);
                
                finalDiffuseColor += diffuseColor;
            }

            vec3 lightReflect = reflect(-pointToLightRay.direction, intersectionNormal);
            float specularFactor = clamp(dot( intersectionNormal, lightReflect ), 0.0, 1.0);

            if( specularFactor > PATH_FLOAT_EPSILON ) {
                vec4 specularColor = intensity * c * objectMaterial.specular * max(pow(specularFactor, objectMaterial.shininess * intensity * intensity), 0.0);
                specularColor = clamp(specularColor, 0.0, 1.0);

                finalSpecularColor += specularColor;
            }
        }
    }

    return finalAmbiantColor + finalDiffuseColor + finalSpecularColor;
}