# Testing stuff with Pathtracing (or maybe Raytracing first ...) and WebGL Shaders

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
                animate: false,
                shaderInject: {
                    num_lights: 2,
                    num_objects: 3
                },
                shaderIncludeFiles: {
                    'structs':      'structs.c',
                    'defines':      'defines.c',
                    'raytracing':   'raytracing.c',
                    'obj-sphere-functions':  'obj-sphere-functions.c',
                    'obj-plane-functions':   'obj-plane-functions.c',
                },
                uniforms: {
                    u_ambiant_color: { type: "v3", value: AMBIANT_COLOR },
                    u_lights: [
                        {
                            origin: { type: "v3", value: new THREE.Vector3( - 300.0, 300.0, 100.0 ) },
                            color: { type: "v3", value: new THREE.Vector3( 1.0, 1.0, 1.0 ) },
                            intensity: { type: "f", value: 1.0 }
                        },
                        {
                            origin: { type: "v3", value: new THREE.Vector3( 350.0, 300.0, 20.0 ) },
                            color: { type: "v3", value: new THREE.Vector3( 1.0, 0.8, 0.8 ) },
                            intensity: { type: "f", value: 0.5 }
                        }
                    ]
                }
            });


## Including/Specifying uniforms

The uniforms options is very powerful thanks to the buildSerializeUniformsObject helper function you'll find in the jquery.three-shader-helper.js file.

Basically, any well constructed JS Array of objects or array of array of objects (etc ... The function is recursive up to a depth of 5 calls) following the logic of a shader struct will be transformed to a set of uniforms.

These uniforms and corresponding structs still have to be defined in the shader but you this avoid having to use the next (horrible) notation to pass struct baseed uniform data :

    '\_u\_lights[0].color[0]': r_value,
    '\_u\_lights[0].color[1]': g_value,
    '\_u\_lights[0].color[2]': b_value,

## Including external code into shaders (imitating C's #include statements)

Any occurence of "{{ include _include\_name_ }}" in the shader will be replaced by the contents of the file identified by _include\_name_ in the shaderIncludeFiles option attributes.



## Injecting variables into shaders

Any occurence of "{{ var _var\_name_ }}" in the shader will be replaced with the value identified by _var\_name_ in the shaderInject option attributes.



## Shader injection Example
	
	{{ include structs }} // Will be replaced with contents from the file referenced by "structs" (In the previous example: structs.c)

	const int sceneNumLights = {{ var num_lights }}; // Will replace {{ var num_lights }} with the value referenced by num_lights (In the previous example: 2)



## Updating uniforms on each frame

There's a helper to access uniforms and modify them on each frame: the options "onFrame" (See example above) provides
all uniforms in its only parameter. You don't have to return anything since the uniforms are passed by reference.



## Default uniforms usable in shader files / content 

* u\_screen\_size : {x: 600.0, y: 600.0} for a 600x600 screen
* u\_frame\_count : The frame number since beginning of rendering. Starts at 0. On the 956th frame, u\_frameCount will equal ... 956