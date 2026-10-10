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

let buffer_Positions = null;
let buffer_Normals = null;
let renderPipeline = null;
let buffer_Uniform = null;
let bindingGroup_Uniform = null;
let perspectiveProjectionMatrix = null;

let depthTexture = null;

var angle_Pyramid = 0.0;

// light toggle : false -> white pyramid, true -> diffuse lit pyramid
var bLight = false;



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
    buffer_Normals = null;
    renderPipeline = null;
    buffer_Uniform = null;
    bindingGroup_Uniform = null;
    perspectiveProjectionMatrix = null;
    depthTexture = null;
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
            if(bLight == false)
            {
                bLight = true;
            }
            else
            {
                bLight = false;
            }
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
    // (per fragment lighting : the vertex shader only sends normal, 2 light directions and viewer vector, same as Shader.vert of the Vulkan version)
    const vertexShaderSourceCode =
    "struct UniformData\n" +
    "{\n" +
    "    modelMatrix: mat4x4<f32>,\n" +
    "    viewMatrix: mat4x4<f32>,\n" +
    "    projectionMatrix: mat4x4<f32>,\n" +
    "    normalMatrix: mat4x4<f32>,\n" +
    "\n" +
    "    // Light 1\n" +
    "    lightAmbient1: vec4<f32>,\n" +
    "    lightDiffuse1: vec4<f32>,\n" +
    "    lightSpecular1: vec4<f32>,\n" +
    "    lightPosition1: vec4<f32>,\n" +
    "\n" +
    "    // Light 2\n" +
    "    lightAmbient2: vec4<f32>,\n" +
    "    lightDiffuse2: vec4<f32>,\n" +
    "    lightSpecular2: vec4<f32>,\n" +
    "    lightPosition2: vec4<f32>,\n" +
    "\n" +
    "    // Material\n" +
    "    materialAmbient: vec4<f32>,\n" +
    "    materialDiffuse: vec4<f32>,\n" +
    "    materialSpecular: vec4<f32>,\n" +
    "    materialShininess: f32,\n" +
    "\n" +
    "    lKeyIsPressed: u32\n" +
    "};\n" +
    "\n" +
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) TransformedNormal: vec3<f32>,\n" +
    "    @location(1) LightDirection1: vec3<f32>,\n" +
    "    @location(2) LightDirection2: vec3<f32>,\n" +
    "    @location(3) ViewerVector: vec3<f32>\n" +
    "};\n" +
    "\n" +
    "@group(0) @binding(0)\n" +
    "var<uniform> uniformData: UniformData;\n" +
    "\n" +
    "@vertex\n" +
    "fn main(\n" +
    "    @location(0) pos: vec3<f32>,\n" +
    "    @location(1) nor: vec3<f32>\n" +
    ") -> VertexOutput\n" +
    "{\n" +
    "    var output: VertexOutput;\n" +
    "    var vPosition = vec4<f32>(pos, 1.0);\n" +
    "\n" +
    "    output.Position = uniformData.projectionMatrix * uniformData.viewMatrix * uniformData.modelMatrix * vPosition;\n" +
    "\n" +
    "    // WGSL needs every output to have a value, so they stay zero when the light is off (fragment shader ignores them then)\n" +
    "    if(uniformData.lKeyIsPressed == 1u)\n" +
    "    {\n" +
    "        var eyeCoordinates = uniformData.viewMatrix * uniformData.modelMatrix * vPosition;\n" +
    "        var normalMatrix = mat3x3<f32>(uniformData.normalMatrix[0].xyz, uniformData.normalMatrix[1].xyz, uniformData.normalMatrix[2].xyz);\n" +
    "\n" +
    "        output.TransformedNormal = normalMatrix * nor;\n" +
    "        output.LightDirection1 = uniformData.lightPosition1.xyz - eyeCoordinates.xyz;\n" +
    "        output.LightDirection2 = uniformData.lightPosition2.xyz - eyeCoordinates.xyz;\n" +
    "        output.ViewerVector = -eyeCoordinates.xyz;\n" +
    "    }\n" +
    "\n" +
    "    return output;\n" +
    "}\n";


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
    // (both lights are calculated here, per fragment, same as Shader.frag of the Vulkan version)
    const fragmentShaderSourceCode =
    "struct UniformData\n" +
    "{\n" +
    "    modelMatrix: mat4x4<f32>,\n" +
    "    viewMatrix: mat4x4<f32>,\n" +
    "    projectionMatrix: mat4x4<f32>,\n" +
    "    normalMatrix: mat4x4<f32>,\n" +
    "\n" +
    "    // Light 1\n" +
    "    lightAmbient1: vec4<f32>,\n" +
    "    lightDiffuse1: vec4<f32>,\n" +
    "    lightSpecular1: vec4<f32>,\n" +
    "    lightPosition1: vec4<f32>,\n" +
    "\n" +
    "    // Light 2\n" +
    "    lightAmbient2: vec4<f32>,\n" +
    "    lightDiffuse2: vec4<f32>,\n" +
    "    lightSpecular2: vec4<f32>,\n" +
    "    lightPosition2: vec4<f32>,\n" +
    "\n" +
    "    // Material\n" +
    "    materialAmbient: vec4<f32>,\n" +
    "    materialDiffuse: vec4<f32>,\n" +
    "    materialSpecular: vec4<f32>,\n" +
    "    materialShininess: f32,\n" +
    "\n" +
    "    lKeyIsPressed: u32\n" +
    "};\n" +
    "\n" +
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) TransformedNormal: vec3<f32>,\n" +
    "    @location(1) LightDirection1: vec3<f32>,\n" +
    "    @location(2) LightDirection2: vec3<f32>,\n" +
    "    @location(3) ViewerVector: vec3<f32>\n" +
    "};\n" +
    "\n" +
    "@group(0) @binding(0)\n" +
    "var<uniform> uniformData: UniformData;\n" +
    "\n" +
    "@fragment\n" +
    "fn main(input: VertexOutput) -> @location(0) vec4<f32>\n" +
    "{\n" +
    "    var finalColor = vec3<f32>(1.0, 1.0, 1.0);\n" +
    "\n" +
    "    if(uniformData.lKeyIsPressed == 1u)\n" +
    "    {\n" +
    "        var N = normalize(input.TransformedNormal);\n" +
    "        var V = normalize(input.ViewerVector);\n" +
    "\n" +
    "        // -------- Light 1 --------\n" +
    "        var L1 = normalize(input.LightDirection1);\n" +
    "        var ambient1 = uniformData.lightAmbient1.xyz * uniformData.materialAmbient.xyz;\n" +
    "        var diffuse1 = uniformData.lightDiffuse1.xyz * uniformData.materialDiffuse.xyz * max(dot(N, L1), 0.0);\n" +
    "        var R1 = reflect(-L1, N);\n" +
    "        var specular1 = uniformData.lightSpecular1.xyz * uniformData.materialSpecular.xyz * pow(max(dot(R1, V), 0.0), uniformData.materialShininess);\n" +
    "\n" +
    "        // -------- Light 2 --------\n" +
    "        var L2 = normalize(input.LightDirection2);\n" +
    "        var ambient2 = uniformData.lightAmbient2.xyz * uniformData.materialAmbient.xyz;\n" +
    "        var diffuse2 = uniformData.lightDiffuse2.xyz * uniformData.materialDiffuse.xyz * max(dot(N, L2), 0.0);\n" +
    "        var R2 = reflect(-L2, N);\n" +
    "        var specular2 = uniformData.lightSpecular2.xyz * uniformData.materialSpecular.xyz * pow(max(dot(R2, V), 0.0), uniformData.materialShininess);\n" +
    "\n" +
    "        // final color = sum of both lights\n" +
    "        finalColor = ambient1 + diffuse1 + specular1 + ambient2 + diffuse2 + specular2;\n" +
    "    }\n" +
    "\n" +
    "    return vec4<f32>(finalColor, 1.0);\n" +
    "}\n";

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

    // declare position array for pyramid (4 side faces + 2 bottom triangles = 18 vertices, 3 floats each)
    const pyramidVertices = new Float32Array([
        // Front face
        0.0, 1.0, 0.0,
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0,

        // Right face
        0.0, 1.0, 0.0,
        1.0, -1.0, 1.0,
        1.0, -1.0, -1.0,

        // Back face
        0.0, 1.0, 0.0,
        1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,

        // Left face
        0.0, 1.0, 0.0,
        -1.0, -1.0, -1.0,
        -1.0, -1.0, 1.0,

        // Bottom triangle 1
        -1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,

        // Bottom triangle 2
        -1.0, -1.0, 1.0,
        1.0, -1.0, -1.0,
        1.0, -1.0, 1.0,
    ]);

    // declare normal array for pyramid (one outward normal per vertex, flat shaded : same normal for the 3 vertices of a face)
    const pyramidNormals = new Float32Array([
        // Front face
        0.0, 0.447214, 0.894427,
        0.0, 0.447214, 0.894427,
        0.0, 0.447214, 0.894427,

        // Right face
        0.894427, 0.447214, 0.0,
        0.894427, 0.447214, 0.0,
        0.894427, 0.447214, 0.0,

        // Back face
        0.0, 0.447214, -0.894427,
        0.0, 0.447214, -0.894427,
        0.0, 0.447214, -0.894427,

        // Left face
        -0.894427, 0.447214, 0.0,
        -0.894427, 0.447214, 0.0,
        -0.894427, 0.447214, 0.0,

        // Bottom triangle 1
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,

        // Bottom triangle 2
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
    ]);

    // create vertex buffer for pyramid position
    const bufferDescriptor_Positions = 
    {
        size: pyramidVertices.byteLength,
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

    Queue.writeBuffer(buffer_Positions, 0, pyramidVertices, 0, pyramidVertices.length);

    console.log("initialise() : buffer_Positions is successfully created and data is copied to it.\n");

    // create vertex buffer for pyramid normal
    const bufferDescriptor_Normals = 
    {
        size: pyramidNormals.byteLength,
        usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST
    };

    buffer_Normals = Device.createBuffer(bufferDescriptor_Normals);

    if(buffer_Normals == null)
    {
        console.log("initialise() : buffer_Normals is not supported by your browser.\n");
        throw Error("initialise() : buffer_Normals is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Normals is supported by your browser.\n");
    }

    Queue.writeBuffer(buffer_Normals, 0, pyramidNormals, 0, pyramidNormals.length);

    console.log("initialise() : buffer_Normals is successfully created and data is copied to it.\n");

    // using UDF 1: createBindGroupLayoutUniform
    const bindGroupLayout_Uniform = createBindGroupLayoutUniform(0, GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT, "uniform");

    // create pipeline layout for uniform
    const pipelineLayoutDescriptor = 
    {
        bindGroupLayouts: [bindGroupLayout_Uniform]
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

    // create uniform buffer using UDF 2: createUniformBuffer
    // 4 matrices (model, view, projection, normal) = 4 * 16 floats = 256 bytes
    // 8 vec4 (2 lights x ambient, diffuse, specular, position) = 8 * 4 floats = 128 bytes
    // 3 vec4 (material ambient, diffuse, specular) = 3 * 4 floats = 48 bytes
    // 1 f32 (materialShininess) + 1 u32 (lKeyIsPressed) = 8 bytes
    // total = 440 bytes, struct size is rounded up to a multiple of 16 = 448 bytes
    const uniformBufferSize = 448;

    buffer_Uniform = createUniformBuffer(uniformBufferSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    // using UDF 3: createBindGroupForUniform
    bindingGroup_Uniform = createBindGroupForUniform(buffer_Uniform, 0, uniformBufferSize, 0, bindGroupLayout_Uniform);

    // for position
    const positionVertexAttribute =
    {
        shaderLocation: 0, // this matches with @location(0) in vertex shader
        offset: 0,
        format: "float32x3"
    };

    const positionVetexBufferLayout =
    {
        arrayStride: 3 * 4, // 3 floats of 4 bytes each
        attributes: [positionVertexAttribute],
        stepMode: "vertex"
    };

    // for normal
    const normalVertexAttribute =
    {
        shaderLocation: 1, // this matches with @location(1) in vertex shader
        offset: 0,
        format: "float32x3"
    };

    const normalVetexBufferLayout =
    {
        arrayStride: 3 * 4, // 3 floats of 4 bytes each
        attributes: [normalVertexAttribute],
        stepMode: "vertex"
    };

    const vertexShaderState =
    {
        module: shaderModule_VertexShader,
        entryPoint: "main",
        buffers: [positionVetexBufferLayout, normalVetexBufferLayout]
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

// USER DEFINED FUNCTIONS (3 UDFs for uniform + updateUniformBuffer)

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

function updateUniformBuffer()
{
    // code

    // matrices
    // model matrix : translate * rotateY
    const modelMatrix = mat4.create();
    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -4.0]);
    mat4.rotateY(modelMatrix, modelMatrix, degToRad(angle_Pyramid));

    // view matrix : identity
    const viewMatrix = mat4.create();

    // WGSL has no inverse() function, so the normal matrix = transpose(inverse(view * model)) is calculated here on the CPU
    const modelViewMatrix = mat4.create();
    mat4.multiply(modelViewMatrix, viewMatrix, modelMatrix);

    const normalMatrix = mat4.create();
    mat4.invert(normalMatrix, modelViewMatrix);
    mat4.transpose(normalMatrix, normalMatrix);

    // light 1 related uniform (white light)
    const lightAmbient1 = [0.1, 0.1, 0.1, 1.0];
    const lightDiffuse1 = [1.0, 1.0, 1.0, 1.0];
    const lightSpecular1 = [1.0, 1.0, 1.0, 1.0];
    const lightPosition1 = [100.0, 100.0, 100.0, 1.0];

    // light 2 related uniform (red light)
    const lightAmbient2 = [0.1, 0.0, 0.0, 1.0];
    const lightDiffuse2 = [1.0, 0.0, 0.0, 1.0];
    const lightSpecular2 = [1.0, 0.0, 0.0, 1.0];
    const lightPosition2 = [-100.0, 100.0, 100.0, 1.0];

    // material related uniform
    const materialAmbient = [0.0, 0.0, 0.0, 1.0];
    const materialDiffuse = [0.5, 0.2, 0.7, 1.0];
    const materialSpecular = [0.7, 0.7, 0.7, 1.0];
    const materialShininess = 128.0;

    // one ArrayBuffer viewed as floats and as u32, so the whole uniform goes to the GPU in a single writeBuffer
    const uniformArrayBuffer = new ArrayBuffer(448);
    const uniformFloats = new Float32Array(uniformArrayBuffer);
    const uniformUints = new Uint32Array(uniformArrayBuffer);

    uniformFloats.set(modelMatrix, 0);                      // byte offset 0
    uniformFloats.set(viewMatrix, 16);                      // byte offset 64
    uniformFloats.set(perspectiveProjectionMatrix, 32);     // byte offset 128
    uniformFloats.set(normalMatrix, 48);                    // byte offset 192

    uniformFloats.set(lightAmbient1, 64);                   // byte offset 256
    uniformFloats.set(lightDiffuse1, 68);                   // byte offset 272
    uniformFloats.set(lightSpecular1, 72);                  // byte offset 288
    uniformFloats.set(lightPosition1, 76);                  // byte offset 304

    uniformFloats.set(lightAmbient2, 80);                   // byte offset 320
    uniformFloats.set(lightDiffuse2, 84);                   // byte offset 336
    uniformFloats.set(lightSpecular2, 88);                  // byte offset 352
    uniformFloats.set(lightPosition2, 92);                  // byte offset 368

    uniformFloats.set(materialAmbient, 96);                 // byte offset 384
    uniformFloats.set(materialDiffuse, 100);                // byte offset 400
    uniformFloats.set(materialSpecular, 104);               // byte offset 416
    uniformFloats[108] = materialShininess;                 // byte offset 432

    // key press related uniform
    if(bLight == true)
    {
        uniformUints[109] = 1;                              // byte offset 436
    }
    else
    {
        uniformUints[109] = 0;
    }

    Queue.writeBuffer(buffer_Uniform, 0, uniformArrayBuffer, 0, uniformArrayBuffer.byteLength);
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

    // update all uniforms (matrices, light, key press)
    updateUniformBuffer();

    // start the renderpass
    const renderPassEncoder = commandEncoder.beginRenderPass(renderPassDescriptor);

    renderPassEncoder.setPipeline(renderPipeline);
    renderPassEncoder.setViewport(0, 0, canvas.width, canvas.height, 0, 1);
    renderPassEncoder.setScissorRect(0, 0, canvas.width, canvas.height);
    renderPassEncoder.setVertexBuffer(0, buffer_Positions);
    renderPassEncoder.setVertexBuffer(1, buffer_Normals);
    renderPassEncoder.setBindGroup(0, bindingGroup_Uniform);
    renderPassEncoder.draw(18);  // 18 vertices for pyramid (4 side triangles + 2 bottom triangles)

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
    angle_Pyramid = angle_Pyramid + 1.5;
    if(angle_Pyramid >= 360.0)
    {
        angle_Pyramid = angle_Pyramid - 360.0;
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

    // destroy the device
    if(Device != null)
    {
        Device.destroy();
        Device = null;
        Queue = null;
        buffer_Positions = null;
        buffer_Normals = null;
        renderPipeline = null;
        buffer_Uniform = null;
        bindingGroup_Uniform = null;
    }

    perspectiveProjectionMatrix = null;

    console.log("uninitialise() : WebGPUDevice is destroyed successfully.\n");

}




