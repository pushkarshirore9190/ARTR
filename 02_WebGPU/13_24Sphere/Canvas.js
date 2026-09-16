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
let buffer_Material = null;
let bindGroup_Material = null;
var lightAmbient = new Float32Array([0.1, 0.1, 0.1, 0.1]);
var lightDiffuse = new Float32Array([1.0, 1.0, 1.0, 0.0]);
var lightSpecular = new Float32Array([1.0, 1.0, 1.0, 0.0]);
var lightPosition = new Float32Array([100.0, 100.0, 100.0, 1.0]);
var lKeyIsPressed = new Uint32Array([0,0,0,0]);


var isLightingEnabled = false;
var accessChoosen = " ";
var lightAngle = 0.0;
var materialTable =
[
    // row 1
    { name: "emerald",        ambient: [0.0215,    0.1745,    0.0215,    1.0], diffuse: [0.07568,    0.61424,    0.07568,    1.0], specular: [0.633,    0.727811, 0.633,    1.0], shininess: 0.6        * 128.0 },
    { name: "brass",          ambient: [0.329412,  0.223529,  0.027451,  1.0], diffuse: [0.780392,   0.568627,   0.113725,   1.0], specular: [0.992157, 0.941176, 0.807843, 1.0], shininess: 0.21794872 * 128.0 },
    { name: "black plastic",  ambient: [0.0,       0.0,       0.0,       1.0], diffuse: [0.01,       0.01,       0.01,       1.0], specular: [0.5,      0.5,      0.5,      1.0], shininess: 0.25       * 128.0 },
    { name: "black rubber",   ambient: [0.02,      0.02,      0.02,      1.0], diffuse: [0.01,       0.01,       0.01,       1.0], specular: [0.4,      0.4,      0.4,      1.0], shininess: 0.78125    * 128.0 },
    // row 2
    { name: "jade",           ambient: [0.135,     0.2225,    0.1575,    1.0], diffuse: [0.54,       0.89,       0.63,       1.0], specular: [0.316228, 0.316228, 0.316228, 1.0], shininess: 0.1        * 128.0 },
    { name: "bronze",         ambient: [0.2125,    0.1275,    0.054,     1.0], diffuse: [0.714,      0.4284,     0.18144,    1.0], specular: [0.393548, 0.271906, 0.166721, 1.0], shininess: 0.2        * 128.0 },
    { name: "cyan plastic",   ambient: [0.0,       0.1,       0.06,      1.0], diffuse: [0.0,        0.50980392, 0.50980392, 1.0], specular: [0.50196078, 0.50196078, 0.50196078, 1.0], shininess: 0.25 * 128.0 },
    { name: "cyan rubber",    ambient: [0.0,       0.05,      0.05,      1.0], diffuse: [0.4,        0.5,        0.5,        1.0], specular: [0.04,     0.7,      0.7,      1.0], shininess: 0.078125   * 128.0 },
    // row 3
    { name: "obsidian",       ambient: [0.05375,   0.05,      0.06625,   1.0], diffuse: [0.18275,    0.17,       0.22525,    1.0], specular: [0.332741, 0.328634, 0.346435, 1.0], shininess: 0.3        * 128.0 },
    { name: "chrome",         ambient: [0.25,      0.25,      0.25,      1.0], diffuse: [0.4,        0.4,        0.4,        1.0], specular: [0.774597, 0.774597, 0.774597, 1.0], shininess: 0.6        * 128.0 },
    { name: "green plastic",  ambient: [0.0,       0.0,       0.0,       1.0], diffuse: [0.1,        0.35,       0.1,        1.0], specular: [0.45,     0.55,     0.45,     1.0], shininess: 0.25       * 128.0 },
    { name: "green rubber",   ambient: [0.0,       0.05,      0.0,       1.0], diffuse: [0.4,        0.5,        0.4,        1.0], specular: [0.04,     0.7,      0.04,     1.0], shininess: 0.0788125  * 128.0 },
    // row 4
    { name: "pearl",          ambient: [0.25,      0.20725,   0.20725,   1.0], diffuse: [1.0,        0.829,      0.829,      1.0], specular: [0.296648, 0.296648, 0.296648, 1.0], shininess: 0.088      * 128.0 },
    { name: "copper",         ambient: [0.19125,   0.0735,    0.0225,    1.0], diffuse: [0.7038,     0.27048,    0.0828,     1.0], specular: [0.256777, 0.137622, 0.086014, 1.0], shininess: 0.1        * 128.0 },
    { name: "red plastic",    ambient: [0.0,       0.0,       0.0,       1.0], diffuse: [0.5,        0.0,        0.0,        1.0], specular: [0.7,      0.6,      0.6,      1.0], shininess: 0.25       * 128.0 },
    { name: "red rubber",     ambient: [0.05,      0.0,       0.0,       1.0], diffuse: [0.5,        0.4,        0.4,        1.0], specular: [0.7,      0.04,     0.04,     1.0], shininess: 0.078125   * 128.0 },
    // row 5
    { name: "ruby",           ambient: [0.1745,    0.01175,   0.01175,   1.0], diffuse: [0.61424,    0.04136,    0.04136,    1.0], specular: [0.727811, 0.626959, 0.626959, 1.0], shininess: 0.6        * 128.0 },
    { name: "gold",           ambient: [0.24725,   0.1995,    0.0745,    1.0], diffuse: [0.75164,    0.60648,    0.22648,    1.0], specular: [0.628281, 0.555802, 0.366065, 1.0], shininess: 0.4        * 128.0 },
    { name: "white plastic",  ambient: [0.0,       0.0,       0.0,       1.0], diffuse: [0.55,       0.55,       0.55,       1.0], specular: [0.7,      0.7,      0.7,      1.0], shininess: 0.25       * 128.0 },
    { name: "white rubber",   ambient: [0.05,      0.05,      0.05,      1.0], diffuse: [0.5,        0.5,        0.5,        1.0], specular: [0.7,      0.7,      0.7,      1.0], shininess: 0.078125   * 128.0 },
    // row 6
    { name: "turquoise",      ambient: [0.1,       0.18725,   0.1745,    1.0], diffuse: [0.396,      0.74151,    0.69102,    1.0], specular: [0.297254, 0.30829,  0.306678, 1.0], shininess: 0.1        * 128.0 },
    { name: "silver",         ambient: [0.19225,   0.19225,   0.19225,   1.0], diffuse: [0.50754,    0.50754,    0.50754,    1.0], specular: [0.508273, 0.508273, 0.508273, 1.0], shininess: 0.4        * 128.0 },
    { name: "yellow plastic", ambient: [0.0,       0.0,       0.0,       1.0], diffuse: [0.5,        0.5,        0.0,        1.0], specular: [0.6,      0.6,      0.5,      1.0], shininess: 0.25       * 128.0 },
    { name: "yellow rubber",  ambient: [0.05,      0.05,      0.0,       1.0], diffuse: [0.5,        0.5,        0.4,        1.0], specular: [0.7,      0.7,      0.04,     1.0], shininess: 0.078125   * 128.0 }
];

const SPHERE_X = [50,250,450,650]; // 4 Columns
const SPHERE_Y = [30,126,222,318,414,510]; // 6 rows

// material related macros
const MATERIAL_SIZE = 64; // This is size of one material sturcut containing 4 vec4s
const MATERIAL_STRIDE = 256; // minimum byte offset allignment hardware limit is 256. we are using it as padding after 64 bytes of above struct for each material.
const NUM_SPHERES = 24; 
let perspectiveProjectionMatrix = null;

let depthTexture = null;

let Sphere = null;
let numMeshIndices = 0;
let buffer_Positions = null;

let buffer_Normals = null;
let buffer_Texcoords = null;
let buffer_Elements = null;





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

    initialise();

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
    console.warning("WebGPUDeviceLostReason: ", info.reason , "WebGPUDeviceLostMessage: ", info.message);
    Device = null;
    Queue = null;
    renderPipeline = null;
    buffer_Uniform = null;
    bindingGroup_Uniform = null;
    buffer_Material = null;
    bindGroup_Material = null;
    perspectiveProjectionMatrix = null;
    depthTexture = null;
    Sphere = null;
    numMeshIndices = 0;
    buffer_Positions = null;

    buffer_Normals = null;
    buffer_Texcoords = null;
    buffer_Elements = null;
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

        case "X":
        case "x":
            accessChoosen = "X";
            break;

        case "Y":
        case "y":
            accessChoosen = "Y";
            break;

        case "Z":
        case "z":
            accessChoosen = "Z";
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


function initialise() 
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
    "    @location(0) transformedNormal: vec3<f32>,\n" +
    "    @location(1) lightDirection: vec3<f32>,\n" +
    "    @location(2) viewerVector: vec3<f32>,\n" +
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
    "    lKeyIsPressed: vec4<u32>,\n" +
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
    "fn main(@location(0) pos: vec3<f32>, @location(1) normal: vec3<f32>) -> VertexOutput\n" +
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
    "    @location(0) transformedNormal: vec3<f32>,\n" +
    "    @location(1) lightDirection: vec3<f32>,\n" +
    "    @location(2) viewerVector: vec3<f32>,\n" +
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
    "    lKeyIsPressed: vec4<u32>,\n" +
    "};\n" +

    "struct Material\n" +
    "{\n" +
    "    materialAmbient: vec4<f32>,\n" +
    "    materialDiffuse: vec4<f32>,\n" +
    "    materialSpecular: vec4<f32>,\n" +
    "    materialShininess: vec4<f32>,\n" +
    "};\n" +

    "@group(0) @binding(0)\n" +
    "var<uniform> uMyUniformData: MyUniformData;\n" +

    "@group(1) @binding(0)\n" +
    "var<uniform> material: Material;\n" +

    "@fragment\n" +
    "fn main(output: VertexOutput) -> @location(0) vec4<f32>\n" +
    "{\n" +
    "    var phong_ads_color: vec3<f32>;\n" +

    "    if(uMyUniformData.lKeyIsPressed.x == 1u)\n" +
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
    "            material.materialAmbient.xyz;\n" +

    // Diffuse
    "        let diffuse: vec3<f32> = " +
    "            uMyUniformData.lightDiffuse.xyz * " +
    "            material.materialDiffuse.xyz * " +
    "            max(dot(normalizedLightDirection, normalizedTransformedNormal), 0.0);\n" +

    // Reflection
    "        let reflectionVector: vec3<f32> = " +
    "            reflect(-normalizedLightDirection, normalizedTransformedNormal);\n" +

    // Specular
    "        let specular: vec3<f32> = " +
    "            uMyUniformData.lightSpecular.xyz * " +
    "            material.materialSpecular.xyz * " +
    "            pow(max(dot(reflectionVector, normalizedViewerVector), 0.0), " +
    "            material.materialShininess.x);\n" +

    "        phong_ads_color = ambient + diffuse + specular;\n" +
    "    }\n" +
    "    else\n" +
    "    {\n" +
    "        phong_ads_color = vec3<f32>(1.0, 1.0, 1.0);\n" +
    "    }\n" +

    "    return vec4<f32>(phong_ads_color, 1.0);\n" +
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

    // declare position array for Sphere
   // create sphere geometry
    Sphere = new Mesh();

    makeSphere(Sphere, 2.0, 30, 30);

    numMeshIndices = Sphere.getIndexCount();

    console.log(
        "initialise() : Sphere geometry = vertex count = ",
        Sphere.getVertexCount(),
        "Index count = ",
        numMeshIndices
    );

    // get sphere mesh data
    const meshData = Sphere.getMeshData();

    // create vertex buffer for positions
    buffer_Positions = createVertexBuffer(meshData.verticesArray);

    if(buffer_Positions == null)
    {
        console.log("initialise() : buffer_Positions creation failed.");
        throw Error("buffer_Positions creation failed.");
    }

    // create vertex buffer for normals
    buffer_Normals = createVertexBuffer(meshData.normalsArray);

    if(buffer_Normals == null)
    {
        console.log("initialise() : buffer_Normals creation failed.");
        throw Error("buffer_Normals creation failed.");
    }

    // create vertex buffer for texture coordinates
    buffer_Texcoords = createVertexBuffer(meshData.texCoordsArray);

    if(buffer_Texcoords == null)
    {
        console.log("initialise() : buffer_Texcoords creation failed.");
        throw Error("buffer_Texcoords creation failed.");
    }

    // create index buffer
    buffer_Elements = createIndexBuffer(meshData.indicesArray);

    if(buffer_Elements == null)
    {
        console.log("initialise() : buffer_Elements creation failed.");
        throw Error("buffer_Elements creation failed.");
    }


    // using UDF 1: createBindGroupLayoutUniform
    const bindGroupLayout_Uniform = createBindGroupLayoutUniform(0, GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT, "uniform");

    // create uniform buffer for mvp matrix using UDF 2: createUniformBuffer
    const myUniformBufferSize = Float32Array.BYTES_PER_ELEMENT * 16 +
                                Float32Array.BYTES_PER_ELEMENT * 16 + // from 64th to 127th
                                Float32Array.BYTES_PER_ELEMENT * 16 + // from 128th to 191st
                                Float32Array.BYTES_PER_ELEMENT * 4 + // from 192th to 207th
                                Float32Array.BYTES_PER_ELEMENT * 4 + // from 208th to 223th
                                Float32Array.BYTES_PER_ELEMENT * 4 + // from 224th to 239th
                                Float32Array.BYTES_PER_ELEMENT * 4 + // from 240th to 250th
                                Uint32Array.BYTES_PER_ELEMENT * 4; // from 320th to 335th


    const materialBufferSize = NUM_SPHERES * MATERIAL_STRIDE; // 24 * 256 = 6144 bytes

    buffer_Material = createUniformBuffer(materialBufferSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    // now we will write material table into buffer_Material but to do that we need to convert name : column table into  floatarray.

    const stride = MATERIAL_STRIDE / Float32Array.BYTES_PER_ELEMENT; // 256 / 4 = 64

    const materialArray = new Float32Array(NUM_SPHERES * stride); // 24 * 64 = 1536

    for(var i = 0; i < NUM_SPHERES; i++)
    {
        materialArray.set(materialTable[i].ambient, i * stride + 0);
        materialArray.set(materialTable[i].diffuse, i * stride + 4);
        materialArray.set(materialTable[i].specular, i * stride + 8);
        materialArray.set([materialTable[i].shininess, 0.0, 0.0, 0.0], i * stride + 12); // padding with 3 zeros
    }

    Queue.writeBuffer(buffer_Material, 0, materialArray, 0, materialArray.length);

    // now create bing group layour=t entry for matrial bind group
    const bindGroupLayoutEntry_Material =
    {
        binding: 0,
        visibility: GPUShaderStage.FRAGMENT,
        buffer:
        {
            type: "uniform",
            hasDynamicOffset: true,
            minBindingSize: MATERIAL_SIZE
        }
    }

    //NOW from above entry create bindgroup layout descriptor for material
    const bindGroupLayoutDescriptor_Material =
    {
        entries: [bindGroupLayoutEntry_Material]
    };

    const bindGroupLayout_Material = Device.createBindGroupLayout(bindGroupLayoutDescriptor_Material);

    if(bindGroupLayout_Material == null)
    {
        console.log("initialise() : bindGroupLayout_Material is not supported by your browser.\n");
        throw Error("initialise() : bindGroupLayout_Material is not supported by your browser.\n");
    }

    // create bind group entry for material 
    const bindGroupEntry_Material =
    {
        binding: 0,
        resource: 
        {
            buffer: buffer_Material,
            offset: 0,
            size: MATERIAL_SIZE //64
        },

    };

    // CRERATE BIND GROUP DESCRIPTOR FOR MATERIAL
    const bindGroupDescriptor_Material =
    {
        layout: bindGroupLayout_Material,
        entries: [bindGroupEntry_Material]
    };

    // CREATE ACTUAL BIND GROUP MATERIAL
    bindGroup_Material = Device.createBindGroup(bindGroupDescriptor_Material);

    if(bindGroup_Material == null)
    {
        console.log("initialise() : bindGroup_Material is not supported by your browser.\n");
        throw Error("initialise() : bindGroup_Material is not supported by your browser.\n");
    }

    // create pipeline layout
    const pipelineLayoutDescriptor =
    {
        bindGroupLayouts: [bindGroupLayout_Uniform, bindGroupLayout_Material]
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
    

    buffer_Uniform = createUniformBuffer(myUniformBufferSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    // using UDF 3: createBindGroupForUniform
    bindingGroup_Uniform = createBindGroupForUniform(buffer_Uniform, 0, myUniformBufferSize, 0, bindGroupLayout_Uniform);

    const positionVertexAttribute =
    {
        shaderLocation: 0, // this matches with @location(0) in vertex shader
        offset: 0,
        format: "float32x3"
    };

    const normalVertexAttribute =
    {
        shaderLocation: 1, // this matches with @location(1) in vertex shader
        offset: 0,
        format: "float32x3"
    };

    const normalVertexBufferLayout =
    {
        arrayStride: Float32Array.BYTES_PER_ELEMENT * 3, // 3 floats of 4 bytes each
        attributes: [normalVertexAttribute],
        stepMode: "vertex"
    };

    const positionVertexBufferLayout =
    {
        arrayStride: Float32Array.BYTES_PER_ELEMENT * 3, // 3 floats of 4 bytes each
        attributes: [positionVertexAttribute],
        stepMode: "vertex"
    };


    const vertexShaderState =
    {
        module: shaderModule_VertexShader,
        entryPoint: "main",
        buffers: [positionVertexBufferLayout,normalVertexBufferLayout]
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
    clearColor = { r: 0.25, g: 0.25, b: 0.25, a: 1.0 };

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
    const modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);

    mat4.multiply(modelViewProjectionMatrix, viewMatrix, modelMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewProjectionMatrix);

    // reset light position
    lightPosition[0] = 0.0;
    lightPosition[1] = 0.0;
    lightPosition[2] = 0.0;
    lightPosition[3] = 1.0;

    if(accessChoosen == "X")
    {
        lightPosition[0] = 0.0;
        lightPosition[1] = 45.0 * Math.sin(degToRad(lightAngle));
        lightPosition[2] = 45.0 * Math.cos(degToRad(lightAngle));
        lightPosition[3] = 1.0;

    }
    else if(accessChoosen == "Y")
    {
        lightPosition[0] = 45.0 * Math.cos(degToRad(lightAngle));
        lightPosition[1] = 0.0;
        lightPosition[2] = 45.0 * Math.sin(degToRad(lightAngle));
        lightPosition[3] = 1.0;
    }
    else if(accessChoosen == "Z")
    {
        lightPosition[0] = 45.0 * Math.sin(degToRad(lightAngle));
        lightPosition[1] = 45.0 * Math.cos(degToRad(lightAngle));
        lightPosition[2] = 0.0;
        lightPosition[3] = 1.0;
    }

    if(accessChoosen == "X" || accessChoosen == "Y" || accessChoosen == "Z")
    {
        lightAngle = lightAngle + 1.5;
        if(lightAngle >= 360.0)
        {
            lightAngle = lightAngle - 360.0;
        }
    }

    // toogle lighting
    if(isLightingEnabled == true)
    {
        lKeyIsPressed[0] = 1;
    }
    else
    {
        lKeyIsPressed[0] = 0;
    }

    // twleve write buffer calls to fill the one unified buffer targeting indiviual byte offest as we saw while intialising myuniform buffer size
    
    // one : matrices
    Queue.writeBuffer(buffer_Uniform, 0, modelMatrix, 0, modelMatrix.length);  // from 0th to 63rd
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16, viewMatrix, 0, viewMatrix.length); // from 64th to 127th
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 + Float32Array.BYTES_PER_ELEMENT * 16 , modelViewProjectionMatrix, 0, modelViewProjectionMatrix.length); // from 128th to 191st
    
    // two : lights
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 , lightAmbient, 0, lightAmbient.length); // from 192th to 207th
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 1 , lightDiffuse, 0, lightDiffuse.length);
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 2 , lightSpecular, 0, lightSpecular.length);
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 3 , lightPosition, 0, lightPosition.length);


    // four: light toggle
    Queue.writeBuffer(buffer_Uniform, Float32Array.BYTES_PER_ELEMENT * 16 * 3 + Float32Array.BYTES_PER_ELEMENT * 4 * 4 , lKeyIsPressed, 0, lKeyIsPressed.length);

    // start the renderpass
    const renderPassEncoder = commandEncoder.beginRenderPass(renderPassDescriptor);

    renderPassEncoder.setPipeline(renderPipeline);
    renderPassEncoder.setScissorRect(0, 0, canvas.width, canvas.height);
    renderPassEncoder.setVertexBuffer(0, buffer_Positions);
    renderPassEncoder.setVertexBuffer(1, buffer_Normals);
    renderPassEncoder.setIndexBuffer(buffer_Elements, "uint16");
    renderPassEncoder.setBindGroup(0, bindingGroup_Uniform);
    
    const viewPortWidth = canvas.width / 8.0;
    const viewPortHeight = canvas.height / 8.0;

    for(let row = 0; row < 6; row++)
    {
        for(let col = 0; col < 4; col++)
        {
            const X = SPHERE_X[col];
            const Y = SPHERE_Y[row];

            const viewPortX = X * canvas.width / 800;
            const viewPortY = Y * canvas.height / 600;

            const i  = (row * 4) + col;

            renderPassEncoder.setViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight, 0, 1);

            renderPassEncoder.setBindGroup(1, bindGroup_Material, [i * MATERIAL_STRIDE]);
            
            renderPassEncoder.drawIndexed(numMeshIndices); // 6 vertices for rectangle (2 triangles)
        }
    }

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

    // destroy the device
    if(Device != null)
    {
        Device.destroy();
        Device = null;
        Queue = null;
        renderPipeline = null;
        buffer_Uniform = null;
        bindingGroup_Uniform = null;
        buffer_Material = null;
        bindGroup_Material = null;
        buffer_Positions = null;
        buffer_Normals = null;
        buffer_Texcoords = null;
        buffer_Elements = null;

    }

    Sphere = null;

    perspectiveProjectionMatrix = null;

    console.log("uninitialise() : WebGPUDevice is destroyed successfully.\n");

}


