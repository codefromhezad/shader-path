var PLUGIN_NAME = 'THREEShaderHelper';

( function($) {

    $[PLUGIN_NAME] = function(opts) {

        /* Options / Settings handling */
        var defaults = {
            debug: false,
            vertexShaderFile: null,
            fragmentShaderFile: null,
            vertexShaderContents: 'void main() { gl_Position = vec4( position, 1.0 ); }',
            fragmentShaderContents: 'void main() { gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0); }',
            canvasSize:     { w: 400, h: 400 },
            canvasContainer: 'body',
            onSceneInit: null
        }
        var opts = $.extend(defaults, opts);

        logger.isActive = opts.debug;


        /* Initial shaders to load debug logs */
        if( opts.vertexShaderFile ) {
            logger.as('Vertex Shader').info('Using file', opts.vertexShaderFile);
        } else {
            logger.as('Vertex Shader').info('Using default shader.');
        }

        if( opts.fragmentShaderFile ) {
            logger.as('Fragment Shader').info('Using file', opts.fragmentShaderFile);
        } else {
            logger.as('Fragment Shader').info('Using default shader.');
        }


        /* Preparing THREE.js / OpenGL necessary variables and codebase */
        var threeCamera   = new THREE.Camera(), 
            threeScene    = new THREE.Scene(), 
            threeRenderer = new THREE.WebGLRenderer(),
            screenPlane   = new THREE.PlaneBufferGeometry( 2, 2 );

        var shaderUniforms = {
            u_screenSize: { type: "v2", value: new THREE.Vector2(opts.canvasSize.w, opts.canvasSize.h) },
        };

        threeCamera.position.z = 1;
        threeRenderer.setPixelRatio( window.devicePixelRatio );


        /* Updating Shaders contents if a filename was specified. In this case,
           we build an array of jquery deferred objects to load them with AJAX */
        var listOfDeferredLoaders = [];
        if( opts.vertexShaderFile ) {
            listOfDeferredLoaders.push( $.get(opts.vertexShaderFile, function(data) { 
                opts.vertexShaderContents = data; 
                logger.as('Vertex Shader').info('Loaded shader file.');
            }) );
        }
        if( opts.fragmentShaderFile ) {
            listOfDeferredLoaders.push( $.get(opts.fragmentShaderFile, function(data) { 
                opts.fragmentShaderContents = data; 
                logger.as('Fragment Shader').info('Loaded shader file.');
            }) );
        }

        /* The actual rendering function. This will be called later (we'll check if
           we need to load some shaders files beforehand first. Check the next comment) */
        var renderMain = function() {
            var screenPlaneMaterial = new THREE.ShaderMaterial({
                uniforms: shaderUniforms,
                vertexShader: opts.vertexShaderContents,
                fragmentShader: opts.fragmentShaderContents
            });

            var screenPlaneMesh = new THREE.Mesh(
                new THREE.PlaneBufferGeometry( 2, 2 ), 
                screenPlaneMaterial
            );

            threeScene.add( screenPlaneMesh );

            $(opts.canvasContainer).append( threeRenderer.domElement );
            
            if( opts.onSceneInit ) {
                opts.onSceneInit();
            }

            threeRenderer.setSize( opts.canvasSize.w, opts.canvasSize.h );
            threeRenderer.render( threeScene, threeCamera );
        }

        /* Shader files loading with jquery Ajax helpers */
        /* If not necessary, continue with scene rendering */
        if( listOfDeferredLoaders.length ) {
            $.when.apply($, listOfDeferredLoaders).then(renderMain);
        } else {
            renderMain();
        }
    }

} ) (jQuery);