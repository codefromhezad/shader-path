const int sceneNumLights = {{ var num_lights }};
const int sceneNumObjects = 3;

uniform vec2 u_screen_size;
uniform int u_frame_count;
uniform vec4 u_ambiant_color;

uniform vec3 u_lights_origin[ sceneNumLights ];
uniform vec4 u_lights_color[ sceneNumLights ];
uniform float u_lights_intensity[ sceneNumLights ];



{{ include defines }}
{{ include structs }}
{{ include obj-sphere-functions }}
{{ include obj-plane-functions }}




ObjectEntity sceneObjects[ 3 ]; 



{{ include raytracing }}



/* And here we go ! */

CameraEntity camera;
RayEntity baseRay;

void main() {

    sceneObjects[0].objectType = PATH_OBJECT_SPHERE;
    sceneObjects[0].origin = vec3(-200.0, 100.0, 400.0);
    sceneObjects[0].radius = 150.0;
    sceneObjects[0].material.diffuseColor = vec4(0.9, 0.9, 1.0, 1.0);
    sceneObjects[0].material.specular = 1.0;
    sceneObjects[0].material.shininess = 100.0;
    sceneObjects[0].material.reflection = 1.0;

    sceneObjects[1].objectType = PATH_OBJECT_SPHERE;
    sceneObjects[1].origin = vec3(200.0, 100.0, 400.0);
    sceneObjects[1].radius = 150.0;
    sceneObjects[1].material.diffuseColor = vec4(0.9, 0.9, 1.0, 1.0);
    sceneObjects[1].material.specular = 1.0;
    sceneObjects[1].material.shininess = 100.0;
    sceneObjects[1].material.reflection = 0.0;

    sceneObjects[2].objectType = PATH_OBJECT_PLANE;
    sceneObjects[2].origin = vec3(0.0, -50.0, 100.0);
    sceneObjects[2].normal = vec3(0.0, 1.0, 0.0);
    sceneObjects[2].material.diffuseColor = vec4(1.0, 0.9, 0.9, 1.0);
    sceneObjects[2].material.specular = 1.0;
    sceneObjects[2].material.shininess = 80.0;
    sceneObjects[2].material.reflection = 0.0;

    camera.origin = vec3(0.0, 200.0, 0.0);
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

    vec4 finalColor = vec4(0.0, 0.0, 0.0, 1.0);

    for(int i = 0; i < PATH_MAX_ITERATIONS; i++) {
        Intersection closestIntersection = getClosestIntersection(baseRay);

        if( closestIntersection.intersect ) {
            vec4 currentColor = getLightContributionColor(closestIntersection);

            float reflection = closestIntersection.object.material.reflection;
            finalColor += (1.0 - reflection) * currentColor;

            if( reflection > PATH_FLOAT_EPSILON ) {
                vec3 intersectionNormal = getNormal(closestIntersection);
                vec3 adaptedIntersectionPoint = closestIntersection.intersectionPoint + intersectionNormal * PATH_FLOAT_EPSILON;

                baseRay.origin = adaptedIntersectionPoint;
                baseRay.direction = reflect(closestIntersection.ray.direction, intersectionNormal);
            } else {
                break;
            }
        } else {
            break;
        }
    }
    
    gl_FragColor = finalColor;
}