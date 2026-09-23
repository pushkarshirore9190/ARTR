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
var clearColor;
let Device = null;
let Context = null;
let Queue = null;
let canvasFormat = null;
let animationFrameId = null;


let renderPipeline = null;
let buffer_Uniform = null;
let bindingGroup_Uniform = null;
var lightAmbient = new Float32Array([0.1, 0.1, 0.1, 0.1]);
var lightDiffuse = new Float32Array([1.0, 1.0, 1.0, 0.0]);
var lightSpecular = new Float32Array([1.0, 1.0, 1.0, 0.0]);
var lightPosition = new Float32Array([0.0, 0.0, 2.0, 1.0]);
var materialAmbient = new Float32Array([0.0, 0.0, 0.0, 0.0]);
var materialDiffuse = new Float32Array([0.5, 0.2, 0.7, 0.0]);
var materialSpecular = new Float32Array([0.7, 0.7, 0.7, 0.0]);
var materialShininess = new Float32Array([128.0, 0.0, 0.0, 0.0]);


var isLightingEnabled = false;
let perspectiveProjectionMatrix = null;

let depthTexture = null;

// for cube
let buffer_Interlived = null;
let texture_Marble =  null;
let sampler_Marble = null;
let bindGroup_Texture_And_Sampler = null;

var angle_cube = 0.0;



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

    // FIX: initialise() is async and was being called without "await". That let main() race
    // ahead to resize() while initialise() was still running (still awaiting loadTexture(), etc.),
    // so perspectiveProjectionMatrix was still null when resize() called mat4.perspective() on it,
    // throwing "Cannot set properties of null". Awaiting here makes resize() wait until
    // initialise() has actually finished and perspectiveProjectionMatrix = mat4.create() has run.
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
    // FIX: console.warning() does not exist on the console object; it's console.warn().
    console.warn("WebGPUDeviceLostReason: ", info.reason , "WebGPUDeviceLostMessage: ", info.message);
    Device = null;
    Queue = null;
    renderPipeline = null;
    buffer_Uniform = null;
    bindingGroup_Uniform = null;
    perspectiveProjectionMatrix = null;
    depthTexture = null;
    buffer_Interlived = null;
    texture_Marble = null;
    sampler_Marble = null;
    bindGroup_Texture_And_Sampler = null;
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
            
        default:
            break;
    }
}


function mouseDown()
{
    //code
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

    // vertex shader code in WGSL
   const vertexShaderSourceCode =
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) Color: vec3<f32>,\n" +
    "    @location(1) Texcoords: vec2<f32>,\n" +
    "    @location(2) transformedNormal: vec3<f32>,\n" +
    "    @location(3) lightDirection: vec3<f32>,\n" +
    "    @location(4) viewerVector: vec3<f32>,\n" +
    "};\n" +

    "struct MyUniformData\n" +
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
    "};\n" +

    "@group(0) @binding(0)\n" +
    "var<uniform> uMyUniformData: MyUniformData;\n" +

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
    "fn main(@location(0) pos: vec3<f32>, @location(1) col: vec3<f32>, @location(2) normal: vec3<f32>, @location(3) texcoord: vec2<f32>) -> VertexOutput\n" +
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

    "    output.Position = " +
    "        uMyUniformData.projectionMatrix * " +
    "        uMyUniformData.viewMatrix * " +
    "        uMyUniformData.modelMatrix * " +
    "        vec4<f32>(pos, 1.0);\n" +

    "    output.Color = col;\n" +

    "    output.Texcoords = texcoord;\n" +

    "    return output;\n" +
    "}";


    const shaderModuleDescriptor_VertexShader = 
    {
        code: vertexShaderSourceCode
    };

    const shaderModule_VertexShader = Device.createShaderModule(shaderModuleDescriptor_VertexShader);

    if(shaderModule_VertexShader == null)
    {
        console.log("initialise() : shaderModule_VertexShader is not supported by your browser.\n");
        throw Error("initialise() : shaderModule_VertexShader is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : shaderModule_VertexShader is supported by your browser.\n");
    }


    // fragment shader code in WGSL
   const fragmentShaderSourceCode =
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) Color: vec3<f32>,\n" +
    "    @location(1) Texcoords: vec2<f32>,\n" +
    "    @location(2) transformedNormal: vec3<f32>,\n" +
    "    @location(3) lightDirection: vec3<f32>,\n" +
    "    @location(4) viewerVector: vec3<f32>,\n" +
    "};\n" +

    "struct MyUniformData\n" +
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
    "};\n" +

    "@group(0) @binding(0) var<uniform> uMyUniformData: MyUniformData;\n" +
    "@group(1) @binding(0) var myTexture2D: texture_2d<f32>;\n" +
    "@group(1) @binding(1) var mySampler: sampler;\n" +

    "@fragment\n" +
    "fn main(output: VertexOutput) -> @location(0) vec4<f32>\n" +
    "{\n" +
    "    var phong_ads_color: vec3<f32>;\n" +


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

    "        var tex = textureSample(myTexture2D, mySampler, output.Texcoords);\n" +

    "        var color = tex * vec4<f32>(output.Color, 1.0) * vec4<f32>(phong_ads_color, 1.0);\n" +

    "        return color;\n" +

    "}";

    const shaderModuleDescriptor_FragmentShader = 
    {
        code: fragmentShaderSourceCode
    };

    const shaderModule_FragmentShader = Device.createShaderModule(shaderModuleDescriptor_FragmentShader);
    
    if(shaderModule_FragmentShader == null)
    {
        console.log("initialise() : shaderModule_FragmentShader is not supported by your browser.\n");
        throw Error("initialise() : shaderModule_FragmentShader is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : shaderModule_FragmentShader is supported by your browser.\n");
    }

     const cube_pcnt = new Float32Array([
        // position            color             normal            texcoord
        // top surface
         1.0,  1.0, -1.0,    0.0, 1.0, 0.0,    0.0,  1.0, 0.0,    0.0, 0.0,
        -1.0,  1.0, -1.0,    0.0, 1.0, 0.0,    0.0,  1.0, 0.0,    1.0, 0.0,
        -1.0,  1.0,  1.0,    0.0, 1.0, 0.0,    0.0,  1.0, 0.0,    1.0, 1.0,
        -1.0,  1.0,  1.0,    0.0, 1.0, 0.0,    0.0,  1.0, 0.0,    1.0, 1.0,
         1.0,  1.0,  1.0,    0.0, 1.0, 0.0,    0.0,  1.0, 0.0,    0.0, 1.0,
         1.0,  1.0, -1.0,    0.0, 1.0, 0.0,    0.0,  1.0, 0.0,    0.0, 0.0,
        // bottom surface
         1.0, -1.0,  1.0,    1.0, 0.5, 0.0,    0.0, -1.0, 0.0,    0.0, 0.0,
        -1.0, -1.0,  1.0,    1.0, 0.5, 0.0,    0.0, -1.0, 0.0,    1.0, 0.0,
        -1.0, -1.0, -1.0,    1.0, 0.5, 0.0,    0.0, -1.0, 0.0,    1.0, 1.0,
        -1.0, -1.0, -1.0,    1.0, 0.5, 0.0,    0.0, -1.0, 0.0,    1.0, 1.0,
         1.0, -1.0, -1.0,    1.0, 0.5, 0.0,    0.0, -1.0, 0.0,    0.0, 1.0,
         1.0, -1.0,  1.0,    1.0, 0.5, 0.0,    0.0, -1.0, 0.0,    0.0, 0.0,
        // front surface
         1.0,  1.0,  1.0,    1.0, 0.0, 0.0,    0.0, 0.0,  1.0,    0.0, 0.0,
        -1.0,  1.0,  1.0,    1.0, 0.0, 0.0,    0.0, 0.0,  1.0,    1.0, 0.0,
        -1.0, -1.0,  1.0,    1.0, 0.0, 0.0,    0.0, 0.0,  1.0,    1.0, 1.0,
        -1.0, -1.0,  1.0,    1.0, 0.0, 0.0,    0.0, 0.0,  1.0,    1.0, 1.0,
         1.0, -1.0,  1.0,    1.0, 0.0, 0.0,    0.0, 0.0,  1.0,    0.0, 1.0,
         1.0,  1.0,  1.0,    1.0, 0.0, 0.0,    0.0, 0.0,  1.0,    0.0, 0.0,
        // back surface
         1.0, -1.0, -1.0,    1.0, 1.0, 0.0,    0.0, 0.0, -1.0,    0.0, 0.0,
        -1.0, -1.0, -1.0,    1.0, 1.0, 0.0,    0.0, 0.0, -1.0,    1.0, 0.0,
        -1.0,  1.0, -1.0,    1.0, 1.0, 0.0,    0.0, 0.0, -1.0,    1.0, 1.0,
        -1.0,  1.0, -1.0,    1.0, 1.0, 0.0,    0.0, 0.0, -1.0,    1.0, 1.0,
         1.0,  1.0, -1.0,    1.0, 1.0, 0.0,    0.0, 0.0, -1.0,    0.0, 1.0,
         1.0, -1.0, -1.0,    1.0, 1.0, 0.0,    0.0, 0.0, -1.0,    0.0, 0.0,
        // left surface
        -1.0,  1.0,  1.0,    0.0, 0.0, 1.0,   -1.0, 0.0, 0.0,    0.0, 0.0,
        -1.0,  1.0, -1.0,    0.0, 0.0, 1.0,   -1.0, 0.0, 0.0,    1.0, 0.0,
        -1.0, -1.0, -1.0,    0.0, 0.0, 1.0,   -1.0, 0.0, 0.0,    1.0, 1.0,
        -1.0, -1.0, -1.0,    0.0, 0.0, 1.0,   -1.0, 0.0, 0.0,    1.0, 1.0,
        -1.0, -1.0,  1.0,    0.0, 0.0, 1.0,   -1.0, 0.0, 0.0,    0.0, 1.0,
        -1.0,  1.0,  1.0,    0.0, 0.0, 1.0,   -1.0, 0.0, 0.0,    0.0, 0.0,
        // right surface
         1.0,  1.0, -1.0,    1.0, 0.0, 1.0,    1.0, 0.0, 0.0,    0.0, 0.0,
         1.0,  1.0,  1.0,    1.0, 0.0, 1.0,    1.0, 0.0, 0.0,    1.0, 0.0,
         1.0, -1.0,  1.0,    1.0, 0.0, 1.0,    1.0, 0.0, 0.0,    1.0, 1.0,
         1.0, -1.0,  1.0,    1.0, 0.0, 1.0,    1.0, 0.0, 0.0,    1.0, 1.0,
         1.0, -1.0, -1.0,    1.0, 0.0, 1.0,    1.0, 0.0, 0.0,    0.0, 1.0,
         1.0,  1.0, -1.0,    1.0, 0.0, 1.0,    1.0, 0.0, 0.0,    0.0, 0.0
    ]);


    // vertex buffer for above interlived array
    buffer_Interlived = createVertexBuffer(cube_pcnt);

    if(buffer_Interlived == null)
    {
        console.log("initialise() : buffer_Interlived creation failed.");
        throw Error("buffer_Interlived creation failed.");
    }
    else
    {
        console.log("initialise() : buffer_Interlived creation succeeded.");
    }


    // using UDF 1: createBindGroupLayoutUniform
    // using UDF 1: createBindGroupLayoutUniform
    const bindGroupLayout_Uniform = createBindGroupLayoutUniform(0, GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT, "uniform");

    // create bind group layout for texture and sampler
    const bindGroupLayout_Texture_And_Sampler = createBindGroupLayoutForTextureAndSampler(
        "float",
        "2d",
        false,
        0,
        GPUShaderStage.FRAGMENT,
        "filtering",
        1,
        GPUShaderStage.FRAGMENT
    );

    // create pipeline layout for mvp uniform

    const pipelineLayoutDescriptor = 
    {
        bindGroupLayouts: [
            bindGroupLayout_Uniform,
            bindGroupLayout_Texture_And_Sampler
        ]
    };

    const pipelineLayout = Device.createPipelineLayout(pipelineLayoutDescriptor);

    if(pipelineLayout == null)
    {
        console.log("initialise() : pipelineLayout is not supported by your browser.\n");
        throw Error("initialise() : pipelineLayout is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : pipelineLayout is supported by your browser.\n");
    }

    // create uniform buffer for mvp matrix using UDF 2: createUniformBuffer
    const myUniformBufferSize = Float32Array.BYTES_PER_ELEMENT * 16 * 3 + // from 64th to 127th
                                Float32Array.BYTES_PER_ELEMENT * 4 * 8 ; // from 128th to 191st


    buffer_Uniform = createUniformBuffer(myUniformBufferSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    // using UDF 3: createBindGroupForUniform
    bindingGroup_Uniform = createBindGroupForUniform(buffer_Uniform, 0, myUniformBufferSize, 0, bindGroupLayout_Uniform);

     // load the image to create the texture
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

    // texture sampler descriptor
    const samplerDescriptor = 
    {
        magFilter: "linear",
        minFilter: "linear",
    }

    // create texture sampler
    sampler_Marble = Device.createSampler(samplerDescriptor);

    if(sampler_Marble == null)
    {
        console.log("initialise() : sampler_Marble is not supported by your browser.\n");
        throw Error("initialise() : sampler_Marble is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : sampler_Marble is supported by your browser.\n"); 
    }

    // create texure and sampler bind group
    bindGroup_Texture_And_Sampler = createBindGroupForTextureAndSampler(
        0,
        texture_Marble,
        1,
        sampler_Marble,
        bindGroupLayout_Texture_And_Sampler
    );

    // interlibed buffer attirbutes pcnt
    const positionVertexAttribute =
    {
        shaderLocation: 0, // this matches with @location(0) in vertex shader
        offset: 0,
        format: "float32x3",
    };

    const colorVertexAttribute =
    {
        shaderLocation: 1, // this matches with @location(1) in vertex shader
        offset: Float32Array.BYTES_PER_ELEMENT * 3,
        format: "float32x3",
    };

    const normalVertexAttribute =
    {
        shaderLocation: 2, // this matches with @location(2) in vertex shader
        offset: Float32Array.BYTES_PER_ELEMENT * 6,
        format: "float32x3",
    };

    const texcoordVertexAttribute =
    {
        shaderLocation: 3, // this matches with @location(3) in vertex shader
        offset: Float32Array.BYTES_PER_ELEMENT * 9,
        format: "float32x2",
    };

    const interlibedVertexBufferLayout =
    {
        attributes: [positionVertexAttribute, colorVertexAttribute, normalVertexAttribute, texcoordVertexAttribute],
        arrayStride: Float32Array.BYTES_PER_ELEMENT * 11, // 11 floats of 4 bytes each
        stepMode: "vertex"
    };

    const vertexShaderState =
    {
        module: shaderModule_VertexShader,
        entryPoint: "main",
        buffers: [interlibedVertexBufferLayout]
    };

    const colorTargetState =
    {
        format: canvasFormat,
    };

    // fragment shader state
    const fragmentShaderState =
    {
        module: shaderModule_FragmentShader,
        entryPoint: "main",
        targets: [colorTargetState]
    };

    const premitiveState =
    {
        frontFace: "ccw",
        cullMode: "none",
        topology: "triangle-list"
    };

    // depth stencil state
    const depthStencilState =
    {
        format: "depth24plus-stencil8",
        depthWriteEnabled: true,
        depthCompare: "less-equal"
    };

    // create final pipeline descriptor
    const pipelineDescriptor =
    {
        layout: pipelineLayout,
        vertex: vertexShaderState,
        fragment: fragmentShaderState,
        primitive: premitiveState,
        depthStencil: depthStencilState,
    };

    renderPipeline = Device.createRenderPipeline(pipelineDescriptor);

    if(renderPipeline == null)
    {
        console.log("initialise() : renderPipeline is not supported by your browser.\n");
        throw Error("initialise() : renderPipeline is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : renderPipeline is supported by your browser.\n");
    }

    // initialise perspective projection matrix
    perspectiveProjectionMatrix = mat4.create();


    // define the clearcolor - BLACK background
    clearColor = { r: 0.0, g: 0.0, b: 0.0, a: 1.0 };

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

function degToRad(degree)
{
    //code
    return (degree * Math.PI / 180.0);
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

    // now create remderpass depth attachment
    const renderPassDepthAttachment =
    {
        view: depthTexture.createView(),
        depthClearValue: 1.0,
        depthLoadOp: "clear",
        depthStoreOp: "store",
        stencilClearValue: 0,
        stencilLoadOp: "clear",
        stencilStoreOp: "store"
        
    }

    // create renderpass color attachment of type GPURenderPassColorAttachment
    const renderPassColorAttachment = 
    {
        view: Context.getCurrentTexture().createView(),
        clearValue: clearColor,
        loadOp: "clear",
        storeOp: "store"
    };

    // according to above create renderpass descriptor of type GPURenderPassDescriptor
    const renderPassDescriptor = 
    {
        colorAttachments: [renderPassColorAttachment],
        depthStencilAttachment: renderPassDepthAttachment
    };

    const modelMatrix = mat4.create();
    const viewMatrix = mat4.create();
    const projectionMatrix = mat4.create(); // Fixed capitalization consistency

    // Apply transformations sequentially to the modelMatrix
    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -4.0]);
    mat4.scale(modelMatrix, modelMatrix, [0.75, 0.75, 0.75]);
    mat4.rotateX(modelMatrix, modelMatrix, degToRad(angle_cube));
    mat4.rotateY(modelMatrix, modelMatrix, degToRad(angle_cube));
    mat4.rotateZ(modelMatrix, modelMatrix, degToRad(angle_cube));

    // FIX: projectionMatrix was left as an identity matrix and never received
    // the real perspective matrix computed in resize(). Copy it in here.
    mat4.copy(projectionMatrix, perspectiveProjectionMatrix);


    // twleve write buffer calls to fill the one unified buffer targeting indiviual byte offest as we saw while intialising myuniform buffer size
    
    // one : matrices
    Queue.writeBuffer(buffer_Uniform, 0, modelMatrix, 0, modelMatrix.length);  // from 0th to 63rd
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16, viewMatrix, 0, viewMatrix.length); // from 64th to 127th
    // FIX: was referencing an undeclared variable "modelViewProjectionMatrix", which threw a
    // ReferenceError every frame and prevented anything from being drawn. This slot holds the
    // projection matrix (matches the uMyUniformData.projectionMatrix field in the shader).
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 + Float32Array.BYTES_PER_ELEMENT * 16 , projectionMatrix, 0, projectionMatrix.length); // from 128th to 191st
    
    // two : lights
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 , lightAmbient, 0, lightAmbient.length); // from 192th to 207th
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 1 , lightDiffuse, 0, lightDiffuse.length);
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 2 , lightSpecular, 0, lightSpecular.length);
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 3 , lightPosition, 0, lightPosition.length);

    // three : material
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 4 , materialAmbient, 0, materialAmbient.length);
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 5 , materialDiffuse, 0, materialDiffuse.length);
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 6 , materialSpecular, 0, materialSpecular.length);
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 7 , materialShininess, 0, materialShininess.length);


    // start the renderpass
    const renderPassEncoder = commandEncoder.beginRenderPass(renderPassDescriptor);

    renderPassEncoder.setPipeline(renderPipeline);
    renderPassEncoder.setViewport(0, 0, canvas.width, canvas.height, 0, 1);
    renderPassEncoder.setScissorRect(0, 0, canvas.width, canvas.height);
    renderPassEncoder.setVertexBuffer(0, buffer_Interlived);
    renderPassEncoder.setBindGroup(0, bindingGroup_Uniform);
    renderPassEncoder.setBindGroup(1, bindGroup_Texture_And_Sampler);
    renderPassEncoder.draw(36); // 6 vertices for rectangle (2 triangles)

    // end the renderpass
    renderPassEncoder.end();

    // finish the command encoder. there can be multiple command encoders we have one and submit it to the queue
    Queue.submit([commandEncoder.finish()]);

    // animation loop
    animationFrameId = requestAnimationFrame(display);

    update();

}

function update() 
{
    // code
    angle_cube = angle_cube + 1.5;
    if(angle_cube >= 360.0)
    {
        angle_cube = angle_cube - 360.0;
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

    // destroy texture and sampler
    if(texture_Marble != null)
    {
        texture_Marble.destroy();
        texture_Marble = null;
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

    // destroy the device
    if(Device != null)
    {
        Device.destroy();
        Device = null;
        Queue = null;
        renderPipeline = null;
        buffer_Uniform = null;
        bindingGroup_Uniform = null;
        buffer_Interlived = null;
        sampler_Marble = null;
        bindGroup_Texture_And_Sampler = null;
    }

    // FIX: removed "Sphere = null;" here — Sphere was never declared anywhere in this file,
    // so this was leftover code (probably pasted from a different demo) that just created a
    // stray implicit global. Nothing in this cube program uses it.

    perspectiveProjectionMatrix = null;

    console.log("uninitialise() : WebGPUDevice is destroyed successfully.\n");

}



