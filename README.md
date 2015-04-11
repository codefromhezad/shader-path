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
	    uniforms: {
	        u_accumulatorFrame: {type; "int", value: 0}
	    }
	});


## Default uniforms usable in shader files / content 

* u_screenSize : {x: 600.0, y: 600.0} for a 600x600 screen
* u_frameNumber : The frame number since beginning of rendering. Starts at 0. On the 956th frame, u_frameNumber will equal ... 956