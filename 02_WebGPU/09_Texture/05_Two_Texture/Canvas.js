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

let buffer_Positions_Pyramid = null;
let buffer_Texcoords_Pyramid = null;
let buffer_Positions_Cube = null;
let buffer_Texcoords_Cube = null;
let renderPipeline = null;
let buffer_MVPUniform_Pyramid = null;
let buffer_MVPUniform_Cube = null;
let bindingGroup_MVPUniform_Pyramid = null;
let bindingGroup_MVPUniform_Cube = null;
let perspectiveProjectionMatrix = null;

let depthTexture = null;
let texture_Stone = null;
let sampler_Stone = null;
let bindGroup_Texture_And_Sampler_Pyramid = null;
let texture_Vijay_Kundali = null;
let sampler_Vijay_Kundali = null;
let bindGroup_Texture_And_Sampler_Cube = null;

var angle_Pyramid = 0.0;
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
    buffer_Positions_Pyramid = null;
    buffer_Texcoords_Pyramid = null;
    buffer_Positions_Cube = null;
    buffer_Texcoords_Cube = null;
    renderPipeline = null;
    buffer_MVPUniform_Pyramid = null;
    buffer_MVPUniform_Cube = null;
    bindingGroup_MVPUniform_Pyramid = null;
    bindingGroup_MVPUniform_Cube = null;
    perspectiveProjectionMatrix = null;
    depthTexture = null;
    texture_Stone = null;
    sampler_Stone = null;
    bindGroup_Texture_And_Sampler_Pyramid = null;
    texture_Vijay_Kundali = null;
    sampler_Vijay_Kundali = null;
    bindGroup_Texture_And_Sampler_Cube = null;
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

    // vertex shader code in WGSL
   const vertexShaderSourceCode =
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
    "@group(1) @binding(0) var myTexture2D: texture_2d<f32>;\n" +
    "@group(1) @binding(1) var mySampler: sampler;\n" +
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
    "    var color = textureSample(myTexture2D, mySampler, output.Texcoords);\n" +
    "    return color;\n" +
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

    // declare position array for pyramid (4 side faces + 2 bottom triangles = 18 vertices)
    const vertex_Position_Pyramid = new Float32Array([
        // Front face
         0.0,  1.0,  0.0, 1.0,   // apex
        -1.0, -1.0,  1.0, 1.0,   // left bottom
         1.0, -1.0,  1.0, 1.0,   // right bottom

        // Right face
         0.0,  1.0,  0.0, 1.0,
         1.0, -1.0,  1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,

        // Back face
         0.0,  1.0,  0.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0,

        // Left face
         0.0,  1.0,  0.0, 1.0,
        -1.0, -1.0, -1.0, 1.0,
        -1.0, -1.0,  1.0, 1.0,

        // Bottom triangle 1
        -1.0, -1.0,  1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,

        // Bottom triangle 2
        -1.0, -1.0,  1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
         1.0, -1.0,  1.0, 1.0
    ]);

    // declare texcoords array for pyramid
    // (loadTexture() uses flipY: true, so v = 1.0 samples the top of the image : apex gets v = 1.0)
    const vertex_Texcoords_Pyramid = new Float32Array([
        // Front face
        0.5, 1.0,   // apex
        0.0, 0.0,   // left bottom
        1.0, 0.0,   // right bottom

        // Right face
        0.5, 1.0,
        0.0, 0.0,
        1.0, 0.0,

        // Back face
        0.5, 1.0,
        0.0, 0.0,
        1.0, 0.0,

        // Left face
        0.5, 1.0,
        0.0, 0.0,
        1.0, 0.0,

        // Bottom triangle 1
        0.0, 0.0,   // (-1, -1,  1)
        0.0, 1.0,   // (-1, -1, -1)
        1.0, 1.0,   // ( 1, -1, -1)

        // Bottom triangle 2
        0.0, 0.0,   // (-1, -1,  1)
        1.0, 1.0,   // ( 1, -1, -1)
        1.0, 0.0    // ( 1, -1,  1)
    ]);

    // declare position array for cube (6 faces x 2 triangles x 3 vertices = 36 vertices)
    const vertex_Position_Cube = new Float32Array([
        // Front face
        -1.0,  1.0,  1.0, 1.0,
         1.0,  1.0,  1.0, 1.0,
         1.0, -1.0,  1.0, 1.0,
        -1.0,  1.0,  1.0, 1.0,
         1.0, -1.0,  1.0, 1.0,
        -1.0, -1.0,  1.0, 1.0,

        // Right face
         1.0,  1.0,  1.0, 1.0,
         1.0,  1.0, -1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
         1.0,  1.0,  1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
         1.0, -1.0,  1.0, 1.0,

        // Back face
         1.0,  1.0, -1.0, 1.0,
        -1.0,  1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0,
         1.0,  1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,

        // Left face
        -1.0,  1.0, -1.0, 1.0,
        -1.0,  1.0,  1.0, 1.0,
        -1.0, -1.0,  1.0, 1.0,
        -1.0,  1.0, -1.0, 1.0,
        -1.0, -1.0,  1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0,

        // Top face
        -1.0,  1.0, -1.0, 1.0,
         1.0,  1.0, -1.0, 1.0,
         1.0,  1.0,  1.0, 1.0,
        -1.0,  1.0, -1.0, 1.0,
         1.0,  1.0,  1.0, 1.0,
        -1.0,  1.0,  1.0, 1.0,

        // Bottom face
        -1.0, -1.0,  1.0, 1.0,
         1.0, -1.0,  1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
        -1.0, -1.0,  1.0, 1.0,
         1.0, -1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0
    ]);

    // declare texcoords array for cube : every face shows the whole image
    // (loadTexture() uses flipY: true, so v = 1.0 samples the top of the image)
    const vertex_Texcoords_Cube = new Float32Array([
        // Front face
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0,

        // Right face
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0,

        // Back face
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0,

        // Left face
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0,

        // Top face
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0,

        // Bottom face
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0
    ]);

    // create vertex buffers for pyramid (position + texcoords)
    buffer_Positions_Pyramid = createVertexBuffer(vertex_Position_Pyramid, "buffer_Positions_Pyramid");
    buffer_Texcoords_Pyramid = createVertexBuffer(vertex_Texcoords_Pyramid, "buffer_Texcoords_Pyramid");

    // create vertex buffers for cube (position + texcoords)
    buffer_Positions_Cube = createVertexBuffer(vertex_Position_Cube, "buffer_Positions_Cube");
    buffer_Texcoords_Cube = createVertexBuffer(vertex_Texcoords_Cube, "buffer_Texcoords_Cube");

    // using UDF 1: createBindGroupLayoutUniform
    const bindGroupLayout_mvpUniform = createBindGroupLayoutUniform(0, GPUShaderStage.VERTEX, "uniform");

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
            bindGroupLayout_mvpUniform,
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
    const mvpUniformSize = 16 * 4; // 4x4 matrix of float32

    // pyramid : its own mvp uniform buffer and bind group
    buffer_MVPUniform_Pyramid = createUniformBuffer(mvpUniformSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    // using UDF 3: createBindGroupForUniform
    bindingGroup_MVPUniform_Pyramid = createBindGroupForUniform(buffer_MVPUniform_Pyramid, 0, mvpUniformSize, 0, bindGroupLayout_mvpUniform);

    // cube : its own mvp uniform buffer and bind group
    buffer_MVPUniform_Cube = createUniformBuffer(mvpUniformSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    bindingGroup_MVPUniform_Cube = createBindGroupForUniform(buffer_MVPUniform_Cube, 0, mvpUniformSize, 0, bindGroupLayout_mvpUniform);

    // texture sampler descriptor (same settings for both samplers)
    const samplerDescriptor = 
    {
        magFilter: "linear",
        minFilter: "linear",
    }

    // ---------------- pyramid : Stone.png

    // load the image to create the texture
    texture_Stone = await loadTexture("Stone.png");

    if(texture_Stone == null)
    {
        console.log("initialise() : texture_Stone is not supported by your browser.\n");
        throw Error("initialise() : texture_Stone is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : texture_Stone is supported by your browser.\n");
    }

    // create texture sampler
    sampler_Stone = Device.createSampler(samplerDescriptor);

    if(sampler_Stone == null)
    {
        console.log("initialise() : sampler_Stone is not supported by your browser.\n");
        throw Error("initialise() : sampler_Stone is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : sampler_Stone is supported by your browser.\n"); 
    }

    // create texure and sampler bind group for pyramid
    bindGroup_Texture_And_Sampler_Pyramid = createBindGroupForTextureAndSampler(
        0,
        texture_Stone,
        1,
        sampler_Stone,
        bindGroupLayout_Texture_And_Sampler
    );

    // ---------------- cube : Vijay_Kundali.png

    // load the image to create the texture
    texture_Vijay_Kundali = await loadTexture("Vijay_Kundali.png");

    if(texture_Vijay_Kundali == null)
    {
        console.log("initialise() : texture_Vijay_Kundali is not supported by your browser.\n");
        throw Error("initialise() : texture_Vijay_Kundali is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : texture_Vijay_Kundali is supported by your browser.\n");
    }

    // create texture sampler
    sampler_Vijay_Kundali = Device.createSampler(samplerDescriptor);

    if(sampler_Vijay_Kundali == null)
    {
        console.log("initialise() : sampler_Vijay_Kundali is not supported by your browser.\n");
        throw Error("initialise() : sampler_Vijay_Kundali is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : sampler_Vijay_Kundali is supported by your browser.\n"); 
    }

    // create texure and sampler bind group for cube
    bindGroup_Texture_And_Sampler_Cube = createBindGroupForTextureAndSampler(
        0,
        texture_Vijay_Kundali,
        1,
        sampler_Vijay_Kundali,
        bindGroupLayout_Texture_And_Sampler
    );

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

    // for texcoords
    const texcoordsVertexAttribute =
    {
        shaderLocation: 1, // this matches with @location(1) in vertex shader
        offset: 0,
        format: "float32x2"
    };

    const texcoordsVetexBufferLayout =
    {
        arrayStride: 4 * 2, // 4 floats of 4 bytes each
        attributes: [texcoordsVertexAttribute],
        stepMode: "vertex"
    };

    const vertexShaderState =
    {
        module: shaderModule_VertexShader,
        entryPoint: "main",
        buffers: [positionVetexBufferLayout, texcoordsVetexBufferLayout]
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

function createVertexBuffer(_data, _name)
{
    // code
    const bufferDescriptor =
    {
        size: _data.byteLength,
        usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST
    };

    const vertexBuffer = Device.createBuffer(bufferDescriptor);
    if(vertexBuffer == null)
    {
        console.log("createVertexBuffer() : " + _name + " is not supported by your browser.\n");
        throw Error("createVertexBuffer() : " + _name + " is not supported by your browser.\n");
    }

    Queue.writeBuffer(vertexBuffer, 0, _data, 0, _data.length);

    console.log("createVertexBuffer() : " + _name + " is successfully created and data is copied to it.\n");

    return vertexBuffer;
}

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

// 3 UDF for texture and sampler

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

    // start the renderpass
    const renderPassEncoder = commandEncoder.beginRenderPass(renderPassDescriptor);

    renderPassEncoder.setPipeline(renderPipeline);
    renderPassEncoder.setViewport(0, 0, canvas.width, canvas.height, 0, 1);
    renderPassEncoder.setScissorRect(0, 0, canvas.width, canvas.height);

    // Pyramid (Stone.png)

    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();
    mat4.translate(modelViewMatrix, modelViewMatrix, [-2.0, 0.0, -6.0]);
    mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angle_Pyramid));
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    Queue.writeBuffer(buffer_MVPUniform_Pyramid, 0, modelViewProjectionMatrix, 0, modelViewProjectionMatrix.length);

    renderPassEncoder.setVertexBuffer(0, buffer_Positions_Pyramid);
    renderPassEncoder.setVertexBuffer(1, buffer_Texcoords_Pyramid);
    renderPassEncoder.setBindGroup(0, bindingGroup_MVPUniform_Pyramid);
    renderPassEncoder.setBindGroup(1, bindGroup_Texture_And_Sampler_Pyramid);
    renderPassEncoder.draw(18);  // 18 vertices for pyramid (4 side triangles + 2 bottom triangles)

    // Cube (Vijay_Kundali.png)

    modelViewMatrix = mat4.create();
    modelViewProjectionMatrix = mat4.create();
    mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, -7.0]);
    mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(angle_Cube));
    mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angle_Cube));
    mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(angle_Cube));
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    Queue.writeBuffer(buffer_MVPUniform_Cube, 0, modelViewProjectionMatrix, 0, modelViewProjectionMatrix.length);

    renderPassEncoder.setVertexBuffer(0, buffer_Positions_Cube);
    renderPassEncoder.setVertexBuffer(1, buffer_Texcoords_Cube);
    renderPassEncoder.setBindGroup(0, bindingGroup_MVPUniform_Cube);
    renderPassEncoder.setBindGroup(1, bindGroup_Texture_And_Sampler_Cube);
    renderPassEncoder.draw(36);  // 36 vertices for cube (6 faces x 2 triangles)

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
    angle_Pyramid = angle_Pyramid + 1.0;
    if(angle_Pyramid >= 360.0)
    {
        angle_Pyramid = angle_Pyramid - 360.0;
    }

    angle_Cube = angle_Cube + 1.0;
    if(angle_Cube >= 360.0)
    {
        angle_Cube = angle_Cube - 360.0;
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
    if(texture_Stone != null)
    {
        texture_Stone.destroy();
        texture_Stone = null;
    }

    if(texture_Vijay_Kundali != null)
    {
        texture_Vijay_Kundali.destroy();
        texture_Vijay_Kundali = null;
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
        buffer_Positions_Pyramid = null;
        buffer_Texcoords_Pyramid = null;
        buffer_Positions_Cube = null;
        buffer_Texcoords_Cube = null;
        renderPipeline = null;
        buffer_MVPUniform_Pyramid = null;
        buffer_MVPUniform_Cube = null;
        bindingGroup_MVPUniform_Pyramid = null;
        bindingGroup_MVPUniform_Cube = null;
        sampler_Stone = null;
        sampler_Vijay_Kundali = null;
        bindGroup_Texture_And_Sampler_Pyramid = null;
        bindGroup_Texture_And_Sampler_Cube = null;
    }

    perspectiveProjectionMatrix = null;

    console.log("uninitialise() : WebGPUDevice is destroyed successfully.\n");

}



