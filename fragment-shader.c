uniform vec2 u_screenSize;

struct SphereObject {
    vec3 position;
    float radius;
};

struct Ray {
    vec3 position;
    vec3 direction;
};

SphereObject sphere;

void main() {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

    sphere.position = vec3(300.0, 300.0, 2.0);
    sphere.radius = 20.0;

    float sx = float(gl_FragCoord.x) - sphere.position.x;
    float sy = float(gl_FragCoord.y) - sphere.position.y;

    if( sqrt( sx * sx + sy * sy ) < sphere.radius ) {
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
}