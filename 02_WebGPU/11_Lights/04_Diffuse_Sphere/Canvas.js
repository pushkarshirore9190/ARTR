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
    window.msCancelRequestAnimationFrame;


// WebGPU related varible
var clearColor;
let Device = null;
let Context = null;
let Queue = null;
let canvasFormat = null;
let animationFrameId = null;

let Sphere = null;
let numMeshIndices = 0;
let buffer_Positions = null;
let buffer_Normals = null;
let buffer_Elements = null;
let renderPipeline = null;
let buffer_Uniform = null;
let bindingGroup_Uniform = null;
let perspectiveProjectionMatrix = null;

let depthTexture = null;

var angle_Sphere = 0.0;

// light toggle : false -> white sphere, true -> diffuse lit sphere
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
    window.addEventListener("keydown", keyDown, false);
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

    const adapter = await GPU.requestAdapter();
    if(adapter == null) 
    {
        console.log("adapter is not supported by your browser.\n");
        throw Error("adapter is not supported by your browser.\n");
    }

    Device = await adapter.requestDevice();
    if(Device == null)
    {
        console.log("Device is not supported by your browser.\n");
        throw Error("Device is not supported by your browser.\n");
    }

    Device.addEventListener("uncapturederror", onUncapturedError);
    Device.lost.then(onDeviceLost);

    initialise();
    resize();
    display();
}

function onUncapturedError(event)
{
    console.error("WebGPUUncapturedError: ", event.error.message);
}

function onDeviceLost(info)
{
    console.warn("WebGPUDeviceLostReason: ", info.reason , "WebGPUDeviceLostMessage: ", info.message);
    Device = null;
    Queue = null;
    Sphere = null;
    numMeshIndices = 0;
    buffer_Positions = null;
    buffer_Normals = null;
    buffer_Elements = null;
    renderPipeline = null;
    buffer_Uniform = null;
    bindingGroup_Uniform = null;
    perspectiveProjectionMatrix = null;
    depthTexture = null;
}

function keyDown(event)
{
    switch (event.key)
    {
        case "Escape":
            uninitialise();
            window.close();
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

    if (full_element == null) 
    {
        if (canvas.requestFullscreen) canvas.requestFullscreen();
        else if (canvas.webkitRequestFullscreen) canvas.webkitRequestFullscreen();
        else if (canvas.mozRequestFullScreen) canvas.mozRequestFullScreen();
        else if (canvas.msRequestFullscreen) canvas.msRequestFullscreen();
    } 
    else 
    {
        if (document.exitFullscreen) document.exitFullscreen();
        else if (document.webkitExitFullscreen) document.webkitExitFullscreen();
        else if (document.mozCancelFullScreen) document.mozCancelFullScreen();
        else if (document.msExitFullscreen) document.msExitFullscreen();
    }
}

function onFullScreenChange()
{
    var full_element =
        document.fullscreenElement ||
        document.webkitFullscreenElement ||
        document.mozFullscreenElement ||
        document.msFullscreenElement ||
        null;

    if (full_element == null) bFullScreen = false;
    else bFullScreen = true;

    resize();
} 

function initialise() 
{
    Queue = Device.queue;
    console.log("initialise() : WebGPUQueue is obtained successfully.\n");

    Context = canvas.getContext("webgpu");
    if(Context == null)
    {
        console.log("initialise() :WebGPUContext is not supported by your browser.\n");
        throw Error("initialise() :WebGPUContext is not supported by your browser.\n");
    }

    canvasFormat = navigator.gpu.getPreferredCanvasFormat();

    const canvasConfiguration = 
    {
        device: Device,
        format: canvasFormat,
        usage : GPUTextureUsage.RENDER_ATTACHMENT | GPUTextureUsage.COPY_SRC,
        alphaMode: "opaque"
    };

    Context.configure(canvasConfiguration);

    // vertex shader code in WGSL
    const vertexShaderSourceCode =
    "struct UniformData\n" +
    "{\n" +
    "    modelMatrix: mat4x4<f32>,\n" +
    "    viewMatrix: mat4x4<f32>,\n" +
    "    projectionMatrix: mat4x4<f32>,\n" +
    "    normalMatrix: mat4x4<f32>,\n" +
    "    lightDiffused: vec4<f32>,\n" +
    "    lightPosition: vec4<f32>,\n" +
    "    materialDiffused: vec4<f32>,\n" +
    "    lKeyIsPressed: u32\n" +
    "};\n" +
    "\n" +
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) DiffusedLight: vec4<f32>\n" +
    "};\n" +
    "\n" +
    "@group(0) @binding(0)\n" +
    "var<uniform> uniformData: UniformData;\n" +
    "\n" +
    "@vertex\n" +
    "fn main(@location(0) pos: vec3<f32>, @location(1) nor: vec3<f32>) -> VertexOutput\n" +
    "{\n" +
    "    var output: VertexOutput;\n" +
    "    var vPosition = vec4<f32>(pos, 1.0);\n" +
    "    output.Position = uniformData.projectionMatrix * uniformData.viewMatrix * uniformData.modelMatrix * vPosition;\n" +
    "    if(uniformData.lKeyIsPressed == 1u)\n" +
    "    {\n" +
    "        var eyeCoordinates = uniformData.viewMatrix * uniformData.modelMatrix * vPosition;\n" +
    "        var normalMatrix = mat3x3<f32>(uniformData.normalMatrix[0].xyz, uniformData.normalMatrix[1].xyz, uniformData.normalMatrix[2].xyz);\n" +
    "        var transformedNormals = normalize(normalMatrix * nor);\n" +
    "        var lightSource = normalize((uniformData.lightPosition - eyeCoordinates).xyz);\n" +
    "        output.DiffusedLight = vec4<f32>(uniformData.lightDiffused.xyz * (uniformData.materialDiffused.xyz * max(dot(lightSource, transformedNormals), 0.0)), 1.0);\n" +
    "    }\n" +
    "    else\n" +
    "    {\n" +
    "        output.DiffusedLight = vec4<f32>(1.0, 1.0, 1.0, 1.0);\n" +
    "    }\n" +
    "    return output;\n" +
    "}";

    const shaderModule_VertexShader = Device.createShaderModule(
    {
        code: vertexShaderSourceCode
    });

    // fragment shader code in WGSL
    const fragmentShaderSourceCode =
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) DiffusedLight: vec4<f32>\n" +
    "};\n" +
    "\n" +
    "@fragment\n" +
    "fn main(output: VertexOutput) -> @location(0) vec4<f32>\n" +
    "{\n" +
    "    return output.DiffusedLight;\n" +
    "}";

    const shaderModule_FragmentShader = Device.createShaderModule(
    {
        code: fragmentShaderSourceCode
    });

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

    // create vertex buffer for normals
    buffer_Normals = createVertexBuffer(meshData.normalsArray);

    // create index buffer
    buffer_Elements = createIndexBuffer(meshData.indicesArray);

    // using UDF 1: createBindGroupLayoutUniform
    const bindGroupLayout_Uniform = createBindGroupLayoutUniform(0, GPUShaderStage.VERTEX, "uniform");

    const pipelineLayoutDescriptor = 
    {
        bindGroupLayouts: [bindGroupLayout_Uniform]
    };

    const pipelineLayout = Device.createPipelineLayout(pipelineLayoutDescriptor);

    // 4 matrices + 3 vec4 + u32, rounded up to 320 bytes
    const uniformBufferSize = 320;

    buffer_Uniform = createUniformBuffer(uniformBufferSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    bindingGroup_Uniform = createBindGroupForUniform(buffer_Uniform, 0, uniformBufferSize, 0, bindGroupLayout_Uniform);

    // for position
    const positionVertexAttribute =
    {
        shaderLocation: 0,
        offset: 0,
        format: "float32x3"
    };

    const positionVetexBufferLayout =
    {
        arrayStride: 3 * 4,
        attributes: [positionVertexAttribute],
        stepMode: "vertex"
    };

    // for normal
    const normalVertexAttribute =
    {
        shaderLocation: 1,
        offset: 0,
        format: "float32x3"
    };

    const normalVetexBufferLayout =
    {
        arrayStride: 3 * 4,
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

    const depthStencilState =
    {
        format: "depth24plus-stencil8",
        depthWriteEnabled: true,
        depthCompare: "less-equal"
    };

    const pipelineDescriptor =
    {
        layout: pipelineLayout,
        vertex: vertexShaderState,
        fragment: fragmentShaderState,
        primitive: premitiveState,
        depthStencil: depthStencilState,
    };

    renderPipeline = Device.createRenderPipeline(pipelineDescriptor);

    perspectiveProjectionMatrix = mat4.create();

    clearColor = { r: 0.0, g: 0.0, b: 0.0, a: 1.0 };
}


// USER DEFINED FUNCTIONS

function createBindGroupLayoutUniform(_bindingIndex, _shaderStageVisibility, _UniformType)
{
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

    return Device.createBindGroupLayout(bindGroupLayoutDescriptor);
}

function createUniformBuffer(_uniformBufferSize, _uniformBufferUsage)
{
    const bufferDescriptor =
    {
        size: _uniformBufferSize,
        usage: _uniformBufferUsage
    };

    return Device.createBuffer(bufferDescriptor);
}

function createBindGroupForUniform(_uniformBuffer, _uniformBufferOffset, _uniformBufferSize, _bindingIndex, _bindGroupLayout)
{
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

    return Device.createBindGroup(bindGroupDescriptor);
}

function createVertexBuffer(_vertexData)
{
    const vertexBufferDescriptor =
    {
        size: _vertexData.byteLength,
        usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST
    };

    const buffer = Device.createBuffer(vertexBufferDescriptor);

    Queue.writeBuffer(buffer, 0, _vertexData, 0, _vertexData.length);

    return buffer;
}

function createIndexBuffer(_indexData)
{
    const indexBufferDescriptor =
    {
        size: _indexData.byteLength,
        usage: GPUBufferUsage.INDEX | GPUBufferUsage.COPY_DST
    };

    const buffer = Device.createBuffer(indexBufferDescriptor);

    Queue.writeBuffer(buffer, 0, _indexData, 0, _indexData.length);

    return buffer;
}

function updateUniformBuffer()
{
    // model matrix
    const modelMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -5.0]);
    mat4.scale(modelMatrix, modelMatrix, [0.75, 0.75, 0.75]);
    mat4.rotateX(modelMatrix, modelMatrix, degToRad(angle_Sphere));
    mat4.rotateY(modelMatrix, modelMatrix, degToRad(angle_Sphere));
    mat4.rotateZ(modelMatrix, modelMatrix, degToRad(angle_Sphere));

    // view matrix : identity
    const viewMatrix = mat4.create();

    // normal matrix
    const modelViewMatrix = mat4.create();

    mat4.multiply(modelViewMatrix, viewMatrix, modelMatrix);

    const normalMatrix = mat4.create();

    mat4.invert(normalMatrix, modelViewMatrix);
    mat4.transpose(normalMatrix, normalMatrix);

    // light related uniform
    const lightDiffused = [1.0, 1.0, 1.0, 1.0];
    const lightPosition = [0.0, 0.0, 2.0, 1.0];
    const materialDiffused = [0.5, 0.5, 0.5, 1.0];

    const uniformArrayBuffer = new ArrayBuffer(320);
    const uniformFloats = new Float32Array(uniformArrayBuffer);
    const uniformUints = new Uint32Array(uniformArrayBuffer);

    uniformFloats.set(modelMatrix, 0);
    uniformFloats.set(viewMatrix, 16);
    uniformFloats.set(perspectiveProjectionMatrix, 32);
    uniformFloats.set(normalMatrix, 48);
    uniformFloats.set(lightDiffused, 64);
    uniformFloats.set(lightPosition, 68);
    uniformFloats.set(materialDiffused, 72);

    if(bLight == true)
    {
        uniformUints[76] = 1;
    }
    else
    {
        uniformUints[76] = 0;
    }

    Queue.writeBuffer(buffer_Uniform, 0, uniformArrayBuffer, 0, uniformArrayBuffer.byteLength);
}

function resize()
{
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

    if(Device != null)
    {
        if(depthTexture !=null)
        {
            depthTexture.destroy();
            depthTexture = null;
        }

        const depthTextureDescriptor =
        {
            size: [canvas.width, canvas.height, 1],
            dimension : "2d",
            format: "depth24plus-stencil8",
            usage: GPUTextureUsage.RENDER_ATTACHMENT | GPUTextureUsage.COPY_SRC
        };

        depthTexture = Device.createTexture(depthTextureDescriptor);
    }

    mat4.perspective(
        perspectiveProjectionMatrix,
        45.0 * Math.PI / 180.0,
        parseFloat(canvas.width) / parseFloat(canvas.height),
        0.1,
        100.0
    );
}

function display()
{
    if(Device == null)
    {
        return;
    }

    const commandEncoder = Device.createCommandEncoder();

    const renderPassDepthAttachment =
    {
        view: depthTexture.createView(),
        depthClearValue: 1.0,
        depthLoadOp: "clear",
        depthStoreOp: "store",
        stencilClearValue: 0,
        stencilLoadOp: "clear",
        stencilStoreOp: "store"
    };

    const renderPassColorAttachment =
    {
        view: Context.getCurrentTexture().createView(),
        clearValue: clearColor,
        loadOp: "clear",
        storeOp: "store"
    };

    const renderPassDescriptor =
    {
        colorAttachments: [renderPassColorAttachment],
        depthStencilAttachment: renderPassDepthAttachment
    };

    updateUniformBuffer();

    const renderPassEncoder = commandEncoder.beginRenderPass(renderPassDescriptor);

    renderPassEncoder.setPipeline(renderPipeline);
    renderPassEncoder.setViewport(0, 0, canvas.width, canvas.height, 0, 1);
    renderPassEncoder.setScissorRect(0, 0, canvas.width, canvas.height);

    renderPassEncoder.setVertexBuffer(0, buffer_Positions);
    renderPassEncoder.setVertexBuffer(1, buffer_Normals);

    renderPassEncoder.setIndexBuffer(buffer_Elements, "uint16");

    renderPassEncoder.setBindGroup(0, bindingGroup_Uniform);

    renderPassEncoder.drawIndexed(numMeshIndices);

    renderPassEncoder.end();

    Queue.submit([commandEncoder.finish()]);

    animationFrameId = requestAnimationFrame(display);

    update();
}

function update()
{
    angle_Sphere = angle_Sphere + 1.5;

    if(angle_Sphere >= 360.0)
    {
        angle_Sphere = angle_Sphere - 360.0;
    }
}

function uninitialise()
{
    if(animationFrameId != null)
    {
        cancelAnimationFrame(animationFrameId);
        animationFrameId = null;
    }

    if(depthTexture != null)
    {
        depthTexture.destroy();
        depthTexture = null;
    }

    if(Context != null)
    {
        Context.unconfigure();
        Context = null;
    }

    if(Device != null)
    {
        Device.destroy();

        Device = null;
        Queue = null;
        Sphere = null;
        buffer_Positions = null;
        buffer_Normals = null;
        buffer_Elements = null;
        renderPipeline = null;
        buffer_Uniform = null;
        bindingGroup_Uniform = null;
    }

    perspectiveProjectionMatrix = null;

    console.log("uninitialise() : WebGPUDevice is destroyed successfully.\n");
}



