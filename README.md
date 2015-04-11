Testing stuff with Pathtracing (or maybe Raytracing first ...) and Fragment Shaders

Using:
* jQuery to help with DOM manipulation,
* THREE.js to help with the base WebGL Material/Shader handling

There is also, a custom made jQuery plugin that heavily uses THREE.js to quickly
setup a Plane geometry and use it as a surface for shaders tinkering.



## THREEShaderHelper start example : 

	$.THREEShaderHelper({
	    debug: true,
	    fragmentShaderFile: 'fragment-shader.c',
	    canvasSize:         {w: 600, h: 600},
	    canvasContainer:    '#container',
	    onSceneInit: function() { $('.loading').remove(); },
	    uniforms: {
	        u_accumulatorFrame: {type; "int", value: 0}
	    }
	});

## Default uniforms usable in shader files / content 

* u_screenSize : 600x600 for a 600x600 screen
* u_frameNumber : The absolute frame number. On the 956th frame, u_frameNumber will equal ... 956