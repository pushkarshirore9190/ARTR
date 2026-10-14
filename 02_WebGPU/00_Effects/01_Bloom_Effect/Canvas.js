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

let perspectiveProjectionMatrix = null;

let depthTexture = null;

// ---------------------------------------------------------------------------------------------
// BLOOM RELATED GLOBALS
// ---------------------------------------------------------------------------------------------

// scene is rendered in HDR (16 bit float) so colors can be > 1.0 ; bloom picks only those bright pixels
const SCENE_FORMAT = "rgba16float";
const BLOOM_FORMAT = "rgba16float";

// number of blur levels (half, quarter, 1/8 ... of canvas size). more levels = wider glow
const BLOOM_MIP_COUNT = 5;

// ---- bloom controls (change these defaults or use keyboard at runtime) ----
var bloom_Threshold = 1.0;   // pixels brighter than this will glow          (Left / Right arrow)
var bloom_Knee      = 0.5;   // softness of the threshold edge (0 = hard)
var bloom_Intensity = 1.0;   // strength of glow added on top of the scene    (Up / Down arrow)
var bloom_Radius    = 1.0;   // spread of the glow (upsample filter radius)   ( [  and  ] )
var bloom_Exposure  = 1.0;   // overall brightness before tone mapping        ( -  and  + )
var viewMode        = 0;     // 0 = final, 1 = bloom only (debug), 2 = bloom OFF   ( V / B )

// scene (HDR) rendering
let renderPipeline_Scene = null;
let bindGroupLayout_Scene = null;
let buffer_Positions = null;
let buffer_Colors = null;
let sceneObjects = [];          // each : { buffer_Uniform, bindingGroup_Uniform, position, emission, angle, speed }

let sceneTexture = null;
let sceneTextureView = null;

// bloom chain textures
let bloomTextures = [];
let bloomTextureViews = [];
let bloomSizes = [];

// post process common
let sampler_Linear = null;
let bindGroupLayout_Post = null;        // uniform + sampler + 1 texture
let bindGroupLayout_Composite = null;   // uniform + sampler + 2 textures

let renderPipeline_Prefilter = null;
let renderPipeline_Downsample = null;
let renderPipeline_Upsample = null;
let renderPipeline_Composite = null;

let postUniformList = [];               // each : { buffer, texelX, texelY }  (one uniform buffer per pass)
let bindingGroup_Prefilter = null;
let bindingGroups_Downsample = [];
let bindingGroups_Upsample = [];
let bindingGroup_Composite = null;


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
    console.warn("WebGPUDeviceLostReason: ", info.reason , "WebGPUDeviceLostMessage: ", info.message);
    Device = null;
    Queue = null;
    buffer_Positions = null;
    buffer_Colors = null;
    renderPipeline_Scene = null;
    bindGroupLayout_Scene = null;
    sceneObjects = [];
    perspectiveProjectionMatrix = null;
    depthTexture = null;

    sceneTexture = null;
    sceneTextureView = null;
    bloomTextures = [];
    bloomTextureViews = [];
    bloomSizes = [];
    sampler_Linear = null;
    bindGroupLayout_Post = null;
    bindGroupLayout_Composite = null;
    renderPipeline_Prefilter = null;
    renderPipeline_Downsample = null;
    renderPipeline_Upsample = null;
    renderPipeline_Composite = null;
    postUniformList = [];
    bindingGroup_Prefilter = null;
    bindingGroups_Downsample = [];
    bindingGroups_Upsample = [];
    bindingGroup_Composite = null;
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

        // ---- bloom controls ----
        case "ArrowUp":
            event.preventDefault();
            bloom_Intensity = Math.min(bloom_Intensity + 0.1, 5.0);
            updateBloomInfo();
            break;

        case "ArrowDown":
            event.preventDefault();
            bloom_Intensity = Math.max(bloom_Intensity - 0.1, 0.0);
            updateBloomInfo();
            break;

        case "ArrowRight":
            event.preventDefault();
            bloom_Threshold = Math.min(bloom_Threshold + 0.1, 5.0);
            updateBloomInfo();
            break;

        case "ArrowLeft":
            event.preventDefault();
            bloom_Threshold = Math.max(bloom_Threshold - 0.1, 0.0);
            updateBloomInfo();
            break;

        case "]":
            bloom_Radius = Math.min(bloom_Radius + 0.1, 3.0);
            updateBloomInfo();
            break;

        case "[":
            bloom_Radius = Math.max(bloom_Radius - 0.1, 0.2);
            updateBloomInfo();
            break;

        case "+":
        case "=":
            bloom_Exposure = Math.min(bloom_Exposure + 0.1, 4.0);
            updateBloomInfo();
            break;

        case "-":
        case "_":
            bloom_Exposure = Math.max(bloom_Exposure - 0.1, 0.1);
            updateBloomInfo();
            break;

        case "B":
        case "b":
            viewMode = (viewMode == 2) ? 0 : 2;    // bloom ON / OFF
            updateBloomInfo();
            break;

        case "V":
        case "v":
            viewMode = (viewMode == 1) ? 0 : 1;    // bloom only (debug) / final
            updateBloomInfo();
            break;

        case "R":
        case "r":
            bloom_Threshold = 1.0;
            bloom_Knee = 0.5;
            bloom_Intensity = 1.0;
            bloom_Radius = 1.0;
            bloom_Exposure = 1.0;
            viewMode = 0;
            updateBloomInfo();
            break;

        default:
            break;
    }
}

// shows current bloom values in the browser tab title and in the console
function updateBloomInfo()
{
    var modeName = "FINAL";
    if(viewMode == 1)
    {
        modeName = "BLOOM ONLY";
    }
    else if(viewMode == 2)
    {
        modeName = "BLOOM OFF";
    }

    var info = "Bloom [" + modeName + "]  threshold=" + bloom_Threshold.toFixed(1) +
               "  intensity=" + bloom_Intensity.toFixed(1) +
               "  radius=" + bloom_Radius.toFixed(1) +
               "  exposure=" + bloom_Exposure.toFixed(1);

    document.title = info;
    console.log(info + "\n");
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

    // =========================================================================================
    // PART 1 : SCENE (rendered into HDR texture, NOT directly to canvas)
    // =========================================================================================

    // vertex shader code in WGSL
    // object uniform = MVP matrix + emission (brightness multiplier, > 1.0 means "glowing" object)
    const vertexShaderSourceCode =
    `struct ObjectUniform
    {
        MVPMatrix: mat4x4<f32>,
        emission: vec4<f32>
    };

    @group(0) @binding(0)
    var<uniform> objectUniform: ObjectUniform;

    struct VertexOutput
    {
        @builtin(position) position: vec4<f32>,
        @location(0) color: vec4<f32>
    };

    @vertex
    fn main(@location(0) pos: vec4<f32>, @location(1) color: vec4<f32>) -> VertexOutput
    {
        var vsOut: VertexOutput;
        vsOut.position = objectUniform.MVPMatrix * pos;
        vsOut.color = vec4<f32>(color.rgb * objectUniform.emission.x, 1.0);
        return vsOut;
    }`;

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


    // fragment shader code in WGSL - output is HDR, so values above 1.0 are kept as it is
    const fragmentShaderSourceCode =
    `@fragment
    fn main(@location(0) color: vec4<f32>) -> @location(0) vec4<f32>
    {
        return color;
    }`;

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

    // declare position array for cube (36 vertices, 6 faces x 2 triangles x 3 vertices)
    const vertex_Position = new Float32Array([
        // Front (+z)
        -1.0,  1.0,  1.0, 1.0,
        -1.0, -1.0,  1.0, 1.0,
         1.0, -1.0,  1.0, 1.0,
        -1.0,  1.0,  1.0, 1.0,
         1.0, -1.0,  1.0, 1.0,
         1.0,  1.0,  1.0, 1.0,

        // Back (-z)
         1.0,  1.0, -1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0,
         1.0,  1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0,
        -1.0,  1.0, -1.0, 1.0,

        // Right (+x)
         1.0,  1.0,  1.0, 1.0,
         1.0, -1.0,  1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
         1.0,  1.0,  1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
         1.0,  1.0, -1.0, 1.0,

        // Left (-x)
        -1.0,  1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0,
        -1.0, -1.0,  1.0, 1.0,
        -1.0,  1.0, -1.0, 1.0,
        -1.0, -1.0,  1.0, 1.0,
        -1.0,  1.0,  1.0, 1.0,

        // Top (+y)
        -1.0,  1.0, -1.0, 1.0,
        -1.0,  1.0,  1.0, 1.0,
         1.0,  1.0,  1.0, 1.0,
        -1.0,  1.0, -1.0, 1.0,
         1.0,  1.0,  1.0, 1.0,
         1.0,  1.0, -1.0, 1.0,

        // Bottom (-y)
        -1.0, -1.0,  1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
        -1.0, -1.0,  1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
         1.0, -1.0,  1.0, 1.0
    ]);

    // one saturated color per face (0..1) , the object's "emission" multiplies it into HDR range
    const faceColors = [
        [1.0, 0.15, 0.10],  // front   - red
        [0.20, 1.0, 0.20],  // back    - green
        [0.20, 0.35, 1.0],  // right   - blue
        [1.0, 0.85, 0.10],  // left    - yellow
        [0.10, 0.90, 1.0],  // top     - cyan
        [1.0, 0.20, 0.90]   // bottom  - magenta
    ];

    const vertex_Color = new Float32Array(36 * 4);
    for(let face = 0; face < 6; face++)
    {
        for(let v = 0; v < 6; v++)
        {
            const index = (face * 6 + v) * 4;
            vertex_Color[index + 0] = faceColors[face][0];
            vertex_Color[index + 1] = faceColors[face][1];
            vertex_Color[index + 2] = faceColors[face][2];
            vertex_Color[index + 3] = 1.0;
        }
    }

    // create vertex buffer for cube position
    const bufferDescriptor_Positions = 
    {
        size: vertex_Position.byteLength,
        usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST
    };

    buffer_Positions = Device.createBuffer(bufferDescriptor_Positions);

    if(buffer_Positions == null)
    {
        console.log("initialise() : buffer_Positions is not supported by your browser.\n");
        throw Error("initialise() : buffer_Positions is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Positions is supported by your browser.\n");
    }

    Queue.writeBuffer(buffer_Positions, 0, vertex_Position, 0, vertex_Position.length);

    // create vertex buffer for cube color
    const bufferDescriptor_Colors = 
    {
        size: vertex_Color.byteLength,
        usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST
    };

    buffer_Colors = Device.createBuffer(bufferDescriptor_Colors);

    if(buffer_Colors == null)
    {
        console.log("initialise() : buffer_Colors is not supported by your browser.\n");
        throw Error("initialise() : buffer_Colors is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Colors is supported by your browser.\n");
    }

    Queue.writeBuffer(buffer_Colors, 0, vertex_Color, 0, vertex_Color.length);

    console.log("initialise() : buffer_Positions and buffer_Colors are successfully created and data is copied to them.\n");

    // using UDF 1: createBindGroupLayoutUniform
    bindGroupLayout_Scene = createBindGroupLayoutUniform(0, GPUShaderStage.VERTEX, "uniform");

    // create pipeline layout for object uniform
    const pipelineLayoutDescriptor = 
    {
        bindGroupLayouts: [bindGroupLayout_Scene]
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

    // create the 3 scene objects : each has its own uniform buffer (MVP 16 floats + emission 4 floats)
    // emission > bloom_Threshold  => object GLOWS ,  emission < bloom_Threshold => object stays normal
    const objectUniformSize = (16 + 4) * 4;

    const objectDescriptions =
    [
        { position: [-2.6, 0.0, -9.0], emission: 4.0, speed: 0.8 },   // HOT bright cube   (strong glow)
        { position: [ 0.0, 0.0, -9.0], emission: 0.6, speed: 0.5 },   // DIM cube          (no glow)
        { position: [ 2.6, 0.0, -9.0], emission: 2.5, speed: 1.1 }    // bright cube       (medium glow)
    ];

    sceneObjects = [];
    for(let i = 0; i < objectDescriptions.length; i++)
    {
        // using UDF 2: createUniformBuffer
        const uniformBuffer = createUniformBuffer(objectUniformSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

        // using UDF 3: createBindGroupForUniform
        const bindGroup = createBindGroupForUniform(uniformBuffer, 0, objectUniformSize, 0, bindGroupLayout_Scene);

        sceneObjects.push(
        {
            buffer_Uniform: uniformBuffer,
            bindingGroup_Uniform: bindGroup,
            position: objectDescriptions[i].position,
            emission: objectDescriptions[i].emission,
            speed: objectDescriptions[i].speed,
            angle: 0.0
        });
    }

    // position attribute
    const positionVertexAttribute =
    {
        shaderLocation: 0, // this matches with @location(0) in vertex shader
        offset: 0,
        format: "float32x4"
    };

    const positionVetexBufferLayout =
    {
        arrayStride: 4 * 4, // 4 floats of 4 bytes each
        attributes: [positionVertexAttribute],
        stepMode: "vertex"
    };

    // color attribute
    const colorVertexAttribute =
    {
        shaderLocation: 1, // this matches with @location(1) in vertex shader
        offset: 0,
        format: "float32x4"
    };

    const colorVetexBufferLayout =
    {
        arrayStride: 4 * 4,
        attributes: [colorVertexAttribute],
        stepMode: "vertex"
    };

    const vertexShaderState =
    {
        module: shaderModule_VertexShader,
        entryPoint: "main",
        buffers: [positionVetexBufferLayout, colorVetexBufferLayout]
    };

    // scene renders into HDR texture so its format is SCENE_FORMAT (not canvasFormat)
    const colorTargetState =
    {
        format: SCENE_FORMAT,
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

    renderPipeline_Scene = Device.createRenderPipeline(pipelineDescriptor);

    if(renderPipeline_Scene == null)
    {
        console.log("initialise() : renderPipeline_Scene is not supported by your browser.\n");
        throw Error("initialise() : renderPipeline_Scene is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : renderPipeline_Scene is supported by your browser.\n");
    }

    // =========================================================================================
    // PART 2 : BLOOM POST PROCESS PIPELINES
    // =========================================================================================
    initialiseBloom();

    // initialise perspective projection matrix
    perspectiveProjectionMatrix = mat4.create();


    // define the clearcolor - BLACK background
    clearColor = { r: 0.0, g: 0.0, b: 0.0, a: 1.0 };

    updateBloomInfo();
}


// ---------------------------------------------------------------------------------------------
// BLOOM SHADERS (WGSL)
// ---------------------------------------------------------------------------------------------

// same uniform for every post pass (32 bytes)
const postCommonShaderCode =
`struct PostParams
{
    texelSize: vec2<f32>,   // 1 / (width, height) of the texture being READ by that pass
    threshold: f32,
    knee: f32,
    intensity: f32,
    radius: f32,
    exposure: f32,
    viewMode: f32
};

@group(0) @binding(0) var<uniform> params: PostParams;
@group(0) @binding(1) var linearSampler: sampler;
@group(0) @binding(2) var srcTexture: texture_2d<f32>;

fn sampleSrc(uv: vec2<f32>) -> vec3<f32>
{
    return textureSampleLevel(srcTexture, linearSampler, uv, 0.0).rgb;
}
`;

// full screen triangle generated from vertex index (no vertex buffer needed)
const postVertexShaderCode =
`struct VertexOutput
{
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>
};

@vertex
fn main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput
{
    var corners = array<vec2<f32>, 3>(
        vec2<f32>(-1.0, -1.0),
        vec2<f32>( 3.0, -1.0),
        vec2<f32>(-1.0,  3.0)
    );

    let p = corners[vertexIndex];

    var vsOut: VertexOutput;
    vsOut.position = vec4<f32>(p, 0.0, 1.0);
    vsOut.uv = vec2<f32>(p.x * 0.5 + 0.5, 0.5 - p.y * 0.5);
    return vsOut;
}`;

// STEP 1 : bright pass - keep only the pixels brighter than threshold (soft knee), write to half-res texture
const prefilterFragmentShaderCode = postCommonShaderCode +
`
fn brightPass(c: vec3<f32>) -> vec3<f32>
{
    let brightness = max(c.r, max(c.g, c.b));

    // soft knee curve : smooth transition around the threshold instead of a hard cut
    var soft = clamp(brightness - params.threshold + params.knee, 0.0, 2.0 * params.knee);
    soft = soft * soft / (4.0 * params.knee + 0.00001);

    let contribution = max(soft, brightness - params.threshold) / max(brightness, 0.00001);
    return c * contribution;
}

@fragment
fn main(@location(0) uv: vec2<f32>) -> @location(0) vec4<f32>
{
    let t = params.texelSize;

    // 4 taps (each one bilinear) = 4x4 source pixels averaged. threshold is applied per tap to reduce flicker
    var color = brightPass(sampleSrc(uv + vec2<f32>(-1.0, -1.0) * t));
    color = color + brightPass(sampleSrc(uv + vec2<f32>( 1.0, -1.0) * t));
    color = color + brightPass(sampleSrc(uv + vec2<f32>(-1.0,  1.0) * t));
    color = color + brightPass(sampleSrc(uv + vec2<f32>( 1.0,  1.0) * t));

    return vec4<f32>(color * 0.25, 1.0);
}`;

// STEP 2 : downsample - 13 tap filter (stable, no flickering) , each level is half size of previous
const downsampleFragmentShaderCode = postCommonShaderCode +
`
@fragment
fn main(@location(0) uv: vec2<f32>) -> @location(0) vec4<f32>
{
    let t = params.texelSize;

    let a = sampleSrc(uv + vec2<f32>(-2.0,  2.0) * t);
    let b = sampleSrc(uv + vec2<f32>( 0.0,  2.0) * t);
    let c = sampleSrc(uv + vec2<f32>( 2.0,  2.0) * t);

    let d = sampleSrc(uv + vec2<f32>(-2.0,  0.0) * t);
    let e = sampleSrc(uv);
    let f = sampleSrc(uv + vec2<f32>( 2.0,  0.0) * t);

    let g = sampleSrc(uv + vec2<f32>(-2.0, -2.0) * t);
    let h = sampleSrc(uv + vec2<f32>( 0.0, -2.0) * t);
    let i = sampleSrc(uv + vec2<f32>( 2.0, -2.0) * t);

    let j = sampleSrc(uv + vec2<f32>(-1.0,  1.0) * t);
    let k = sampleSrc(uv + vec2<f32>( 1.0,  1.0) * t);
    let l = sampleSrc(uv + vec2<f32>(-1.0, -1.0) * t);
    let m = sampleSrc(uv + vec2<f32>( 1.0, -1.0) * t);

    var color = e * 0.125;
    color = color + (a + c + g + i) * 0.03125;
    color = color + (b + d + f + h) * 0.0625;
    color = color + (j + k + l + m) * 0.125;

    return vec4<f32>(color, 1.0);
}`;

// STEP 3 : upsample - 3x3 tent filter, result is ADDED (blend one + one) on the bigger level
// params.radius controls how far the taps go => how wide the glow spreads
const upsampleFragmentShaderCode = postCommonShaderCode +
`
@fragment
fn main(@location(0) uv: vec2<f32>) -> @location(0) vec4<f32>
{
    let d = params.texelSize * params.radius;

    var color = sampleSrc(uv + vec2<f32>(-d.x,  d.y));
    color = color + sampleSrc(uv + vec2<f32>( 0.0,  d.y)) * 2.0;
    color = color + sampleSrc(uv + vec2<f32>( d.x,  d.y));

    color = color + sampleSrc(uv + vec2<f32>(-d.x, 0.0)) * 2.0;
    color = color + sampleSrc(uv) * 4.0;
    color = color + sampleSrc(uv + vec2<f32>( d.x, 0.0)) * 2.0;

    color = color + sampleSrc(uv + vec2<f32>(-d.x, -d.y));
    color = color + sampleSrc(uv + vec2<f32>( 0.0, -d.y)) * 2.0;
    color = color + sampleSrc(uv + vec2<f32>( d.x, -d.y));

    return vec4<f32>(color / 16.0, 1.0);
}`;

// STEP 4 : composite - scene + bloom * intensity , exposure, ACES tone mapping, gamma, write to canvas
const compositeFragmentShaderCode = postCommonShaderCode +
`
@group(0) @binding(3) var bloomTexture: texture_2d<f32>;

fn toneMapACES(x: vec3<f32>) -> vec3<f32>
{
    let a = 2.51;
    let b = 0.03;
    let c = 2.43;
    let d = 0.59;
    let e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), vec3<f32>(0.0), vec3<f32>(1.0));
}

@fragment
fn main(@location(0) uv: vec2<f32>) -> @location(0) vec4<f32>
{
    let sceneColor = sampleSrc(uv);
    let bloomColor = textureSampleLevel(bloomTexture, linearSampler, uv, 0.0).rgb;

    var color = sceneColor;
    if(params.viewMode < 0.5)
    {
        color = sceneColor + bloomColor * params.intensity;      // final
    }
    else if(params.viewMode < 1.5)
    {
        color = bloomColor * params.intensity;                   // bloom only (debug)
    }

    color = toneMapACES(color * params.exposure);
    color = pow(color, vec3<f32>(1.0 / 2.2));                    // linear -> display gamma

    return vec4<f32>(color, 1.0);
}`;


// creates samplers, bind group layouts and all post process pipelines (size independent things)
function initialiseBloom()
{
    // code
    const samplerDescriptor =
    {
        magFilter: "linear",
        minFilter: "linear",
        mipmapFilter: "nearest",
        addressModeU: "clamp-to-edge",
        addressModeV: "clamp-to-edge"
    };

    sampler_Linear = Device.createSampler(samplerDescriptor);
    if(sampler_Linear == null)
    {
        console.log("initialiseBloom() : sampler_Linear is not supported by your browser.\n");
        throw Error("initialiseBloom() : sampler_Linear is not supported by your browser.\n");
    }

    // bind group layout : uniform + sampler + 1 texture
    bindGroupLayout_Post = Device.createBindGroupLayout(
    {
        entries:
        [
            { binding: 0, visibility: GPUShaderStage.FRAGMENT, buffer: { type: "uniform" } },
            { binding: 1, visibility: GPUShaderStage.FRAGMENT, sampler: { type: "filtering" } },
            { binding: 2, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: "float", viewDimension: "2d" } }
        ]
    });

    if(bindGroupLayout_Post == null)
    {
        console.log("initialiseBloom() : bindGroupLayout_Post is not supported by your browser.\n");
        throw Error("initialiseBloom() : bindGroupLayout_Post is not supported by your browser.\n");
    }

    // bind group layout : uniform + sampler + scene texture + bloom texture
    bindGroupLayout_Composite = Device.createBindGroupLayout(
    {
        entries:
        [
            { binding: 0, visibility: GPUShaderStage.FRAGMENT, buffer: { type: "uniform" } },
            { binding: 1, visibility: GPUShaderStage.FRAGMENT, sampler: { type: "filtering" } },
            { binding: 2, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: "float", viewDimension: "2d" } },
            { binding: 3, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: "float", viewDimension: "2d" } }
        ]
    });

    if(bindGroupLayout_Composite == null)
    {
        console.log("initialiseBloom() : bindGroupLayout_Composite is not supported by your browser.\n");
        throw Error("initialiseBloom() : bindGroupLayout_Composite is not supported by your browser.\n");
    }

    // additive blending : used by upsample passes to accumulate every blur level
    const additiveBlendState =
    {
        color: { srcFactor: "one", dstFactor: "one", operation: "add" },
        alpha: { srcFactor: "one", dstFactor: "one", operation: "add" }
    };

    renderPipeline_Prefilter  = createPostPipeline(prefilterFragmentShaderCode,  bindGroupLayout_Post,      BLOOM_FORMAT, null);
    renderPipeline_Downsample = createPostPipeline(downsampleFragmentShaderCode, bindGroupLayout_Post,      BLOOM_FORMAT, null);
    renderPipeline_Upsample   = createPostPipeline(upsampleFragmentShaderCode,   bindGroupLayout_Post,      BLOOM_FORMAT, additiveBlendState);
    renderPipeline_Composite  = createPostPipeline(compositeFragmentShaderCode,  bindGroupLayout_Composite, canvasFormat, null);

    console.log("initialiseBloom() : bloom pipelines are successfully created.\n");
}


// USER DEFINED FUNCTIONS

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

// creates one full screen post process pipeline (UDF 4)
function createPostPipeline(_fragmentShaderCode, _bindGroupLayout, _colorFormat, _blendState)
{
    // code
    const shaderModule_PostVertex = Device.createShaderModule({ code: postVertexShaderCode });
    const shaderModule_PostFragment = Device.createShaderModule({ code: _fragmentShaderCode });

    if(shaderModule_PostVertex == null || shaderModule_PostFragment == null)
    {
        console.log("createPostPipeline() : shader module is not supported by your browser.\n");
        throw Error("createPostPipeline() : shader module is not supported by your browser.\n");
    }

    const pipelineLayout = Device.createPipelineLayout({ bindGroupLayouts: [_bindGroupLayout] });
    if(pipelineLayout == null)
    {
        console.log("createPostPipeline() : pipelineLayout is not supported by your browser.\n");
        throw Error("createPostPipeline() : pipelineLayout is not supported by your browser.\n");
    }

    const colorTargetState =
    {
        format: _colorFormat
    };

    if(_blendState != null)
    {
        colorTargetState.blend = _blendState;
    }

    const pipelineDescriptor =
    {
        layout: pipelineLayout,
        vertex:
        {
            module: shaderModule_PostVertex,
            entryPoint: "main"
        },
        fragment:
        {
            module: shaderModule_PostFragment,
            entryPoint: "main",
            targets: [colorTargetState]
        },
        primitive:
        {
            topology: "triangle-list",
            cullMode: "none"
        }
    };

    const pipeline = Device.createRenderPipeline(pipelineDescriptor);
    if(pipeline == null)
    {
        console.log("createPostPipeline() : pipeline is not supported by your browser.\n");
        throw Error("createPostPipeline() : pipeline is not supported by your browser.\n");
    }

    return pipeline;
}

// creates a texture which can be rendered into AND sampled later (UDF 5)
function createRenderTexture(_width, _height, _format)
{
    // code
    const textureDescriptor =
    {
        size: [_width, _height, 1],
        dimension: "2d",
        format: _format,
        usage: GPUTextureUsage.RENDER_ATTACHMENT | GPUTextureUsage.TEXTURE_BINDING
    };

    const texture = Device.createTexture(textureDescriptor);
    if(texture == null)
    {
        console.log("createRenderTexture() : texture is not supported by your browser.\n");
        throw Error("createRenderTexture() : texture is not supported by your browser.\n");
    }

    return texture;
}

// one uniform buffer per post pass (UDF 6). WebGPU queue.writeBuffer is not ordered with passes inside one command encoder, so every pass needs its own buffer
function createPostUniform(_texelX, _texelY)
{
    // code
    const uniformBuffer = createUniformBuffer(8 * 4, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    postUniformList.push(
    {
        buffer: uniformBuffer,
        texelX: _texelX,
        texelY: _texelY
    });

    return uniformBuffer;
}

// bind group for post pass : uniform + sampler + texture(s) (UDF 7)
function createBindGroupPost(_uniformBuffer, _textureView, _bloomTextureView)
{
    // code
    const entries =
    [
        { binding: 0, resource: { buffer: _uniformBuffer, offset: 0, size: 8 * 4 } },
        { binding: 1, resource: sampler_Linear },
        { binding: 2, resource: _textureView }
    ];

    var layout = bindGroupLayout_Post;

    if(_bloomTextureView != null)
    {
        entries.push({ binding: 3, resource: _bloomTextureView });
        layout = bindGroupLayout_Composite;
    }

    const bindGroup = Device.createBindGroup({ layout: layout, entries: entries });
    if(bindGroup == null)
    {
        console.log("createBindGroupPost() : bindGroup is not supported by your browser.\n");
        throw Error("createBindGroupPost() : bindGroup is not supported by your browser.\n");
    }

    return bindGroup;
}

// destroys everything which depends on canvas size
function destroyBloomResources()
{
    // code
    if(sceneTexture != null)
    {
        sceneTexture.destroy();
        sceneTexture = null;
        sceneTextureView = null;
    }

    for(let i = 0; i < bloomTextures.length; i++)
    {
        bloomTextures[i].destroy();
    }
    bloomTextures = [];
    bloomTextureViews = [];
    bloomSizes = [];

    for(let i = 0; i < postUniformList.length; i++)
    {
        postUniformList[i].buffer.destroy();
    }
    postUniformList = [];

    bindingGroup_Prefilter = null;
    bindingGroups_Downsample = [];
    bindingGroups_Upsample = [];
    bindingGroup_Composite = null;
}

// creates scene texture, bloom chain textures, per pass uniform buffers and bind groups. called from resize()
function createBloomResources()
{
    // code
    destroyBloomResources();

    const width = canvas.width;
    const height = canvas.height;

    // HDR scene texture (same size as canvas)
    sceneTexture = createRenderTexture(width, height, SCENE_FORMAT);
    sceneTextureView = sceneTexture.createView();

    // bloom chain : level 0 = 1/2 size , level 1 = 1/4 size , level 2 = 1/8 size ...
    for(let i = 0; i < BLOOM_MIP_COUNT; i++)
    {
        const levelWidth = Math.max(1, width >> (i + 1));
        const levelHeight = Math.max(1, height >> (i + 1));

        bloomSizes.push([levelWidth, levelHeight]);

        const texture = createRenderTexture(levelWidth, levelHeight, BLOOM_FORMAT);
        bloomTextures.push(texture);
        bloomTextureViews.push(texture.createView());
    }

    // prefilter : reads scene texture
    const uniformBuffer_Prefilter = createPostUniform(1.0 / width, 1.0 / height);
    bindingGroup_Prefilter = createBindGroupPost(uniformBuffer_Prefilter, sceneTextureView, null);

    // downsample i : reads level (i-1) , writes level i
    bindingGroups_Downsample = [];
    for(let i = 1; i < BLOOM_MIP_COUNT; i++)
    {
        const uniformBuffer = createPostUniform(1.0 / bloomSizes[i - 1][0], 1.0 / bloomSizes[i - 1][1]);
        bindingGroups_Downsample[i] = createBindGroupPost(uniformBuffer, bloomTextureViews[i - 1], null);
    }

    // upsample i : reads level i , adds into level (i-1)
    bindingGroups_Upsample = [];
    for(let i = BLOOM_MIP_COUNT - 1; i >= 1; i--)
    {
        const uniformBuffer = createPostUniform(1.0 / bloomSizes[i][0], 1.0 / bloomSizes[i][1]);
        bindingGroups_Upsample[i] = createBindGroupPost(uniformBuffer, bloomTextureViews[i], null);
    }

    // composite : reads scene texture + bloom level 0
    const uniformBuffer_Composite = createPostUniform(1.0 / width, 1.0 / height);
    bindingGroup_Composite = createBindGroupPost(uniformBuffer_Composite, sceneTextureView, bloomTextureViews[0]);
}

// writes current bloom controls to every post pass uniform buffer
function updatePostUniforms()
{
    // code
    const data = new Float32Array(8);

    for(let i = 0; i < postUniformList.length; i++)
    {
        data[0] = postUniformList[i].texelX;
        data[1] = postUniformList[i].texelY;
        data[2] = bloom_Threshold;
        data[3] = bloom_Knee;
        data[4] = bloom_Intensity;
        data[5] = bloom_Radius;
        data[6] = bloom_Exposure;
        data[7] = viewMode;

        Queue.writeBuffer(postUniformList[i].buffer, 0, data, 0, data.length);
    }
}

// runs one full screen pass (UDF 8)
function runPostPass(_commandEncoder, _pipeline, _bindGroup, _targetView, _loadOp)
{
    // code
    const colorAttachment =
    {
        view: _targetView,
        clearValue: clearColor,
        loadOp: _loadOp,
        storeOp: "store"
    };

    const passEncoder = _commandEncoder.beginRenderPass({ colorAttachments: [colorAttachment] });

    passEncoder.setPipeline(_pipeline);
    passEncoder.setBindGroup(0, _bindGroup);
    passEncoder.draw(3);    // one full screen triangle
    passEncoder.end();
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

        // HDR scene texture + bloom chain depend on canvas size so recreate them here
        createBloomResources();
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

    // ---------------------------------------------------------------------------------------
    // PASS 1 : render scene into HDR texture (NOT into canvas)
    // ---------------------------------------------------------------------------------------

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

    // create renderpass color attachment of type GPURenderPassColorAttachment - target is HDR sceneTexture
    const renderPassColorAttachment = 
    {
        view: sceneTextureView,
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

    // update uniform of each object (every object has its own buffer)
    const objectUniformData = new Float32Array(20);
    for(let i = 0; i < sceneObjects.length; i++)
    {
        const modelViewMatrix = mat4.create();
        const modelViewProjectionMatrix = mat4.create();

        mat4.translate(modelViewMatrix, modelViewMatrix, sceneObjects[i].position);
        mat4.rotateY(modelViewMatrix, modelViewMatrix, sceneObjects[i].angle * Math.PI / 180.0);
        mat4.rotateX(modelViewMatrix, modelViewMatrix, sceneObjects[i].angle * 0.6 * Math.PI / 180.0);
        mat4.scale(modelViewMatrix, modelViewMatrix, [0.8, 0.8, 0.8]);
        mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

        objectUniformData.set(modelViewProjectionMatrix, 0);
        objectUniformData[16] = sceneObjects[i].emission;
        objectUniformData[17] = 0.0;
        objectUniformData[18] = 0.0;
        objectUniformData[19] = 0.0;

        Queue.writeBuffer(sceneObjects[i].buffer_Uniform, 0, objectUniformData, 0, objectUniformData.length);
    }

    // update bloom controls in every post pass uniform
    updatePostUniforms();

    // start the renderpass
    const renderPassEncoder = commandEncoder.beginRenderPass(renderPassDescriptor);

    renderPassEncoder.setPipeline(renderPipeline_Scene);
    renderPassEncoder.setViewport(0, 0, canvas.width, canvas.height, 0, 1);
    renderPassEncoder.setScissorRect(0, 0, canvas.width, canvas.height);
    renderPassEncoder.setVertexBuffer(0, buffer_Positions);
    renderPassEncoder.setVertexBuffer(1, buffer_Colors);

    for(let i = 0; i < sceneObjects.length; i++)
    {
        renderPassEncoder.setBindGroup(0, sceneObjects[i].bindingGroup_Uniform);
        renderPassEncoder.draw(36);  // 36 vertices for cube
    }

    // end the renderpass
    renderPassEncoder.end();

    // ---------------------------------------------------------------------------------------
    // PASS 2 : bright pass  (scene -> bloom level 0)
    // ---------------------------------------------------------------------------------------
    runPostPass(commandEncoder, renderPipeline_Prefilter, bindingGroup_Prefilter, bloomTextureViews[0], "clear");

    // ---------------------------------------------------------------------------------------
    // PASS 3 : downsample chain  (level i-1 -> level i)
    // ---------------------------------------------------------------------------------------
    for(let i = 1; i < BLOOM_MIP_COUNT; i++)
    {
        runPostPass(commandEncoder, renderPipeline_Downsample, bindingGroups_Downsample[i], bloomTextureViews[i], "clear");
    }

    // ---------------------------------------------------------------------------------------
    // PASS 4 : upsample chain  (level i is ADDED on top of level i-1)  - loadOp "load" keeps existing content
    // ---------------------------------------------------------------------------------------
    for(let i = BLOOM_MIP_COUNT - 1; i >= 1; i--)
    {
        runPostPass(commandEncoder, renderPipeline_Upsample, bindingGroups_Upsample[i], bloomTextureViews[i - 1], "load");
    }

    // ---------------------------------------------------------------------------------------
    // PASS 5 : composite scene + bloom into the canvas
    // ---------------------------------------------------------------------------------------
    runPostPass(commandEncoder, renderPipeline_Composite, bindingGroup_Composite, Context.getCurrentTexture().createView(), "clear");

    // finish the command encoder. there can be multiple command encoders we have one and submit it to the queue
    Queue.submit([commandEncoder.finish()]);

    // animation loop
    animationFrameId = requestAnimationFrame(display);

    update();

}

function update() 
{
    // code
    for(let i = 0; i < sceneObjects.length; i++)
    {
        sceneObjects[i].angle = sceneObjects[i].angle + sceneObjects[i].speed;
        if(sceneObjects[i].angle >= 360.0)
        {
            sceneObjects[i].angle = sceneObjects[i].angle - 360.0;
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

    // destroy bloom textures and uniform buffers
    destroyBloomResources();

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
        buffer_Positions = null;
        buffer_Colors = null;
        renderPipeline_Scene = null;
        bindGroupLayout_Scene = null;
        sceneObjects = [];

        sampler_Linear = null;
        bindGroupLayout_Post = null;
        bindGroupLayout_Composite = null;
        renderPipeline_Prefilter = null;
        renderPipeline_Downsample = null;
        renderPipeline_Upsample = null;
        renderPipeline_Composite = null;
    }

    perspectiveProjectionMatrix = null;

    console.log("uninitialise() : WebGPUDevice is destroyed successfully.\n");

}





