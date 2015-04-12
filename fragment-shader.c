


/* Defining constants */

#define PATH_MAX_LIGHTS 10
#define PATH_MAX_OBJECTS 10

#define PATH_FLOAT_EPSILON 0.0001
#define PATH_INFINITY 9999999.0

#define PATH_OBJECT_SPHERE 1



/* Defining uniforms */

const int sceneNumLights = {{js:num_lights}};
const int sceneNumObjects = 1;

uniform vec2 u_screen_size;
uniform int u_frame_count;
uniform vec3 u_ambiant_color;

uniform vec3 u_lights_origin[ sceneNumLights ];
uniform vec3 u_lights_color[ sceneNumLights ];
uniform float u_lights_intensity[ sceneNumLights ];


/* Defining Structs */

struct MaterialEntity {
    vec3 diffuseColor;
    float specular;
    int shininess;
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

struct Intersection {
    bool intersect;
    float distance;
    ObjectEntity object;
    RayEntity ray;
    vec3 intersectionPoint;
    vec3 normal;
};



/* Defining raytracing functions */

vec3 sphereNormal(vec3 intersectPoint, ObjectEntity sphere) {
    return normalize(intersectPoint - sphere.origin);
}

vec3 getIntersectPoint(RayEntity ray, float dist) {
    return ray.origin + ray.direction * dist;
}

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
    intersect.intersectionPoint = getIntersectPoint(ray, distance);

    if( intersect.object.objectType == PATH_OBJECT_SPHERE ) {
        intersect.normal = sphereNormal(intersect.intersectionPoint, intersect.object);
    }

    return intersect;
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



/* Global Scene variables */
ObjectEntity sceneObjects[ 1 ]; 



/* Polymorphic Object Intersection helper */

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



/* Global Scene calc functions (Lighting/Shading mainly) */

bool lightPositionIsVisibleFrom(RayEntity pointToLightRay) {
    Intersection intersect = getClosestIntersection(pointToLightRay);

    return ! intersect.intersect;
}

vec3 getLightContribution(Intersection intersection) {

    vec3 finalAmbiantColor = u_ambiant_color;
    vec3 finalDiffuseColor = vec3(0.0, 0.0, 0.0);
    vec3 finalSpecularColor = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < sceneNumLights; i++) {
        vec3 o = u_lights_origin[i];
        vec3 c = u_lights_color[i];
        float intensity = u_lights_intensity[i];
        
        vec3 adaptedIntersectionPoint = intersection.intersectionPoint + intersection.normal * PATH_FLOAT_EPSILON;
        
        RayEntity pointToLightRay;
        pointToLightRay.origin = adaptedIntersectionPoint;
        pointToLightRay.direction = normalize(o - adaptedIntersectionPoint);

        if( lightPositionIsVisibleFrom(pointToLightRay) ) {

            MaterialEntity objectMaterial = intersection.object.material;

            float diffuseFactor = clamp(dot(intersection.normal, pointToLightRay.direction), 0.0, 1.0);

            if( diffuseFactor > PATH_FLOAT_EPSILON ) {
                vec3 diffuseColor = intensity * c * objectMaterial.diffuseColor * diffuseFactor;
                diffuseColor.r = clamp(diffuseColor.r, 0.0, 1.0);
                diffuseColor.g = clamp(diffuseColor.g, 0.0, 1.0);
                diffuseColor.b = clamp(diffuseColor.b, 0.0, 1.0);
                finalDiffuseColor += diffuseColor;
            }

            vec3 lightReflect = reflect(-pointToLightRay.direction, intersection.normal);
            float specularFactor = clamp(dot( intersection.normal, lightReflect ), 0.0, 1.0);

            if( specularFactor > PATH_FLOAT_EPSILON ) {
                finalSpecularColor += intensity * c * objectMaterial.specular * max(pow(specularFactor, float(objectMaterial.shininess)), 0.0);
            }
        }
    }

    return finalAmbiantColor + finalDiffuseColor + finalSpecularColor;
}



/* And here we go ! */

CameraEntity camera;
RayEntity baseRay;

void main() {

    /* Those settings and calculations should be done only once
       and not for each pixel. They should be out of main() but how ? */
    sceneObjects[0].objectType = PATH_OBJECT_SPHERE;
    sceneObjects[0].origin = vec3(0.0, 0.0, 2.0);
    sceneObjects[0].radius = 1.0;
    sceneObjects[0].material.diffuseColor = vec3(0.9, 0.9, 1.0);
    sceneObjects[0].material.specular = 1.0;
    sceneObjects[0].material.shininess = 400;

    camera.origin = vec3(0.0, 0.0, 0.0);
    camera.coordinateSystem[0] = vec3(1.0, 0.0, 0.0);
    camera.coordinateSystem[1] = vec3(0.0, 1.0, 0.0);
    camera.coordinateSystem[2] = vec3(0.0, 0.0, 1.0);
    camera.fov.x = 90.0;
    camera.fov.y = camera.fov.x / (u_screen_size.x / u_screen_size.y);
    camera.nearClipPlaneDist = 1.0;
    /* ---------------------------------------------------- */

    vec2 homogenousPixelPosition;
    homogenousPixelPosition.x = 2.0 * float(gl_FragCoord.x) / u_screen_size.x - 1.0;
    homogenousPixelPosition.y = 2.0 * float(gl_FragCoord.y) / u_screen_size.y - 1.0;

    vec3 viewVector;
    viewVector.x = camera.nearClipPlaneDist * homogenousPixelPosition.x * tan(camera.fov.x / 2.0);
    viewVector.y = camera.nearClipPlaneDist * homogenousPixelPosition.y * tan(camera.fov.y / 2.0);
    viewVector.z = 1.0;

    baseRay.origin = camera.origin;
    baseRay.direction = normalize(vec3(
        dot(viewVector, camera.coordinateSystem[0]),
        dot(viewVector, camera.coordinateSystem[1]),
        dot(viewVector, camera.coordinateSystem[2])
    ));

    float r = u_ambiant_color.r; 
    float g = u_ambiant_color.g; 
    float b = u_ambiant_color.b;

    Intersection closestIntersection = getClosestIntersection(baseRay);

    if( closestIntersection.intersect ) {

        vec3 color = getLightContribution(closestIntersection);

        r = color.r;
        g = color.g;
        b = color.b;
    }
    
    gl_FragColor = vec4(r, g, b, 1.0);
}