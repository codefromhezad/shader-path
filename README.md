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
        onFrame: function(uniforms) {
            uniforms.u_lights_origin.value[0].x = 2.0 * Math.cos(uniforms.u_frame_count.value / 40.0);
            uniforms.u_lights_origin.value[0].z = 2.0 * Math.sin(uniforms.u_frame_count.value / 40.0);
        },
        shaderInject: {
            num_lights: 1
        },
        shaderIncludeFiles: {
            structs: 'structs.c'
        },
        uniforms: {
            u_ambiant_color: { type: "v4", value: AMBIANT_COLOR },
            u_lights_origin: { type: "v3v", value: [ 
                new THREE.Vector3( 2.0, 1.5, -2.0 ) 
            ] },
            u_lights_color: { type: "v4v", value: [
                new THREE.Vector4( 1.0, 1.0, 1.0, 1.0 )
            ] },
            u_lights_intensity: { type: "fv1", value: [ 
                1.0
            ] }
        }
    });



## Including external code into shaders (imitating C's #include statements)

Any occurence of "{{ include _include\_name_ }}" in the shader will be replaced by the contents of the file identified by _include\_name_.



## Injecting variables into shaders

Any occurence of "{{ var _var\_name_ }}" in the shader will be replaced with the value identified by _var\_name_.



## Shader injection Example
	
	{{ include structs }} // Will be replaced with contents from the file referenced by "structs"

	const int sceneNumLights = {{ var num_lights }}; // Will replace {{...}} with value referenced by num_lights



## Updating uniforms on each frame

There's a helper to access uniforms and modify them on each frame: the options "onFrame" (See example above) provides
all uniforms in its only parameter. You don't have to return anything since the uniforms are passed by reference.



## Default uniforms usable in shader files / content 

* u\_screen\_size : {x: 600.0, y: 600.0} for a 600x600 screen
* u\_frame\_count : The frame number since beginning of rendering. Starts at 0. On the 956th frame, u\_frameCount will equal ... 956