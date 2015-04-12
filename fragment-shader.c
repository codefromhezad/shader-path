const int sceneNumLights = {{ js: num_lights }};
const int sceneNumObjects = 1;

uniform vec2 u_screen_size;
uniform int u_frame_count;
uniform vec4 u_ambiant_color;

uniform vec3 u_lights_origin[ sceneNumLights ];
uniform vec4 u_lights_color[ sceneNumLights ];
uniform float u_lights_intensity[ sceneNumLights ];



{{ js_include: defines }}
{{ js_include: structs }}
{{ js_include: intersection }}
{{ js_include: obj-sphere-functions }}




ObjectEntity sceneObjects[ 1 ]; 



{{ js_include: raytracing }}



/* And here we go ! */

CameraEntity camera;
RayEntity baseRay;

void main() {

    /* Those settings and calculations should be done only once
       and not for each pixel. They should be out of main() but how ? */
    sceneObjects[0].objectType = PATH_OBJECT_SPHERE;
    sceneObjects[0].origin = vec3(0.0, 0.0, 2.0);
    sceneObjects[0].radius = 1.0;
    sceneObjects[0].material.diffuseColor = vec4(0.9, 0.9, 1.0, 1.0);
    sceneObjects[0].material.specular = 1.0;
    sceneObjects[0].material.shininess = 400.0;

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

    vec4 finalColor = u_ambiant_color;

    Intersection closestIntersection = getClosestIntersection(baseRay);

    if( closestIntersection.intersect ) {
        finalColor = getLightContributionColor(closestIntersection);
    }
    
    gl_FragColor = finalColor;
}