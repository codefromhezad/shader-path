


/* Defining constants */

#define PATH_MAX_LIGHTS 10
#define PATH_MAX_OBJECTS 10

#define PATH_FLOAT_EPSILON 0.0001
#define PATH_INFINITY 9999999.0

#define PATH_OBJECT_SPHERE 1



/* Defining uniforms */

const int sceneNumLights = {{js:num_lights}};
const int sceneNumObjects = 1;

uniform vec2 u_screenSize;
uniform int u_frameCount;
uniform vec3 u_lights_position[ sceneNumLights ];
uniform vec3 u_lights_color[ sceneNumLights ];
uniform int u_lights_intensity[ sceneNumLights ];


/* Defining Structs */

struct MaterialEntity {
    vec3 baseColor;
    float diffuse;
    float specular;
    int shininess;
};

struct LightEntity {
    vec3 origin;
    vec3 color;
    float intensity;
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
    vec3 intersectionPoint;
};



/* Defining raytracing functions */

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
    intersect.intersectionPoint = getIntersectPoint(ray, distance);
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

vec3 sphereNormal(vec3 intersectPoint, ObjectEntity sphere) {
    return normalize(intersectPoint - sphere.origin);
}



/* Global Scene variables */

LightEntity sceneLights[ sceneNumLights ];
ObjectEntity sceneObjects[ 1 ]; 



/* And here we go ! */

CameraEntity camera;
RayEntity baseRay;

void main() {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

    /* Those settings and calculations should be done only once
       and not for each pixel. They should be out of main() but how ? */
    sceneObjects[0].objectType = PATH_OBJECT_SPHERE;
    sceneObjects[0].origin = vec3(0.0, 0.0, 2.0);
    sceneObjects[0].radius = 1.0;
    sceneObjects[0].material.baseColor = vec3(0.8, 0.8, 1.0);
    sceneObjects[0].material.diffuse = 0.8;
    sceneObjects[0].material.specular = 1.0;
    sceneObjects[0].material.shininess = 80;

    sceneLights[0].origin = vec3(2.0, 2.0, 1.0);
    sceneLights[0].color = vec3(1.0, 1.0, 1.0);
    sceneLights[0].intensity = 1.0;

    camera.origin = vec3(0.0, 0.0, 0.0);
    camera.coordinateSystem[0] = vec3(1.0, 0.0, 0.0);
    camera.coordinateSystem[1] = vec3(0.0, 1.0, 0.0);
    camera.coordinateSystem[2] = vec3(0.0, 0.0, 1.0);
    camera.fov.x = 90.0;
    camera.fov.y = camera.fov.x / (u_screenSize.x / u_screenSize.y);
    camera.nearClipPlaneDist = 1.0;
    /* ---------------------------------------------------- */

    vec2 homogenousPixelPosition;
    homogenousPixelPosition.x = 2.0 * float(gl_FragCoord.x) / u_screenSize.x - 1.0;
    homogenousPixelPosition.y = 2.0 * float(gl_FragCoord.y) / u_screenSize.y - 1.0;

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

    float r = 0.0;
    float g = 0.0;
    float b = 0.0;

    Intersection closestIntersection;
    closestIntersection.distance = PATH_INFINITY;
    closestIntersection.intersect = false;

    Intersection actualIntersection;
    for(int i = 0; i < sceneNumObjects; i++) {
        if( sceneObjects[i].objectType == PATH_OBJECT_SPHERE ) {
            actualIntersection = sphereIntersect(baseRay, sceneObjects[i]);
        }

        if( actualIntersection.intersect && actualIntersection.distance < closestIntersection.distance ) {
            closestIntersection = actualIntersection;
        }
    }

    if( closestIntersection.intersect ) {

        vec3 normal = sphereNormal(closestIntersection.intersectionPoint, closestIntersection.object);

        //float lightContribution = dot(normal,)
        r = 1.0;
        g = 1.0;
        b = 1.0;
    }
    
    gl_FragColor = vec4(r, g, b, 1.0);
}