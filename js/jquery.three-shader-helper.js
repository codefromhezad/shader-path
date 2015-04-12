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
            uniforms: {},
            shaderInject: {},
            shaderIncludeFiles: {},
            shaderIncludeContents: {},
            onSceneInit: null,
            onFrame: null
        }
        var opts = $.extend(defaults, opts);

        logger.isActive = opts.debug;
        logger.domainPadLength = 10;


        /* Initial shaders to load debug logs */
        if( opts.vertexShaderFile ) {
            logger.as('Init').info('Loading vertex shader file ...');
        } else {
            logger.as('Init').info('Using default vertex shader.');
        }

        if( opts.fragmentShaderFile ) {
            logger.as('Init').info('Loading fragment shader file ...');
        } else {
            logger.as('Init').info('Using default fragment shader.');
        }


        /* Preparing THREE.js / OpenGL necessary variables and codebase */
        var threeCamera   = new THREE.Camera(), 
            threeScene    = new THREE.Scene(), 
            threeRenderer = new THREE.WebGLRenderer(),
            screenPlane   = new THREE.PlaneBufferGeometry( 2, 2 );

        var shaderUniforms = {
            u_screen_size:  { type: "v2", value: new THREE.Vector2(opts.canvasSize.w, opts.canvasSize.h) },
            u_frame_count: { type: "i", value: 0 }
        };

        shaderUniforms = $.extend(shaderUniforms, opts.uniforms);

        logger.as('Init').info('Declared uniforms', shaderUniforms);

        threeCamera.position.z = 1;
        threeRenderer.setPixelRatio( window.devicePixelRatio );


        /* Updating Shaders contents if a filename was specified. In this case,
           we build an array of jquery deferred objects to load them with AJAX */
        var listOfDeferredLoaders = [];
        if( opts.vertexShaderFile ) {
            listOfDeferredLoaders.push( $.get(opts.vertexShaderFile, function(data) { 
                opts.vertexShaderContents = data; 
                logger.as('Init').info('Vertex shader file loaded.');
            }) );
        }
        if( opts.fragmentShaderFile ) {
            listOfDeferredLoaders.push( $.get(opts.fragmentShaderFile, function(data) { 
                opts.fragmentShaderContents = data; 
                logger.as('Init').info('Fragment shader file loaded.');
            }) );
        }
        if( opts.shaderIncludeFiles ) {
            for( var includeName in opts.shaderIncludeFiles ) {
                var includeFile = opts.shaderIncludeFiles[includeName];

                ( function(includeName, includeFile) {
                    listOfDeferredLoaders.push( $.get( includeFile, function(data) {
                        opts.shaderIncludeContents[includeName] = data;
                    }) );
                } ) (includeName, includeFile);
            }
        }

        /* The actual rendering function. This will be called later (we'll check if
           we need to load some shaders files beforehand first. Check the next comment) */
        var renderMain = function() {

            if( opts.shaderIncludeContents ) {
                logger.as('Before render').info('Injected includes', opts.shaderIncludeFiles);
                for( var includeName in opts.shaderIncludeContents ) {
                    var includeRegexp = new RegExp("\{\{\s*js_include\s*:\s*"+includeName+"\s*\}\}", 'g');
                    opts.vertexShaderContents = opts.vertexShaderContents.replace(includeRegexp, opts.shaderIncludeContents[includeName]);
                    opts.fragmentShaderContents = opts.fragmentShaderContents.replace(includeRegexp, opts.shaderIncludeContents[includeName]);
                }
            }

            if( opts.shaderInject ) {
                logger.as('Before render').info('Injected variables', opts.shaderInject);
                for( var varName in opts.shaderInject ) {
                    var injectorRegexp = new RegExp("\{\{\s*js\s*:\s*"+varName+"\s*\}\}", 'g');
                    opts.vertexShaderContents = opts.vertexShaderContents.replace(injectorRegexp, opts.shaderInject[varName]);
                    opts.fragmentShaderContents = opts.fragmentShaderContents.replace(injectorRegexp, opts.shaderInject[varName]);
                }
            }

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

            var renderFrame = function() {

                threeRenderer.render( threeScene, threeCamera );
                screenPlaneMesh.material.uniforms.u_frame_count.value += 1;
                
                if( opts.onFrame ) {
                    opts.onFrame(screenPlaneMesh.material.uniforms);
                }

                window.requestAnimationFrame(renderFrame);
            }

            window.requestAnimationFrame(renderFrame);
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