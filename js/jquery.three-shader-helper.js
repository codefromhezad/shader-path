var PLUGIN_NAME = 'THREEShaderHelper';

( function($) {

    /* buildSerializedUniformsObject:
     * Shader helper function. Recursively transforms regular JS array/objects to an object containing
     * serialized values in an object, ready to be passed to the shader.
    */

    var buildSerializeUniformsObject = function(uniformName, uniformValue) {
        var serializedUniformFinalValue = {};
        var MAX_RECURSION = 5;

        var serializeUniformObjectInner = function(objPath, objValue, recursive_depth) {
            if( recursive_depth === undefined ) {
                recursive_depth = 0;
            }

            if( recursive_depth >= MAX_RECURSION ) {
                H.logger.error('To much recursion while serializing uniform object.');
                return false;
            }

            if( H.isArray(objValue) ) {

                for(var i = 0; i < objValue.length; i++) {
                    var ret = serializeUniformObjectInner(objPath + '[' + i + ']', objValue[i], recursive_depth +1);
                    if( ! ret ) {
                        return false;
                    }
                }

            } else if( H.isObject(objValue) ) {

                if( objValue.type !== undefined && objValue.value !== undefined ) {
                    serializedUniformFinalValue[objPath] = objValue;
                    return true;
                } else {
                    for(var i in objValue) {
                        var ret;
                        
                        if( objValue.hasOwnProperty(i) ) {
                            ret = serializeUniformObjectInner(objPath + '.' + i, objValue[i], recursive_depth + 1);
                        }

                        if( ! ret ) {
                            return false;
                        }
                    }
                }

            } else {

                H.logger.error('Error while serializing uniform ' + uniformName);

                return false;;
            }

            return true;
        }

        serializeUniformObjectInner(uniformName, uniformValue);

        return serializedUniformFinalValue;
    };


    $[PLUGIN_NAME] = function(opts, parameter) {

        /* Options / Settings handling */
        var defaults = {
            debug: false,
            vertexShaderFile: null,
            fragmentShaderFile: null,
            vertexShaderContents: 'void main() { gl_Position = vec4( position, 1.0 ); }',
            fragmentShaderContents: 'void main() { gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0); }',
            canvasSize:     { w: 400, h: 400 },
            canvasContainer: 'body',
            animate: true,
            uniforms: {},
            shaderInject: {},
            shaderIncludeFiles: {},
            shaderIncludeContents: {},
            onSceneInit: null,
            onFrame: null
        }

        if( H.isObject( opts ) ) {
            opts = $.extend(defaults, opts);
        } else {

            /* Modifiers/Helpers once the plugin is launched */

            if( typeof opts == 'string' ) {
                switch( opts ) {
                    case 'debug':
                        if( ! parameter ) {
                            return false;
                        }
                        if( ! window[PLUGIN_NAME+'_dbg'][parameter] ) {
                            H.logger.error('Unkown debug slug: ' + parameter);
                            return false;
                        }
                        H.logger.as(parameter).info(window[PLUGIN_NAME+'_dbg'][parameter]);
                        return false;
                }
            }
        }

        H.logger.isActive = opts.debug;
        H.logger.domainPadLength = 10;


        window[PLUGIN_NAME+'_dbg'] = {};
        var registerDbgData = function(slug, data) {
            window[PLUGIN_NAME+'_dbg'][slug] = data;
        }
        var showDebugSlugs = function() {
            var slugs = Object.keys(window[PLUGIN_NAME+'_dbg']).join(', ');
            H.logger.as('Debug').info("For more debug data, type $.THREEShaderHelper('debug', '<debug slug>') in the console.");
            H.logger.as('Debug').info("Available debug slugs: " + slugs);
        }

        /* Initial shaders to load debug logs */
        if( opts.vertexShaderFile ) {
            H.logger.as('Init').info('Vertex shader: ' + opts.vertexShaderFile);
        } else {
            H.logger.as('Init').info('Vertex shader: Default');
        }

        if( opts.fragmentShaderFile ) {
            H.logger.as('Init').info('Fragment shader: ' + opts.fragmentShaderFile);
        } else {
            H.logger.as('Init').info('Fragment shader: Default');
        }


        /* Preparing THREE.js / OpenGL necessary variables and codebase */
        var threeCamera   = new THREE.Camera(), 
            threeScene    = new THREE.Scene(), 
            threeRenderer = new THREE.WebGLRenderer(),
            screenPlane   = new THREE.PlaneBufferGeometry( 2, 2 );
        
        var tempShaderUniforms = $.extend(
            {
                u_screen_size:  { type: "v2", value: new THREE.Vector2(opts.canvasSize.w, opts.canvasSize.h) },
                u_frame_count: { type: "i", value: 0 }
            },
            opts.uniforms
        );

        var parsedUniforms = {};
        for(var uniform_name in tempShaderUniforms) {
            parsedUniforms = $.extend(parsedUniforms, buildSerializeUniformsObject(uniform_name, tempShaderUniforms[uniform_name]));
        }

        var shaderUniforms = parsedUniforms;

        H.logger.as('Init').info(H.objSize(shaderUniforms) + ' uniforms loaded');
        registerDbgData('uniforms', shaderUniforms);
        
        threeCamera.position.z = 1;
        threeRenderer.setPixelRatio( window.devicePixelRatio );


        /* Updating Shaders contents if a filename was specified. In this case,
           we build an array of jquery deferred objects to load them with AJAX */
        var listOfDeferredLoaders = [];
        if( opts.vertexShaderFile ) {
            listOfDeferredLoaders.push( $.get(opts.vertexShaderFile, function(data) { 
                opts.vertexShaderContents = data; 
            }) );
        }
        if( opts.fragmentShaderFile ) {
            listOfDeferredLoaders.push( $.get(opts.fragmentShaderFile, function(data) { 
                opts.fragmentShaderContents = data; 
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
                H.logger.as('Init').info(H.objSize(opts.shaderIncludeFiles) + ' files injected as includes');
                registerDbgData('includes', opts.shaderIncludeFiles);

                for( var includeName in opts.shaderIncludeContents ) {
                    var includeRegexp = new RegExp("\{\{ *include +"+includeName+" *\}\}", 'g');
                    opts.vertexShaderContents = opts.vertexShaderContents.replace(includeRegexp, opts.shaderIncludeContents[includeName]);
                    opts.fragmentShaderContents = opts.fragmentShaderContents.replace(includeRegexp, opts.shaderIncludeContents[includeName]);
                }
            }

            if( opts.shaderInject ) {
                H.logger.as('Init').info(H.objSize(opts.shaderInject) + ' template variables injected');
                registerDbgData('vars', opts.shaderInject);

                for( var varName in opts.shaderInject ) {
                    var injectorRegexp = new RegExp("\{\{ *var +"+varName+" *\}\}", 'g');
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
            }

            var renderFrameAnimate = function() {
                renderFrame();

                screenPlaneMesh.material.uniforms.u_frame_count.value += 1;
                
                if( opts.onFrame ) {
                    opts.onFrame(screenPlaneMesh.material.uniforms);
                }

                window.requestAnimationFrame(renderFrameAnimate);
            }

            H.logger.as('Init').info('Ready to render');

            if( opts.animate ) {
                window.requestAnimationFrame(renderFrameAnimate);
            } else {
                renderFrame();
            }

            showDebugSlugs();
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