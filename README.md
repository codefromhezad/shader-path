Testing stuff with Pathtracing (or maybe Raytracing first ...) and WebGL Shaders

There is also, a custom made jQuery plugin that heavily uses THREE.js to quickly
load external shader files and setup a Plane geometry to use it as a surface for 
shaders tinkering.


## THREEShaderHelper example : 

	$.THREEShaderHelper({
        debug: true,
        fragmentShaderFile: 'fragment-shader.c',
        canvasSize:         {w: 600, h: 600},
        canvasContainer:    '#container',
        onSceneInit: function() { $('.loading').remove(); },
        shaderInject: {
            num_lights: 1
        },
        uniforms: {
            u_lights_position: { type: "v3v", value: [ 
                new THREE.Vector3( 1.0, 1.0, 1.0 ) 
            ] },
            u_lights_color: { type: "v3v", value: [
                new THREE.Vector3( 1.0, 1.0, 1.0 )
            ] },
            u_lights_intensity: { type: "fv1", value: [ 
                1.0
            ] }
        }
    });


## Injecting data in shaders

I don't know how useful it will be but since we can't access arrays with a variable index in a fragment shader, I came up
with this hack.

It works like a templating language: Any occurence of "{{js:var_name}}" in the shader will be replaced with the value 
passed in shaderInject object (eg: using the previous example, "{{js:num_lights}}" will be replaced with "1" in the shader)


## Default uniforms usable in shader files / content 

* u_screen_size : {x: 600.0, y: 600.0} for a 600x600 screen
* u_frame_count : The frame number since beginning of rendering. Starts at 0. On the 956th frame, u_frameCount will equal ... 956