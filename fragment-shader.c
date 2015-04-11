uniform vec2 u_screenSize;

struct SphereEntity {
    vec3 origin;
    float radius;
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

SphereEntity sphere;
CameraEntity camera;
RayEntity baseRay;

void main() {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

    /* Those settings and calculations should be done only once
       and not for each pixel. They should be out of main() but how ? */
    sphere.origin = vec3(0.0, 2.0, 2.0);
    sphere.radius = 20.0;

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

    float r = (baseRay.direction.x + 1.0) / 2.0;
    float g = (baseRay.direction.y + 1.0) / 2.0;
    float b = (baseRay.direction.z + 1.0) / 2.0;
    
    gl_FragColor = vec4(r, g, b, 1.0);
}