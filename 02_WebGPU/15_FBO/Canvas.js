// global varibles

var canvas = null;

var bFullScreen = false;

var CANVAS_ORIGINAL_WIDTH;
var CANVAS_ORIGINAL_HEIGHT;

// animation related
var requestAnimationFrame =
    window.requestAnimationFrame ||
    window.webkitRequestAnimationFrame ||
    window.mozRequestAnimationFrame ||
    window.oRequestAnimationFrame ||
    window.msRequestAnimationFrame;

var cancelAnimationFrame = 
    window.cancelAnimationFrame ||
    window.webkitCancelRequestAnimationFrame ||
    window.webkitCancelAnimationFrame ||
    window.mozCancelRequestAnimationFrame ||
    window.mozCancelAnimationFrame ||
    window.oCancelRequestAnimationFrame ||
    window.oCancelAnimationFrame ||
    window.msCancelRequestAnimationFrame ||
    window.msCancelAnimationFrame;

// WebGPU related varible
var clearColor = null;
var clearColor_FBO = null;
let Device = null;
let Context = null;
let Queue = null;
let canvasFormat = null;
let animationFrameId = null;


var lightAmbient = new Float32Array([0.4, 0.4, 0.4, 1.0]);
var lightDiffuse = new Float32Array([1.0, 1.0, 1.0, 1.0]);
var lightSpecular = new Float32Array([1.0, 1.0, 1.0, 1.0]);
var lightPosition = new Float32Array([100.0, 100.0, 100.0, 1.0]);
var materialAmbient = new Float32Array([0.9, 0.5, 0.3, 1.0]);
var materialDiffuse = new Float32Array([0.9, 0.5, 0.3, 1.0]);
var materialSpecular = new Float32Array([0.8, 0.8, 0.8, 1.0]);
var materialShininess = new Float32Array([128.0, 0.0, 0.0, 1.0]);
var keyFlags = new Uint32Array([0,0,0,0]);  // .x for light . y for texture


var isLightingEnabled = false;
var isTexturingEnabled = false;
var isTeapotAnimating  = true;
let perspectiveProjectionMatrix = null;
let perspectiveProjectionMatrix_FBO = null;

let depthTexture = null;


// Teapot related new declaration
const FBO_WIDTH = 512;
const FBO_HEIGHT = 512;


let texture_Teapot_Color = null;
let texture_Teapot_Depth = null;
let texture_Marble = null;
let sampler_Marble = null;
let sampler_Teapot = null;
let renderPipeline_Teapot = null;
let buffer_Uniform_Teapot = null;
let bindgroup_Uniform_Teapot = null;
let bindgroup_Marble_Texture_And_Sampler = null;

let buffer_Teapot_Positions = null;
let buffer_Teapot_Normals = null;
let buffer_Teapot_Texcoords = null;
let buffer_Teapot_Elements = null;
let numTeapotIndices = 0;

//cube related declarations

let renderPipeline_Cube = null;
let buffer_Uniform_Cube = null;
let bindgroup_Uniform_Cube = null;
let bindgroup_Teapot_Texture_And_Sampler = null;

let buffer_Cube_Positions =  null;
let buffer_Cube_Texcoords =  null;

var angle_Teapot = 0.0;
var angle_Cube = 0.0;



// onload function
async function main() {
    // get canvas
    canvas = document.getElementById("PRS");
    if (canvas == null) {
        console.log("Getting Canvas failed\n");
    }
    else {
        console.log("Getting Canvas Suceeded\n");
    }

    // set canvas width and heigth for future use
    CANVAS_ORIGINAL_WIDTH = canvas.width;
    CANVAS_ORIGINAL_HEIGHT = canvas.height;

    //register event handers

    // register for keyboard events
    window.addEventListener("keydown", keyDown, false);

    // register for mouse events
    window.addEventListener("click", mouseDown, false);

    window.addEventListener("resize", resize, false);

    // best practices for WebGPU during fullscreen
    document.addEventListener("fullscreenchange", onFullScreenChange, false);

    document.addEventListener("webkitfullscreenchange", onFullScreenChange, false);

    // initialize WebGPU
    const GPU = navigator.gpu;
    if(GPU == null) 
    {
        console.log("WebGPU is not supported by your browser.\n");
        throw Error("WebGPU is not supported by your browser.\n");

    }
    else
    {    
        console.log("WebGPU is supported by your browser.\n");
    }

    // get GPUadapter object from GPU interface
    const adapter = await GPU.requestAdapter();
    if(adapter == null) 
    {
        console.log("adapter is not supported by your browser.\n");
        throw Error("adapter is not supported by your browser.\n");

    }
    else
    {    
        console.log("adapter is supported by your browser.\n");
    }

    // get GPUDevice object from GPUadapter object
    Device = await adapter.requestDevice();
    if(Device == null)
    {
        console.log("Device is not supported by your browser.\n");
        throw Error("Device is not supported by your browser.\n");
    }
    else
    {
        console.log("Device is supported by your browser.\n");
    }

    // as broswer can be on mobaile, so device may get lost dure to reset switchoff disconnection in such we may not have cpturable error so rester on generic handler for uncaught error with device
    Device.addEventListener("uncapturederror", onUncapturedError);

    // register specific device lost handler
    Device.lost.then(onDeviceLost);

    // call stub functions from here

    await initialise();

    resize();

    display();
}

function onUncapturedError(event)
{
    // code
    console.error("WebGPUUncapturedError: ", event.error.message);
}

function onDeviceLost(info)
{
    // code
    console.warn("WebGPUDeviceLostReason: ", info.reason , "WebGPUDeviceLostMessage: ", info.message);
    Device = null;
    Queue = null;
    perspectiveProjectionMatrix = null;
    perspectiveProjectionMatrix_FBO = null;
    depthTexture = null;

    // teapot realted varibles
    texture_Teapot_Color = null;
    texture_Teapot_Depth = null;
    texture_Marble = null;
    sampler_Marble = null;
    sampler_Teapot = null;
    renderPipeline_Teapot = null;
    buffer_Uniform_Teapot = null;
    bindgroup_Uniform_Teapot = null;
    bindgroup_Marble_Texture_And_Sampler = null;

    buffer_Teapot_Positions = null;
    buffer_Teapot_Normals = null;
    buffer_Teapot_Texcoords = null;
    buffer_Teapot_Elements = null;

    // cube related varibles
    renderPipeline_Cube = null;
    buffer_Uniform_Cube = null;
    bindgroup_Uniform_Cube = null;
    bindgroup_Teapot_Texture_And_Sampler = null;

    buffer_Cube_Positions =  null;
    buffer_Cube_Texcoords =  null;



}


function keyDown(event)
{
    switch (event.key) // keyCode is deprecated
    {
        case "Escape":
            uninitialise();
            window.close(); // May not work across all browsers
            break;

        case "F":
        case "f":
            toggleFullScreen();
            break;

        case "L":
        case "l":
            isLightingEnabled = !isLightingEnabled;
            break;

        case "T":
        case "t":
            isTexturingEnabled = !isTexturingEnabled;
            break;

        default:
            break;
    }
}


function mouseDown()
{
    //code
}


function degToRad(degree)
{
    //code
    return (degree * Math.PI / 180.0);
}


function toggleFullScreen() 
{
    var full_element =
        document.fullscreenElement ||
        document.webkitFullscreenElement ||
        document.mozFullscreenElement ||
        document.msFullscreenElement ||
        null;

    //if not fullscreen
    if (full_element == null) 
    {
        if (canvas.requestFullscreen) 
        {
            canvas.requestFullscreen();
        } 
        else if (canvas.webkitRequestFullscreen) 
        {
            canvas.webkitRequestFullscreen();
        } 
        else if (canvas.mozRequestFullScreen) 
        {
            canvas.mozRequestFullScreen();
        } 
        else if (canvas.msRequestFullscreen) 
        {
            canvas.msRequestFullscreen();
        }

        // in we webgl we initilaise bFullScreen here not thinking about sync opreations but in webgpu considering cross browser fullscreen compability we will do this in onFullScreenChange event handler
    } 

    else 
    {
        // if already full screen
        if (document.exitFullscreen) 
        {
            document.exitFullscreen();
        } 
        else if (document.webkitExitFullscreen) 
        {
            document.webkitExitFullscreen();
        } 
        else if (document.mozCancelFullScreen) 
        {
            document.mozCancelFullScreen();
        } 
        else if (document.msExitFullscreen) 
        {
            document.msExitFullscreen();
        }

       // in we webgl we initilaise bFullScreen here not thinking about sync opreations but in webgpu considering cross browser fullscreen compability we will do this in onFullScreenChange event handler
    }
}

function onFullScreenChange()
{
    // code
    var full_element =
        document.fullscreenElement ||
        document.webkitFullscreenElement ||
        document.mozFullscreenElement ||
        document.msFullscreenElement ||
        null;

    //if not fullscreen
    if (full_element == null) 
    {
        bFullScreen = false;
    } 
    else 
    {
        bFullScreen = true;
    }

    // call resize here
    resize();
} 

async function initialise() 
{
    // code
    Queue = Device.queue;
    console.log("initialise() : WebGPUQueue is obtained successfully.\n");

    // get the WebGPUContext 
    Context = canvas.getContext("webgpu");
    if(Context == null)
    {
        console.log("initialise() :WebGPUContext is not supported by your browser.\n");
        throw Error("initialise() :WebGPUContext is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() :WebGPUContext is supported by your browser.\n");
    }

    // get the preferred WebGPU color format for canvas who type is GPUTextureFormat
    canvasFormat = navigator.gpu.getPreferredCanvasFormat();

    // configure the canvas by this obtained format to suit our needs and become functionable
    const canvasConfiguration = 
    {
        device: Device,
        format: canvasFormat,
        usage : GPUTextureUsage.RENDER_ATTACHMENT | GPUTextureUsage.COPY_SRC,
        alphaMode: "opaque"
    };

    Context.configure(canvasConfiguration);
    console.log("initialise() : canvasConfiguration is successfully done for WebGPU with format : ", canvasFormat, "\n");


    // Teapot

    // vertex shader code in WGSL
   const teapotVertexShaderSourceCode =
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) transformedNormal: vec3<f32>,\n" +
    "    @location(1) lightDirection: vec3<f32>,\n" +
    "    @location(2) viewerVector: vec3<f32>,\n" +
    "    @location(3) texcoords: vec2<f32>,\n" +
    "};\n" +

    "struct MyUniformData_FBO\n" +
    "{\n" +
    "    modelMatrix: mat4x4<f32>,\n" +
    "    viewMatrix: mat4x4<f32>,\n" +
    "    projectionMatrix: mat4x4<f32>,\n" +
    "    lightAmbient: vec4<f32>,\n" +
    "    lightDiffuse: vec4<f32>,\n" +
    "    lightSpecular: vec4<f32>,\n" +
    "    lightPosition: vec4<f32>,\n" +
    "    materialAmbient: vec4<f32>,\n" +
    "    materialDiffuse: vec4<f32>,\n" +
    "    materialSpecular: vec4<f32>,\n" +
    "    materialShininess: vec4<f32>,\n" +
    "    keyFlags: vec4<u32>,\n" +
    "};\n" +

    "@group(0) @binding(0)\n" +
    "var<uniform> uMyUniformData: MyUniformData_FBO;\n" +

    // Convert mat4x4 to mat3x3
    "fn mat3FromMat4(m: mat4x4<f32>) -> mat3x3<f32>\n" +
    "{\n" +
    "    return mat3x3<f32>(m[0].xyz, m[1].xyz, m[2].xyz);\n" +
    "}\n" +

    // Inverse of 3x3 matrix
    "fn inverse3x3(m: mat3x3<f32>) -> mat3x3<f32>\n" +
    "{\n" +
    "    let determinant = " +
    "        m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) - " +
    "        m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) + " +
    "        m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);\n" +

    "    let inverse_determinant = 1.0 / determinant;\n" +

    "    let column0 = vec3<f32>(\n" +
    "        (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * inverse_determinant,\n" +
    "        (m[2][1] * m[0][2] - m[0][1] * m[2][2]) * inverse_determinant,\n" +
    "        (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * inverse_determinant\n" +
    "    );\n" +

    "    let column1 = vec3<f32>(\n" +
    "        (m[2][0] * m[1][2] - m[1][0] * m[2][2]) * inverse_determinant,\n" +
    "        (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * inverse_determinant,\n" +
    "        (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * inverse_determinant\n" +
    "    );\n" +

    "    let column2 = vec3<f32>(\n" +
    "        (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * inverse_determinant,\n" +
    "        (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * inverse_determinant,\n" +
    "        (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * inverse_determinant\n" +
    "    );\n" +

    "    return mat3x3<f32>(column0, column1, column2);\n" +
    "}\n" +

    "@vertex\n" +
    "fn main(@location(0) pos: vec3<f32>, @location(1) normal: vec3<f32>, @location(2) tex: vec2<f32>) -> VertexOutput\n" +
    "{\n" +
    "    var output: VertexOutput;\n" +

    "    let eyeCoordinates: vec4<f32> = " +
    "        uMyUniformData.viewMatrix * " +
    "        uMyUniformData.modelMatrix * " +
    "        vec4<f32>(pos, 1.0);\n" +

    "    let modelViewMatrix: mat3x3<f32> = " +
    "        mat3FromMat4(uMyUniformData.viewMatrix * uMyUniformData.modelMatrix);\n" +

    "    let normalMatrix: mat3x3<f32> = " +
    "        transpose(inverse3x3(modelViewMatrix));\n" +

    "    output.transformedNormal = " +
    "        normalize(normalMatrix * normal);\n" +

    "    output.lightDirection = " +
    "        normalize(uMyUniformData.lightPosition.xyz - eyeCoordinates.xyz);\n" +

    "    output.viewerVector = " +
    "        normalize(-eyeCoordinates.xyz);\n" +

    "    output.texcoords = tex;\n" +

    "    output.Position = " +
    "        uMyUniformData.projectionMatrix * " +
    "        uMyUniformData.viewMatrix * " +
    "        uMyUniformData.modelMatrix * " +
    "        vec4<f32>(pos, 1.0);\n" +

    "    return output;\n" +
    "}";


    const shaderModuleDescriptor_TeapotVertexShader = 
    {
        code: teapotVertexShaderSourceCode
    };

    const shaderModule_TeapotVertexShader = Device.createShaderModule(shaderModuleDescriptor_TeapotVertexShader);

    if(shaderModule_TeapotVertexShader == null)
    {
        console.log("initialise() : shaderModule_TeapotVertexShader is not supported by your browser.\n");
        throw Error("initialise() : shaderModule_TeapotVertexShader is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : shaderModule_TeapotVertexShader is supported by your browser.\n");
    }


    // fragment shader code in WGSL
   const teapotFragmentShaderSourceCode =
     "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) transformedNormal: vec3<f32>,\n" +
    "    @location(1) lightDirection: vec3<f32>,\n" +
    "    @location(2) viewerVector: vec3<f32>,\n" +
    "    @location(3) texcoords: vec2<f32>,\n" +
    "};\n" +

    "struct MyUniformData_FBO\n" +
    "{\n" +
    "    modelMatrix: mat4x4<f32>,\n" +
    "    viewMatrix: mat4x4<f32>,\n" +
    "    projectionMatrix: mat4x4<f32>,\n" +
    "    lightAmbient: vec4<f32>,\n" +
    "    lightDiffuse: vec4<f32>,\n" +
    "    lightSpecular: vec4<f32>,\n" +
    "    lightPosition: vec4<f32>,\n" +
    "    materialAmbient: vec4<f32>,\n" +
    "    materialDiffuse: vec4<f32>,\n" +
    "    materialSpecular: vec4<f32>,\n" +
    "    materialShininess: vec4<f32>,\n" +
    "    keyFlags: vec4<u32>,\n" +
    "};\n" +

    "@group(0) @binding(0)\n" +
    "var<uniform> uMyUniformData: MyUniformData_FBO;\n" +

    "@group(1) @binding(0) var umarbleTexture: texture_2d<f32>;\n" +

    "@group(1) @binding(1) var umarbleSampler: sampler;\n" +


    "@fragment\n" +
    "fn main(output: VertexOutput) -> @location(0) vec4<f32>\n" +
    "{\n" +
    "    var phong_ads_color: vec3<f32>;\n" +

    "    if(uMyUniformData.keyFlags.x == 1u && uMyUniformData.keyFlags.y == 1u)\n" +
    "    {\n" +

    "        let normalizedTransformedNormal: vec3<f32> = " +
    "            normalize(output.transformedNormal);\n" +

    "        let normalizedLightDirection: vec3<f32> = " +
    "            normalize(output.lightDirection);\n" +

    "        let normalizedViewerVector: vec3<f32> = " +
    "            normalize(output.viewerVector);\n" +

    // Ambient
    "        let ambient: vec3<f32> = " +
    "            uMyUniformData.lightAmbient.xyz * " +
    "            uMyUniformData.materialAmbient.xyz * textureSample(umarbleTexture, umarbleSampler, output.texcoords).xyz;\n" +

    // Diffuse
    "        let diffuse: vec3<f32> = " +
    "            uMyUniformData.lightDiffuse.xyz * " +
    "            uMyUniformData.materialDiffuse.xyz * " +
    "            max(dot(normalizedLightDirection, normalizedTransformedNormal), 0.0) * textureSample(umarbleTexture, umarbleSampler, output.texcoords).xyz;\n" +

    // Reflection
    "        let reflectionVector: vec3<f32> = " +
    "            reflect(-normalizedLightDirection, normalizedTransformedNormal);\n" +

    // Specular
    "        let specular: vec3<f32> = " +
    "            uMyUniformData.lightSpecular.xyz * " +
    "            uMyUniformData.materialSpecular.xyz * " +
    "            pow(max(dot(reflectionVector, normalizedViewerVector), 0.0), " +
    "            uMyUniformData.materialShininess.x);\n" +

    "        phong_ads_color = ambient + diffuse + specular;\n" +
    "    }\n" +

    "   else if(uMyUniformData.keyFlags.x == 1u && uMyUniformData.keyFlags.y == 0u)\n" +
    "    {\n" +

    "        let normalizedTransformedNormal: vec3<f32> = " +
    "            normalize(output.transformedNormal);\n" +

    "        let normalizedLightDirection: vec3<f32> = " +
    "            normalize(output.lightDirection);\n" +

    "        let normalizedViewerVector: vec3<f32> = " +
    "            normalize(output.viewerVector);\n" +

    // Ambient
    "        let ambient: vec3<f32> = " +
    "            uMyUniformData.lightAmbient.xyz * " +
    "            uMyUniformData.materialAmbient.xyz;\n" +

    // Diffuse
    "        let diffuse: vec3<f32> = " +
    "            uMyUniformData.lightDiffuse.xyz * " +
    "            uMyUniformData.materialDiffuse.xyz * " +
    "            max(dot(normalizedLightDirection, normalizedTransformedNormal), 0.0);\n" +

    // Reflection
    "        let reflectionVector: vec3<f32> = " +
    "            reflect(-normalizedLightDirection, normalizedTransformedNormal);\n" +

    // Specular
    "        let specular: vec3<f32> = " +
    "            uMyUniformData.lightSpecular.xyz * " +
    "            uMyUniformData.materialSpecular.xyz * " +
    "            pow(max(dot(reflectionVector, normalizedViewerVector), 0.0), " +
    "            uMyUniformData.materialShininess.x);\n" +

    "        phong_ads_color = ambient + diffuse + specular;\n" +
    "    }\n" +

    "    else if(uMyUniformData.keyFlags.x == 0u && uMyUniformData.keyFlags.y == 1u)\n" +
    "    {\n" +
    "        phong_ads_color = textureSample(umarbleTexture, umarbleSampler, output.texcoords).xyz;\n" +
    "    }\n" +


    "    else\n" +
    "    {\n" +
    "        phong_ads_color = vec3<f32>(1.0, 1.0, 1.0);\n" +
    "    }\n" +

    "    return vec4<f32>(phong_ads_color, 1.0);\n" +
    "}";

    const shaderModuleDescriptor_TeapotFragmentShader =
    {
        code: teapotFragmentShaderSourceCode
    };

    const shaderModule_TeapotFragmentShader = Device.createShaderModule(shaderModuleDescriptor_TeapotFragmentShader);
    
    if(shaderModule_TeapotFragmentShader == null)
    {
        console.log("initialise() : shaderModule_teapotFragmentShader is not supported by your browser.\n");
        throw Error("initialise() : shaderModule_teapotFragmentShader is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : shaderModule_teapotFragmentShader is supported by your browser.\n");
    }

    // CUBE

    const cubeVertexShaderSourceCode =
    "struct MVPUniform\n" +
    "{\n" +
    "    MVPMatrix: mat4x4<f32>\n" +
    "};\n" +
    "\n" +
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) Texcoords: vec2<f32>\n" +
    "};\n" +
    "\n" +
    "@group(0) @binding(0)\n" +
    "var<uniform> mvpUniform: MVPUniform;\n" +
    "\n" +
    "@vertex\n" +
    "fn main(\n" +
    "    @location(0) pos: vec4<f32>,\n" +
    "    @location(1) tex: vec2<f32>\n" +
    ") -> VertexOutput\n" +
    "{\n" +
    "    var output: VertexOutput;\n" +
    "    output.Position = mvpUniform.MVPMatrix * pos;\n" +
    "    output.Texcoords = tex;\n" +
    "    return output;\n" +
    "}";


    const shaderModuleDescriptor_CubeVertexShader = 
    {
        code: cubeVertexShaderSourceCode
    };

    const shaderModule_CubeVertexShader = Device.createShaderModule(shaderModuleDescriptor_CubeVertexShader);

    if(shaderModule_CubeVertexShader == null)
    {
        console.log("initialise() : shaderModule_CubeVertexShader is not supported by your browser.\n");
        throw Error("initialise() : shaderModule_CubeVertexShader is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : shaderModule_CubeVertexShader is supported by your browser.\n");
    }

     // fragment shader code in WGSL
    const cubeFragmentShaderSourceCode =
    "@group(1) @binding(0) var uteapotTexture: texture_2d<f32>;\n" +
    "@group(1) @binding(1) var uteapotSampler: sampler;\n" +
    "\n" +
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) Texcoords: vec2<f32>\n" +
    "};\n" +
    "\n" +
    "@fragment\n" +
    "fn main(output: VertexOutput) -> @location(0) vec4<f32>\n" +
    "{\n" +
    "    var color = textureSample(uteapotTexture, uteapotSampler, output.Texcoords);\n" +
    "    return color;\n" +
    "}";

    const shaderModuleDescriptor_CubeFragmentShader = 
    {
        code: cubeFragmentShaderSourceCode
    };

    const shaderModule_CubeFragmentShader = Device.createShaderModule(shaderModuleDescriptor_CubeFragmentShader);
    
    if(shaderModule_CubeFragmentShader == null)
    {
        console.log("initialise() : shaderModule_CubeFragmentShader is not supported by your browser.\n");
        throw Error("initialise() : shaderModule_CubeFragmentShader is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : shaderModule_CubeFragmentShader is supported by your browser.\n");
    }

    // teapot gemotry
    const teapot = new Mesh();
    teapot.allocate(teapotVertexCount);
    
    const startTime = performance.now();

    for(let i = 0; i < teapotVertexCount; i+=3)
    {
        // Positions

        const singleVertex = 
        [
            
            [
                teapotPositions[(i + 0) * 3 + 0],
                teapotPositions[(i + 0) * 3 + 1],
                teapotPositions[(i + 0) * 3 + 2],
            ],

            [
                teapotPositions[(i + 1) * 3 + 0],
                teapotPositions[(i + 1) * 3 + 1],
                teapotPositions[(i + 1) * 3 + 2],
            ],

            [
                teapotPositions[(i + 2) * 3 + 0],
                teapotPositions[(i + 2) * 3 + 1],
                teapotPositions[(i + 2) * 3 + 2],
            ]

        ];

        // Normal

        const singleNormal = 
        [
            
            [
                teapotNormals[(i + 0) * 3 + 0],
                teapotNormals[(i + 0) * 3 + 1],
                teapotNormals[(i + 0) * 3 + 2],
            ],

            [
                teapotNormals[(i + 1) * 3 + 0],
                teapotNormals[(i + 1) * 3 + 1],
                teapotNormals[(i + 1) * 3 + 2],
            ],

            [
                teapotNormals[(i + 2) * 3 + 0],
                teapotNormals[(i + 2) * 3 + 1],
                teapotNormals[(i + 2) * 3 + 2],
            ]

        ];

        // Texcoords

        const singleTexcoord = 
        [
            
            [
                teapotTexCoords[(i + 0) * 2 + 0],
                teapotTexCoords[(i + 0) * 2 + 1]
            ],

            [
                teapotTexCoords[(i + 1) * 2 + 0],
                teapotTexCoords[(i + 1) * 2 + 1]
            ],

            [
                teapotTexCoords[(i + 2) * 2 + 0],
                teapotTexCoords[(i + 2) * 2 + 1]
            ]

        ];

        teapot.addTriangle(singleVertex, singleNormal, singleTexcoord);

    }

    numTeapotIndices = teapot.getIndexCount();

    const teapotMeshData = teapot.getMeshData();

    buffer_Teapot_Positions = createVertexBuffer(teapotMeshData.verticesArray);

    if(buffer_Teapot_Positions == null)
    {
        console.log("initialise() : buffer_Teapot_Positions is not supported by your browser.\n");
        throw Error("initialise() : buffer_Teapot_Positions is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Teapot_Positions is supported by your browser.\n");
    }

    buffer_Teapot_Normals = createVertexBuffer(teapotMeshData.normalsArray);

    if(buffer_Teapot_Normals == null)
    {
        console.log("initialise() : buffer_Teapot_Normals is not supported by your browser.\n");
        throw Error("initialise() : buffer_Teapot_Normals is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Teapot_Normals is supported by your browser.\n");
    }

    buffer_Teapot_Texcoords = createVertexBuffer(teapotMeshData.texCoordsArray);

    if(buffer_Teapot_Texcoords == null)
    {
        console.log("initialise() : buffer_Teapot_Texcoords is not supported by your browser.\n");
        throw Error("initialise() : buffer_Teapot_Texcoords is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Teapot_Texcoords is supported by your browser.\n");
    }

    buffer_Teapot_Elements = createIndexBuffer(teapotMeshData.indicesArray);

    if(buffer_Teapot_Elements == null)
    {
        console.log("initialise() : buffer_Teapot_Elements is not supported by your browser.\n");
        throw Error("initialise() : buffer_Teapot_Elements is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Teapot_Elements is supported by your browser.\n");
    }

    // Cube Geometry

    const cube_Positions = new Float32Array([
    // Front face
    -1.0, -1.0,  1.0,
     1.0, -1.0,  1.0,
     1.0,  1.0,  1.0,

    -1.0, -1.0,  1.0,
     1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,

    // Back face
     1.0, -1.0, -1.0,
    -1.0, -1.0, -1.0,
    -1.0,  1.0, -1.0,

     1.0, -1.0, -1.0,
    -1.0,  1.0, -1.0,
     1.0,  1.0, -1.0,

    // Right face
     1.0, -1.0,  1.0,
     1.0, -1.0, -1.0,
     1.0,  1.0, -1.0,

     1.0, -1.0,  1.0,
     1.0,  1.0, -1.0,
     1.0,  1.0,  1.0,

    // Left face
    -1.0, -1.0, -1.0,
    -1.0, -1.0,  1.0,
    -1.0,  1.0,  1.0,

    -1.0, -1.0, -1.0,
    -1.0,  1.0,  1.0,
    -1.0,  1.0, -1.0,

    // Top face
    -1.0,  1.0,  1.0,
     1.0,  1.0,  1.0,
     1.0,  1.0, -1.0,

    -1.0,  1.0,  1.0,
     1.0,  1.0, -1.0,
    -1.0,  1.0, -1.0,

    // Bottom face
    -1.0, -1.0, -1.0,
     1.0, -1.0, -1.0,
     1.0, -1.0,  1.0,

    -1.0, -1.0, -1.0,
     1.0, -1.0,  1.0,
    -1.0, -1.0,  1.0
    ]);

    const cube_Texcoords = new Float32Array([
        // Front
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        // Back
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        // Right
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        // Left
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        // Top
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        // Bottom
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,

        0.0, 0.0,
        1.0, 1.0,
        0.0, 1.0
    ]);

    buffer_Cube_Positions = createVertexBuffer(cube_Positions);

    if(buffer_Cube_Positions == null)
    {
        console.log("initialise() : buffer_Cube_Positions is not supported by your browser.\n");
        throw Error("initialise() : buffer_Cube_Positions is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Cube_Positions is supported by your browser.\n");
    }

    buffer_Cube_Texcoords = createVertexBuffer(cube_Texcoords);

    if(buffer_Cube_Texcoords == null)
    {
        console.log("initialise() : buffer_Cube_Texcoords is not supported by your browser.\n");
        throw Error("initialise() : buffer_Cube_Texcoords is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Cube_Texcoords is supported by your browser.\n");
    }

    // marble texture

    texture_Marble = await loadTexture("marble.png");

    if(texture_Marble == null)
    {
        console.log("initialise() : texture_Marble is not supported by your browser.\n");
        throw Error("initialise() : texture_Marble is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : texture_Marble is supported by your browser.\n");
    }

    // FBO texture

    // remember the sturecure will not have nay image but here teapot will render as texture of cube

    const teapotColorTextureDescriptor = 
    {
        size: [FBO_WIDTH, FBO_HEIGHT, 1],
        dimension : "2d",
        format: "rgba8unorm",
        usage: GPUTextureUsage.TEXTURE_BINDING | GPUTextureUsage.RENDER_ATTACHMENT
    };

    texture_Teapot_Color = Device.createTexture(teapotColorTextureDescriptor);

    if(texture_Teapot_Color == null)
    {
        console.log("initialise() : texture_Teapot_Color is not supported by your browser.\n");
        throw Error("initialise() : texture_Teapot_Color is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : texture_Teapot_Color is supported by your browser.\n");
    }
    
    // depthtexture

    const teapotDepthTextureDescriptor =
    {
        size: [FBO_WIDTH, FBO_HEIGHT, 1],
        dimension : "2d",
        format: "depth24plus-stencil8",
        usage: GPUTextureUsage.RENDER_ATTACHMENT
    };

    texture_Teapot_Depth = Device.createTexture(teapotDepthTextureDescriptor);

    if(texture_Teapot_Depth == null)
    {
        console.log("initialise() : texture_Teapot_Depth is not supported by your browser.\n");
        throw Error("initialise() : texture_Teapot_Depth is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : texture_Teapot_Depth is supported by your browser.\n");
    }

    const samplerDescriptor_Marble =
    {
        magFilter: "linear",
        minFilter: "linear"
    };

    sampler_Marble = Device.createSampler(samplerDescriptor_Marble);

    if(sampler_Marble == null)
    {
        console.log("initialise() : sampler_Marble is not supported by your browser.\n");
        throw Error("initialise() : sampler_Marble is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : sampler_Marble is supported by your browser.\n");
    }

    // TEAPOT TEXXTURE SAMPLER
    const samplerDescriptor_Teapot =
    {
        magFilter: "linear",
        minFilter: "linear"
    };

    sampler_Teapot = Device.createSampler(samplerDescriptor_Teapot);

    if(sampler_Teapot == null)
    {
        console.log("initialise() : sampler_Teapot is not supported by your browser.\n");
        throw Error("initialise() : sampler_Teapot is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : sampler_Teapot is supported by your browser.\n");
    }

    const teapotUniformBufferSize = Float32Array.BYTES_PER_ELEMENT * 16 * 3 + // model, view, projection matrices
                                   Float32Array.BYTES_PER_ELEMENT * 4 * 8 + // light ambient, diffuse, specular, position
                                   Uint32Array.BYTES_PER_ELEMENT * 4; // keyFlags

    
    buffer_Uniform_Teapot = createUniformBuffer(teapotUniformBufferSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    const cubeUniformBufferSize = Float32Array.BYTES_PER_ELEMENT * 16; // MVP matrix

    buffer_Uniform_Cube = createUniformBuffer(cubeUniformBufferSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    // teapot bindgroup layout
    const bindGroupLayout_Uniform_Teapot = createBindGroupLayoutUniform(0, GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT, "uniform");

    // cube bindgroup layout
    const bindGroupLayout_Uniform_Cube = createBindGroupLayoutUniform(0, GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT, "uniform");

    
    const bindGroupLayout_Texture_And_Sampler = createBindGroupLayoutForTextureAndSampler("float", "2d", false, 0, GPUShaderStage.FRAGMENT, "filtering", 1, GPUShaderStage.FRAGMENT);

    // as both have linear same texture format will share same bindgroup layout for texture and sampler

    bindgroup_Uniform_Teapot = createBindGroupForUniform(buffer_Uniform_Teapot, 0, teapotUniformBufferSize, 0, bindGroupLayout_Uniform_Teapot);

    // cube bindgruop
    bindgroup_Uniform_Cube = createBindGroupForUniform(buffer_Uniform_Cube, 0, cubeUniformBufferSize, 0, bindGroupLayout_Uniform_Cube);

    bindgroup_Marble_Texture_And_Sampler = createBindGroupForTextureAndSampler(0, texture_Marble, 1, sampler_Marble, bindGroupLayout_Texture_And_Sampler);

    bindgroup_Teapot_Texture_And_Sampler = createBindGroupForTextureAndSampler(0, texture_Teapot_Color, 1, sampler_Teapot, bindGroupLayout_Texture_And_Sampler);

    // teapot pipeline layput

    const PipelineLayoutDescriptor_Teapot =
    {
        bindGroupLayouts: [bindGroupLayout_Uniform_Teapot, bindGroupLayout_Texture_And_Sampler]
    };

    const PipelineLayout_Teapot = Device.createPipelineLayout(PipelineLayoutDescriptor_Teapot);

    if(PipelineLayout_Teapot == null)
    {
        console.log("initialise() : PipelineLayout_Teapot is not supported by your browser.\n");
        throw Error("initialise() : PipelineLayout_Teapot is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : PipelineLayout_Teapot is supported by your browser.\n");
    }

    // cube pipeline layout

    const PipelineLayoutDescriptor_Cube =
    {
        bindGroupLayouts: [bindGroupLayout_Uniform_Cube, bindGroupLayout_Texture_And_Sampler]
    };

    const PipelineLayout_Cube = Device.createPipelineLayout(PipelineLayoutDescriptor_Cube);

    if(PipelineLayout_Cube == null)
    {
        console.log("initialise() : PipelineLayout_Cube is not supported by your browser.\n");
        throw Error("initialise() : PipelineLayout_Cube is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : PipelineLayout_Cube is supported by your browser.\n");
    }


    // Vertex Attitributes



    const positionVertexAttribute =  // shared between teapot and cube
    {
        shaderLocation: 0, // this matches with @location(0) in vertex shader
        offset: 0,
        format: "float32x3"
    };

    const normalVertexAttribute = // for teapot only
    {
        shaderLocation: 1, // this matches with @location(1) in vertex shader
        offset: 0,
        format: "float32x3"
    };

    const texcoordVertexAttribute_Teapot =
    {
        shaderLocation: 2, // this matches with @location(2) in vertex shader
        offset: 0,
        format: "float32x2"
    };

    const texcoordVertexAttribute_Cube =
    {
        shaderLocation: 1, // this matches with @location(1) in vertex shader
        offset: 0,
        format: "float32x2"
    };

    
    const positionVertexBufferLayout = // shared between teapot and cube
    {
        attributes: [positionVertexAttribute],
        arrayStride: Float32Array.BYTES_PER_ELEMENT * 3, // 3 floats of 4 bytes each
        stepMode: "vertex"
    };
    
    const normalVertexBufferLayout = // for teapot only
    {
        arrayStride: Float32Array.BYTES_PER_ELEMENT * 3, // 3 floats of 4 bytes each
        attributes: [normalVertexAttribute],
        stepMode: "vertex"
    };

    const texcoordVertexBufferLayout_Teapot =
    {
        attributes: [texcoordVertexAttribute_Teapot],
        arrayStride: Float32Array.BYTES_PER_ELEMENT * 2, // 2 floats of 4 bytes each
        stepMode: "vertex"
    };

    const texcoordVertexBufferLayout_Cube =
    {
        attributes: [texcoordVertexAttribute_Cube],
        arrayStride: Float32Array.BYTES_PER_ELEMENT * 2, // 2 floats of 4 bytes each
        stepMode: "vertex"
    };
    
    const vertexShaderState_Teapot =
    {
        module: shaderModule_TeapotVertexShader,
        entryPoint: "main",
        buffers: [positionVertexBufferLayout,normalVertexBufferLayout,texcoordVertexBufferLayout_Teapot]
    };

    const vertexShaderState_Cube =
    {
        module: shaderModule_CubeVertexShader,
        entryPoint: "main",
        buffers: [positionVertexBufferLayout,texcoordVertexBufferLayout_Cube]
    };

    // color target state

    const colorTargetState_Teapot =
    {
        format: "rgba8unorm",
    };

    const colorTargetState_Cube =
    {
        format: canvasFormat,
    };

    // fragment shader state
    const fragmentShaderState_Teapot =
    {
        module: shaderModule_TeapotFragmentShader,
        entryPoint: "main",
        targets: [colorTargetState_Teapot]
    };

    const fragmentShaderState_Cube =
    {
        module: shaderModule_CubeFragmentShader,
        entryPoint: "main",
        targets: [colorTargetState_Cube]
    };

    const premitiveState =  // shared between teapot and cube
    {
        frontFace: "cw",
        cullMode: "none",
        topology: "triangle-list"
    };

    // depth stencil state
    const depthStencilState =  // shared between teapot and cube
    {
        format: "depth24plus-stencil8",
        depthWriteEnabled: true,
        depthCompare: "less-equal"
    };

    // create final pipeline descriptor for teapot
    const pipelineDescriptor_Teapot =
    {
        layout: PipelineLayout_Teapot,
        vertex: vertexShaderState_Teapot,
        fragment: fragmentShaderState_Teapot,
        primitive: premitiveState,
        depthStencil: depthStencilState,
    };

    renderPipeline_Teapot = Device.createRenderPipeline(pipelineDescriptor_Teapot);

    if(renderPipeline_Teapot == null)
    {
        console.log("initialise() : renderPipeline_Teapot is not supported by your browser.\n");
        throw Error("initialise() : renderPipeline_Teapot is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : renderPipeline_Teapot is supported by your browser.\n");
    }

    // create final pipeline descriptor for cube
    const pipelineDescriptor_Cube =
    {
        layout: PipelineLayout_Cube,
        vertex: vertexShaderState_Cube,
        fragment: fragmentShaderState_Cube,
        primitive: premitiveState,
        depthStencil: depthStencilState,
    };

    renderPipeline_Cube = Device.createRenderPipeline(pipelineDescriptor_Cube);

    if(renderPipeline_Cube == null)
    {
        console.log("initialise() : renderPipeline_Cube is not supported by your browser.\n");
        throw Error("initialise() : renderPipeline_Cube is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : renderPipeline_Cube is supported by your browser.\n");
    }

    // white background for black cube
    clearColor = { r: 1.0, g: 1.0, b: 1.0, a: 1.0 };

    // define the clearcolor - BLACK background
    clearColor_FBO = { r: 0.0, g: 0.0, b: 0.0, a: 1.0 };

    // initialise perspective projection matrix
    perspectiveProjectionMatrix = mat4.create();

    perspectiveProjectionMatrix_FBO = mat4.create();

    // Initliase teapot/Fbos  projecttion matrix onces
    mat4.perspective(perspectiveProjectionMatrix_FBO, 45.0 * Math.PI / 180.0, FBO_WIDTH / FBO_HEIGHT, 0.1, 100.0);

}

// USER DEFINED FUNCTIONS (3 UDFs)

function createBindGroupLayoutUniform(_bindingIndex, _shaderStageVisibility, _UniformType)
{
    // code
    const bindGroupLayoutEntry =
    {
        binding: _bindingIndex,
        visibility: _shaderStageVisibility,
        buffer:
        {
            type: _UniformType
        }
    };

    const bindGroupLayoutDescriptor = 
    {
        entries: [bindGroupLayoutEntry]
    };

    const bindGroupLayout = Device.createBindGroupLayout(bindGroupLayoutDescriptor);
    if(bindGroupLayout == null)
    {
        console.log("createBindGroupLayoutUniform() : bindGroupLayout is not supported by your browser.\n");
        throw Error("createBindGroupLayoutUniform() : bindGroupLayout is not supported by your browser.\n");
    }

    return bindGroupLayout;
}

function createUniformBuffer(_uniformBufferSize, _uniformBufferUsage)
{
    // code
    const bufferDescriptor =
    {
        size: _uniformBufferSize,
        usage: _uniformBufferUsage
    };

    const uniformBuffer = Device.createBuffer(bufferDescriptor);
    if(uniformBuffer == null)
    {
        console.log("createUniformBuffer() : uniformBuffer is not supported by your browser.\n");
        throw Error("createUniformBuffer() : uniformBuffer is not supported by your browser.\n");
    }

    return uniformBuffer;
}

function createBindGroupForUniform(_uniformBuffer, _uniformBufferOffset, _uniformBufferSize, _bindingIndex, _bindGroupLayout)
{
    //code
    const bufferBinding = 
    {
        buffer: _uniformBuffer,
        offset: _uniformBufferOffset,
        size: _uniformBufferSize
    };

    const bindGroupEntry =
    {
        binding: _bindingIndex,
        resource: bufferBinding
    };

    const bindGroupDescriptor =
    {
        layout: _bindGroupLayout,
        entries: [bindGroupEntry]
    };

    const bindGroup = Device.createBindGroup(bindGroupDescriptor);
    if(bindGroup == null)
    {
        console.log("createBindGroupForUniform() : bindGroup is not supported by your browser.\n");
        throw Error("createBindGroupForUniform() : bindGroup is not supported by your browser.\n");
    }

    return bindGroup;
}

// create vertex bufer UDF
function createVertexBuffer(_vertexData)
{
    // code

    const vertexBufferDescriptor =
    {
        size: _vertexData.byteLength,
        usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST
    };

    const buffer = Device.createBuffer(vertexBufferDescriptor);
    if(buffer == null)
    {
       return null;
    }

    Queue.writeBuffer(buffer, 0, _vertexData, 0, _vertexData.length);

    return buffer;
}

// create index buffer UDF
function createIndexBuffer(_indexData)
{
    // code
    const indexBufferDescriptor =
    {
        size: _indexData.byteLength,
        usage: GPUBufferUsage.INDEX | GPUBufferUsage.COPY_DST
    };

    const buffer = Device.createBuffer(indexBufferDescriptor);
    if(buffer == null)
    {
        return null;
    }

    Queue.writeBuffer(buffer, 0, _indexData, 0, _indexData.length);

    return buffer;
}

async function loadTexture(_imageFileName)
{
    // code
    const image = new Image();
    image.src = _imageFileName;

    await image.decode();

    const imageBitmap = await createImageBitmap(image);

    if(imageBitmap == null)
    {
        throw Error("loadTexture() : imageBitmap is not supported by your browser.\n");
        console.log("loadTexture() : imageBitmap is not supported by your browser.\n");
    }
    else
    {
        console.log("loadTexture() : imageBitmap is supported by your browser.\n");
    }

    // now create texture descriptor based on above image bitmap
    const textureDescriptor = 
    {
        size: [imageBitmap.width, imageBitmap.height, 1],
        dimension : "2d",
        format: "rgba8unorm",
        usage: GPUTextureUsage.TEXTURE_BINDING | GPUTextureUsage.COPY_DST | GPUTextureUsage.RENDER_ATTACHMENT
    };

    const _texture = Device.createTexture(textureDescriptor);
    if(_texture == null)
    {
        throw Error("loadTexture() : _texture is not supported by your browser.\n");
        console.log("loadTexture() : _texture is not supported by your browser.\n");
    }
    else
    {
        console.log("loadTexture() : _texture is supported by your browser.\n");
    }

    const copySource = 
    {
        source: imageBitmap,
        flipY: true
    }

    const copyDestination = 
    {
        texture: _texture,
        mipLevel: 0
    }

    Queue.copyExternalImageToTexture(copySource, copyDestination, textureDescriptor.size);

    return _texture;

}

function createBindGroupLayoutForTextureAndSampler(_textureSampleType, _textureViewDimension, _isTextureMultisampled, _textureBindingIndex, _textureShaderStageVisibility, _samplerType, _samplerBindingIndex, _samplerShaderStageVisibility)
{
    // code

    // create binding layout
    const bindingLayout_Texture = 
    {
        sampleType: _textureSampleType,
        viewDimension: _textureViewDimension,
        multisampled: _isTextureMultisampled
    }

    // now create bindgroup layout entry
    const bindGroupLayoutEntry_Texture =
    {
        binding: _textureBindingIndex,
        visibility: _textureShaderStageVisibility,
        texture: bindingLayout_Texture
    }

    // now create binding layout for sampler
    const bindingLayout_Sampler =
    {
        type: _samplerType
    }

    // now create bindgroup layout entry for sampler
    const bindGroupLayoutEntry_Sampler =
    {
        binding: _samplerBindingIndex,
        visibility: _samplerShaderStageVisibility,
        sampler: bindingLayout_Sampler
    }

   // now create bindgroup layout descriptor
   const bindGroupLayoutDescriptor =
   {
        entries: [bindGroupLayoutEntry_Texture, bindGroupLayoutEntry_Sampler]
   }

   // now create the final bindgroup layout
    const bindGroupLayout = Device.createBindGroupLayout(bindGroupLayoutDescriptor);

    if(bindGroupLayout == null)
    {
        throw Error("createBindGroupLayoutForTextureAndSampler() : bindGroupLayout is not supported by your browser.\n");
        console.log("createBindGroupLayoutForTextureAndSampler() : bindGroupLayout is not supported by your browser.\n");
    }
    else
    {
        console.log("createBindGroupLayoutForTextureAndSampler() : bindGroupLayout is supported by your browser.\n");
    }

    return bindGroupLayout; 
}

function createBindGroupForTextureAndSampler(_textureBindingIndex, _texture, _samplerBindingIndex, _sampler, _bindGroupLayout)
{
    // code

    // create bindgroup entry for texture
    const bindGroupEntry_Texture =
    {
        binding: _textureBindingIndex,
        resource: _texture.createView()
    }

    // create bindgroup entry for sampler
    const bindGroupEntry_Sampler =
    {
        binding: _samplerBindingIndex,
        resource: _sampler
    }

    // create the final bindgroup
    const bindGroupDescriptor =
    {
        layout: _bindGroupLayout,
        entries: [bindGroupEntry_Texture, bindGroupEntry_Sampler]
    };

    const bindGroup = Device.createBindGroup(bindGroupDescriptor);

    if(bindGroup == null)
    {
        throw Error("createBindGroupForTextureAndSampler() : bindGroup is not supported by your browser.\n");
        console.log("createBindGroupForTextureAndSampler() : bindGroup is not supported by your browser.\n");
    }
    else
    {
        console.log("createBindGroupForTextureAndSampler() : bindGroup is supported by your browser.\n");
    }

    return bindGroup;
}


function resize() 
{
    // code
    if (bFullScreen == true) 
    {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else 
    {
        canvas.width = CANVAS_ORIGINAL_WIDTH;
        canvas.height = CANVAS_ORIGINAL_HEIGHT;
    }

    // depth texture
    if(Device != null)
    {
        if(depthTexture !=null)
        {
            depthTexture.destroy();
            depthTexture = null;
        }

        // to crate depth texture we need depth texture descriptopr
        const depthTextureDescriptor =
        {
            size: [canvas.width, canvas.height, 1],
            dimension : "2d",
            format: "depth24plus-stencil8",
            usage: GPUTextureUsage.RENDER_ATTACHMENT | GPUTextureUsage.COPY_SRC
        };

        // now create depth texture using above depth texture descriptor
        depthTexture = Device.createTexture(depthTextureDescriptor);
        if(depthTexture == null)
        {
            console.log("resize() : depthTexture is not supported by your browser.\n");
            throw Error("resize() : depthTexture is not supported by your browser.\n");
        }

    }

    // initilase perspective prokecttion matrix
    mat4.perspective(perspectiveProjectionMatrix, 45.0 * Math.PI / 180.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
}

function display() 
{
    // code

    // device may be lost , initialisation may not be done yet
    if(Device == null)
    {
        return;
    }

    // get command encoder from device due to async nature and possiblelity of device lost its better to create it perframe in display
    const commandEncoder = Device.createCommandEncoder();
    if(commandEncoder == null)
    {
        console.log("display() : commandEncoder is not supported by your browser.\n");
        throw Error("display() : commandEncoder is not supported by your browser.\n");
    }

    var teapotModelMatrix = mat4.create();
    var teapotViewMatrix = mat4.create();

    mat4.translate(teapotModelMatrix, teapotModelMatrix, [0.0, 0.0, -1.5]);

    mat4.rotateY(teapotModelMatrix, teapotModelMatrix, degToRad(angle_Teapot));


    // toogle lighting
    if(isLightingEnabled == true)
    {
       keyFlags[0] = 1;
    }
    else
    {
        keyFlags[0] = 0;
    }

    // texure
    if(isTexturingEnabled == true)
    {
       keyFlags[1] = 1;
    }
    else
    {
        keyFlags[1] = 0;
    }

    // twleve write buffer calls to fill the one unified buffer targeting indiviual byte offest as we saw while intialising myuniform buffer size
    
    // one : matrices
    Queue.writeBuffer(buffer_Uniform_Teapot, 0, teapotModelMatrix, 0, teapotModelMatrix.length);  // from 0th to 63rd
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 16, teapotViewMatrix, 0, teapotViewMatrix.length); // from 64th to 127th
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 32 , perspectiveProjectionMatrix_FBO, 0, perspectiveProjectionMatrix_FBO.length); // from 128th to 191st
    
    // two : lights
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 48 , lightAmbient, 0, lightAmbient.length); // from 192th to 207th
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 52 , lightDiffuse, 0, lightDiffuse.length);
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 56 , lightSpecular, 0, lightSpecular.length);
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 60 , lightPosition, 0, lightPosition.length);

    // three : material
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 64 , materialAmbient, 0, materialAmbient.length);
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 68 , materialDiffuse, 0, materialDiffuse.length);
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 72 , materialSpecular, 0, materialSpecular.length);
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 76 , materialShininess, 0, materialShininess.length);

    // four: light toggle
    Queue.writeBuffer(buffer_Uniform_Teapot, Float32Array.BYTES_PER_ELEMENT * 80 , keyFlags, 0, keyFlags.length);


    // cube transformation matrices
    var cubeModelMatrix = mat4.create();
    const cubeMVPMatrix = mat4.create();

    mat4.translate(cubeModelMatrix, cubeModelMatrix, [0.0, 0.0, -5.0]);
    mat4.rotateX(cubeModelMatrix, cubeModelMatrix, degToRad(angle_Cube));
    mat4.rotateY(cubeModelMatrix, cubeModelMatrix, degToRad(angle_Cube));
    mat4.rotateZ(cubeModelMatrix, cubeModelMatrix, degToRad(angle_Cube));

    mat4.multiply(cubeMVPMatrix, perspectiveProjectionMatrix, cubeModelMatrix);

    Queue.writeBuffer(buffer_Uniform_Cube, 0, cubeMVPMatrix, 0, cubeMVPMatrix.length);

    const teapotColorAttachment =
    {
        view: texture_Teapot_Color.createView(),
        clearValue: clearColor_FBO,
        loadOp: "clear",
        storeOp: "store"
    };

    const teapotDepthAttachment =
    {
        view: texture_Teapot_Depth.createView(),
        depthClearValue: 1.0,
        depthLoadOp: "clear",
        depthStoreOp: "store",
        stencilClearValue: 0,
        stencilLoadOp: "clear",
        stencilStoreOp: "store"
    };

    // according to above create renderpass descriptor of type GPURenderPassDescriptor
    const teapotRenderPassDescriptor = 
    {
        colorAttachments: [teapotColorAttachment],
        depthStencilAttachment: teapotDepthAttachment
    };

    // start the renderpass
    const teapotRenderPassEncoder = commandEncoder.beginRenderPass(teapotRenderPassDescriptor);

    teapotRenderPassEncoder.setPipeline(renderPipeline_Teapot);
    teapotRenderPassEncoder.setViewport(0, 0, FBO_WIDTH, FBO_HEIGHT, 0, 1); // MUST BE EQual to teapot texture size
    teapotRenderPassEncoder.setScissorRect(0, 0, FBO_WIDTH, FBO_HEIGHT);
    teapotRenderPassEncoder.setVertexBuffer(0, buffer_Teapot_Positions);
    teapotRenderPassEncoder.setVertexBuffer(1, buffer_Teapot_Normals);
    teapotRenderPassEncoder.setVertexBuffer(2, buffer_Teapot_Texcoords);
    teapotRenderPassEncoder.setIndexBuffer(buffer_Teapot_Elements, "uint16");
    teapotRenderPassEncoder.setBindGroup(0, bindgroup_Uniform_Teapot);
    teapotRenderPassEncoder.setBindGroup(1, bindgroup_Marble_Texture_And_Sampler);
    teapotRenderPassEncoder.drawIndexed(numTeapotIndices); 

    // end the renderpass
    teapotRenderPassEncoder.end();


    // cube render pass

    const cubeColorAttachment =
    {
        view: Context.getCurrentTexture().createView(),
        clearValue: clearColor,
        loadOp: "clear",
        storeOp: "store"
    };

     const cubeDepthAttachment =
    {
        view: depthTexture.createView(),
        depthClearValue: 1.0,
        depthLoadOp: "clear",
        depthStoreOp: "store",
        stencilClearValue: 0,
        stencilLoadOp: "clear",
        stencilStoreOp: "store"
    };

     const cubeRenderPassDescriptor = 
    {
        colorAttachments: [cubeColorAttachment],
        depthStencilAttachment: cubeDepthAttachment
    };

    const cubeRenderPassEncoder = commandEncoder.beginRenderPass(cubeRenderPassDescriptor);

    cubeRenderPassEncoder.setPipeline(renderPipeline_Cube);
    cubeRenderPassEncoder.setViewport(0, 0, canvas.width, canvas.height, 0, 1);
    cubeRenderPassEncoder.setScissorRect(0, 0, canvas.width, canvas.height);
    cubeRenderPassEncoder.setVertexBuffer(0, buffer_Cube_Positions);
    cubeRenderPassEncoder.setVertexBuffer(1, buffer_Cube_Texcoords);
    cubeRenderPassEncoder.setBindGroup(0, bindgroup_Uniform_Cube);
    cubeRenderPassEncoder.setBindGroup(1, bindgroup_Teapot_Texture_And_Sampler);
    cubeRenderPassEncoder.draw(36);

    // end the renderpass
    cubeRenderPassEncoder.end();

    // finish the command encoder. there can be multiple command encoders we have one and submit it to the queue
    Queue.submit([commandEncoder.finish()]);

    // animation loop
    animationFrameId = requestAnimationFrame(display);

    update();

}

function update() 
{
    // code
    if(isTeapotAnimating == true)
    {
        angle_Teapot += 1.0;
        if(angle_Teapot >= 360.0)
        {
            angle_Teapot = 0.0;
        }

        angle_Cube += 1.0;
        if(angle_Cube >= 360.0)
        {
            angle_Cube = 0.0;
        }
    }
}

function uninitialise() 
{
    // code

    // use animation frame id for safe animation cancellation and uninitialise
    if(animationFrameId != null)
    {
        cancelAnimationFrame(animationFrameId);
        animationFrameId = null;
    }

    // destroy depth texture
    if(depthTexture != null)
    {
        depthTexture.destroy();
        depthTexture = null;
    }

    // unconfigure/ destroy the canvas context
    if(Context != null)
    {
        Context.unconfigure();
        Context = null;
    }

    if(texture_Teapot_Color != null)
    {
        texture_Teapot_Color.destroy();
        texture_Teapot_Color = null;
    }

    if(texture_Teapot_Depth != null)
    {
        texture_Teapot_Depth.destroy();
        texture_Teapot_Depth = null;
    }

    if(texture_Marble != null)
    {
        texture_Marble.destroy();
        texture_Marble = null;
    }

    // destroy the device
    if(Device != null)
    {
        Device.destroy();
        Device = null;
        Queue = null;
       texture_Teapot_Color = null;
        texture_Teapot_Depth = null;
        texture_Marble = null;
        sampler_Marble = null;
        sampler_Teapot = null;
        renderPipeline_Teapot = null;
        buffer_Uniform_Teapot = null;
        bindgroup_Uniform_Teapot = null;
        bindgroup_Marble_Texture_And_Sampler = null;

        buffer_Teapot_Positions = null;
        buffer_Teapot_Normals = null;
        buffer_Teapot_Texcoords = null;
        buffer_Teapot_Elements = null;

        // cube related varibles
        renderPipeline_Cube = null;
        buffer_Uniform_Cube = null;
        bindgroup_Uniform_Cube = null;
        bindgroup_Teapot_Texture_And_Sampler = null;

        buffer_Cube_Positions =  null;
        buffer_Cube_Texcoords =  null;

    }

    Sphere = null;

    perspectiveProjectionMatrix = null;

    console.log("uninitialise() : WebGPUDevice is destroyed successfully.\n");

}



