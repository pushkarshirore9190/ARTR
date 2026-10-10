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


let renderPipeline_PV = null;
let renderPipeline_PF = null;
let buffer_MVPUniform = null;
let bindingGroup_MVPUniform = null;
let perspectiveProjectionMatrix = null;

let depthTexture = null;

let Sphere = null;
let numMeshIndices = 0;
let buffer_Positions = null;

let buffer_Normals = null;
let buffer_Texcoords = null;
let buffer_Elements = null;

// light toggle : false -> white sphere, true -> Phong lit sphere
var bLight = false;

// shader selection : 'V' -> per vertex, 'F' -> per fragment
var choosenShader = 'V';




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
    renderPipeline_PV = null;
    renderPipeline_PF = null;
    buffer_MVPUniform = null;
    bindingGroup_MVPUniform = null;
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
        case "Q":
        case "q":
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

        case "V":
        case "v":
            choosenShader = 'V'; // Per-Vertex
            break;

        case "P":
        case "p":
            choosenShader = 'F'; // Per-Fragment
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

    // vertex shader code in WGSL (PV)
    const vertexShaderSourceCode_PV =
    "struct UniformData\n" +
    "{\n" +
    "    modelMatrix: mat4x4<f32>,\n" +
    "    viewMatrix: mat4x4<f32>,\n" +
    "    projectionMatrix: mat4x4<f32>,\n" +
    "    normalMatrix: mat4x4<f32>,\n" +
    "\n" +
    "    lightAmbient: vec4<f32>,\n" +
    "    lightDiffuse: vec4<f32>,\n" +
    "    lightSpecular: vec4<f32>,\n" +
    "    lightPosition: vec4<f32>,\n" +
    "\n" +
    "    materialAmbient: vec4<f32>,\n" +
    "    materialDiffuse: vec4<f32>,\n" +
    "    materialSpecular: vec4<f32>,\n" +
    "    materialShininess: f32,\n" +
    "\n" +
    "    lKeyIsPressed: u32\n" +
    "};\n" +
    "\n" +
    "@group(0) @binding(0)\n" +
    "var<uniform> uniformData: UniformData;\n" +
    "\n" +
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) VertexColor: vec4<f32>\n" +
    "};\n" +
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
    "    if(uniformData.lKeyIsPressed == 1u)\n" +
    "    {\n" +
    "        var eyeCoordinates = uniformData.viewMatrix * uniformData.modelMatrix * vPosition;\n" +
    "        var normalMatrix = mat3x3<f32>(uniformData.normalMatrix[0].xyz, uniformData.normalMatrix[1].xyz, uniformData.normalMatrix[2].xyz);\n" +
    "        var transformedNormal = normalize(normalMatrix * nor);\n" +
    "        var lightDirection = normalize(uniformData.lightPosition.xyz - eyeCoordinates.xyz);\n" +
    "        var viewerVector = normalize(-eyeCoordinates.xyz);\n" +
    "\n" +
    "        var ambient = uniformData.lightAmbient.xyz * uniformData.materialAmbient.xyz;\n" +
    "        var diffuse = uniformData.lightDiffuse.xyz * uniformData.materialDiffuse.xyz * max(dot(lightDirection, transformedNormal), 0.0);\n" +
    "        var reflectionVector = reflect(-lightDirection, transformedNormal);\n" +
    "        var specular = uniformData.lightSpecular.xyz * uniformData.materialSpecular.xyz * pow(max(dot(reflectionVector, viewerVector), 0.0), uniformData.materialShininess);\n" +
    "\n" +
    "        output.VertexColor = vec4<f32>(ambient + diffuse + specular, 1.0);\n" +
    "    }\n" +
    "    else\n" +
    "    {\n" +
    "        output.VertexColor = vec4<f32>(1.0, 1.0, 1.0, 1.0);\n" +
    "    }\n" +
    "\n" +
    "    return output;\n" +
    "}\n";

    const shaderModuleDescriptor_VertexShader_PV = 
    {
        code: vertexShaderSourceCode_PV
    };

    const shaderModule_VertexShader_PV = Device.createShaderModule(shaderModuleDescriptor_VertexShader_PV);

    if(shaderModule_VertexShader_PV == null)
    {
        console.log("initialise() : shaderModule_VertexShader_PV is not supported by your browser.\n");
        throw Error("initialise() : shaderModule_VertexShader_PV is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : shaderModule_VertexShader_PV is supported by your browser.\n");
    }

    // fragment shader code in WGSL (PV)
    const fragmentShaderSourceCode_PV =
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) VertexColor: vec4<f32>\n" +
    "};\n" +
    "\n" +
    "@fragment\n" +
    "fn main(input: VertexOutput) -> @location(0) vec4<f32>\n" +
    "{\n" +
    "    return input.VertexColor;\n" +
    "}\n";

    const shaderModuleDescriptor_FragmentShader_PV = 
    {
        code: fragmentShaderSourceCode_PV
    };

    const shaderModule_FragmentShader_PV = Device.createShaderModule(shaderModuleDescriptor_FragmentShader_PV);

    if(shaderModule_FragmentShader_PV == null)
    {
        console.log("initialise() : shaderModule_FragmentShader_PV is not supported by your browser.\n");
        throw Error("initialise() : shaderModule_FragmentShader_PV is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : shaderModule_FragmentShader_PV is supported by your browser.\n");
    }

    // vertex shader code in WGSL (PF)
    const vertexShaderSourceCode_PF =
    "struct UniformData\n" +
    "{\n" +
    "    modelMatrix: mat4x4<f32>,\n" +
    "    viewMatrix: mat4x4<f32>,\n" +
    "    projectionMatrix: mat4x4<f32>,\n" +
    "    normalMatrix: mat4x4<f32>,\n" +
    "\n" +
    "    lightAmbient: vec4<f32>,\n" +
    "    lightDiffuse: vec4<f32>,\n" +
    "    lightSpecular: vec4<f32>,\n" +
    "    lightPosition: vec4<f32>,\n" +
    "\n" +
    "    materialAmbient: vec4<f32>,\n" +
    "    materialDiffuse: vec4<f32>,\n" +
    "    materialSpecular: vec4<f32>,\n" +
    "    materialShininess: f32,\n" +
    "\n" +
    "    lKeyIsPressed: u32\n" +
    "};\n" +
    "\n" +
    "@group(0) @binding(0)\n" +
    "var<uniform> uniformData: UniformData;\n" +
    "\n" +
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) TransformedNormal: vec3<f32>,\n" +
    "    @location(1) LightDirection: vec3<f32>,\n" +
    "    @location(2) ViewerVector: vec3<f32>\n" +
    "};\n" +
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
    "        output.TransformedNormal = normalMatrix * nor;\n" +
    "        output.LightDirection = uniformData.lightPosition.xyz - eyeCoordinates.xyz;\n" +
    "        output.ViewerVector = -eyeCoordinates.xyz;\n" +
    "    }\n" +
    "\n" +
    "    return output;\n" +
    "}\n";

    const shaderModuleDescriptor_VertexShader_PF = 
    {
        code: vertexShaderSourceCode_PF
    };

    const shaderModule_VertexShader_PF = Device.createShaderModule(shaderModuleDescriptor_VertexShader_PF);

    if(shaderModule_VertexShader_PF == null)
    {
        console.log("initialise() : shaderModule_VertexShader_PF is not supported by your browser.\n");
        throw Error("initialise() : shaderModule_VertexShader_PF is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : shaderModule_VertexShader_PF is supported by your browser.\n");
    }

    // fragment shader code in WGSL (PF)
    const fragmentShaderSourceCode_PF =
    "struct UniformData\n" +
    "{\n" +
    "    modelMatrix: mat4x4<f32>,\n" +
    "    viewMatrix: mat4x4<f32>,\n" +
    "    projectionMatrix: mat4x4<f32>,\n" +
    "    normalMatrix: mat4x4<f32>,\n" +
    "\n" +
    "    lightAmbient: vec4<f32>,\n" +
    "    lightDiffuse: vec4<f32>,\n" +
    "    lightSpecular: vec4<f32>,\n" +
    "    lightPosition: vec4<f32>,\n" +
    "\n" +
    "    materialAmbient: vec4<f32>,\n" +
    "    materialDiffuse: vec4<f32>,\n" +
    "    materialSpecular: vec4<f32>,\n" +
    "    materialShininess: f32,\n" +
    "\n" +
    "    lKeyIsPressed: u32\n" +
    "};\n" +
    "\n" +
    "@group(0) @binding(0)\n" +
    "var<uniform> uniformData: UniformData;\n" +
    "\n" +
    "struct VertexOutput\n" +
    "{\n" +
    "    @builtin(position) Position: vec4<f32>,\n" +
    "    @location(0) TransformedNormal: vec3<f32>,\n" +
    "    @location(1) LightDirection: vec3<f32>,\n" +
    "    @location(2) ViewerVector: vec3<f32>\n" +
    "};\n" +
    "\n" +
    "@fragment\n" +
    "fn main(input: VertexOutput) -> @location(0) vec4<f32>\n" +
    "{\n" +
    "    var phongLight = vec3<f32>(1.0, 1.0, 1.0);\n" +
    "\n" +
    "    if(uniformData.lKeyIsPressed == 1u)\n" +
    "    {\n" +
    "        var N = normalize(input.TransformedNormal);\n" +
    "        var L = normalize(input.LightDirection);\n" +
    "        var V = normalize(input.ViewerVector);\n" +
    "\n" +
    "        var ambient = uniformData.lightAmbient.xyz * uniformData.materialAmbient.xyz;\n" +
    "        var diffuse = uniformData.lightDiffuse.xyz * uniformData.materialDiffuse.xyz * max(dot(L, N), 0.0);\n" +
    "        var R = reflect(-L, N);\n" +
    "        var specular = uniformData.lightSpecular.xyz * uniformData.materialSpecular.xyz * pow(max(dot(R, V), 0.0), uniformData.materialShininess);\n" +
    "\n" +
    "        phongLight = ambient + diffuse + specular;\n" +
    "    }\n" +
    "\n" +
    "    return vec4<f32>(phongLight, 1.0);\n" +
    "}\n";

    const shaderModuleDescriptor_FragmentShader_PF = 
    {
        code: fragmentShaderSourceCode_PF
    };

    const shaderModule_FragmentShader_PF = Device.createShaderModule(shaderModuleDescriptor_FragmentShader_PF);

    if(shaderModule_FragmentShader_PF == null)
    {
        console.log("initialise() : shaderModule_FragmentShader_PF is not supported by your browser.\n");
        throw Error("initialise() : shaderModule_FragmentShader_PF is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : shaderModule_FragmentShader_PF is supported by your browser.\n");
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
    const bindGroupLayout_mvpUniform = createBindGroupLayoutUniform(0, GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT, "uniform");

    // create pipeline layout for mvp uniform
    const pipelineLayoutDescriptor = 
    {
        bindGroupLayouts: [bindGroupLayout_mvpUniform]
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

    // create uniform buffer (matrices + light + material + key) using UDF 2: createUniformBuffer
    // 4 matrices (model, view, projection, normal)  = 4 * 16 floats = 256 bytes
    // 7 vec4 (3 light colors, light position, 3 material colors) = 7 * 4 floats = 112 bytes
    // 1 f32 (materialShininess) + 1 u32 (lKeyIsPressed) = 8 bytes
    // total = 376 bytes, struct size is rounded up to a multiple of 16 = 384 bytes
    const mvpUniformSize = 96 * 4;

    buffer_MVPUniform = createUniformBuffer(mvpUniformSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    // using UDF 3: createBindGroupForUniform
    bindingGroup_MVPUniform = createBindGroupForUniform(buffer_MVPUniform, 0, mvpUniformSize, 0, bindGroupLayout_mvpUniform);

    // for position
    const positionVertexAttribute =
    {
        shaderLocation: 0, // this matches with @location(0) in vertex shader
        offset: 0,
        format: "float32x3"
    };

    const positionVetexBufferLayout =
    {
        arrayStride: Float32Array.BYTES_PER_ELEMENT * 3, // 3 floats of 4 bytes each
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
        arrayStride: Float32Array.BYTES_PER_ELEMENT * 3, // 3 floats of 4 bytes each
        attributes: [normalVertexAttribute],
        stepMode: "vertex"
    };

    // vertex shader states (per vertex and per fragment)
    const vertexShaderState_PV =
    {
        module: shaderModule_VertexShader_PV,
        entryPoint: "main",
        buffers: [positionVetexBufferLayout, normalVetexBufferLayout]
    };

    const vertexShaderState_PF =
    {
        module: shaderModule_VertexShader_PF,
        entryPoint: "main",
        buffers: [positionVetexBufferLayout, normalVetexBufferLayout]
    };

    const colorTargetState =
    {
        format: canvasFormat,
    };

    // fragment shader states (per vertex and per fragment)
    const fragmentShaderState_PV =
    {
        module: shaderModule_FragmentShader_PV,
        entryPoint: "main",
        targets: [colorTargetState]
    };

    const fragmentShaderState_PF =
    {
        module: shaderModule_FragmentShader_PF,
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

    // create final pipeline descriptors (same layout, same states, only the shaders are different)
    const pipelineDescriptor_PV =
    {
        layout: pipelineLayout,
        vertex: vertexShaderState_PV,
        fragment: fragmentShaderState_PV,
        primitive: premitiveState,
        depthStencil: depthStencilState,
    };

    const pipelineDescriptor_PF =
    {
        layout: pipelineLayout,
        vertex: vertexShaderState_PF,
        fragment: fragmentShaderState_PF,
        primitive: premitiveState,
        depthStencil: depthStencilState,
    };

    renderPipeline_PV = Device.createRenderPipeline(pipelineDescriptor_PV);

    if(renderPipeline_PV == null)
    {
        console.log("initialise() : renderPipeline_PV is not supported by your browser.\n");
        throw Error("initialise() : renderPipeline_PV is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : renderPipeline_PV is supported by your browser.\n");
    }

    renderPipeline_PF = Device.createRenderPipeline(pipelineDescriptor_PF);

    if(renderPipeline_PF == null)
    {
        console.log("initialise() : renderPipeline_PF is not supported by your browser.\n");
        throw Error("initialise() : renderPipeline_PF is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : renderPipeline_PF is supported by your browser.\n");
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

    // matrices
    const modelMatrix = mat4.create();
    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);

    // view matrix : identity
    const viewMatrix = mat4.create();

    const modelViewMatrix = mat4.create();
    mat4.multiply(modelViewMatrix, viewMatrix, modelMatrix);

    // WGSL has no inverse() function, so the normal matrix = transpose(inverse(view * model)) is calculated here on the CPU
    const normalMatrix = mat4.create();
    mat4.invert(normalMatrix, modelViewMatrix);
    mat4.transpose(normalMatrix, normalMatrix);

    // light related uniform
    const lightAmbient = [0.1, 0.1, 0.1, 1.0];
    const lightDiffuse = [1.0, 1.0, 1.0, 1.0];
    const lightSpecular = [1.0, 1.0, 1.0, 1.0];
    const lightPosition = [100.0, 100.0, 100.0, 1.0];

    // material related uniform
    const materialAmbient = [0.0, 0.0, 0.0, 1.0];
    const materialDiffuse = [0.5, 0.2, 0.7, 1.0];
    const materialSpecular = [0.7, 0.7, 0.7, 1.0];
    const materialShininess = 128.0;

    // one ArrayBuffer viewed as floats and as u32, so the whole uniform goes to the GPU in a single writeBuffer
    const uniformArrayBuffer = new ArrayBuffer(buffer_MVPUniform.size);
    const uniformFloats = new Float32Array(uniformArrayBuffer);
    const uniformUints = new Uint32Array(uniformArrayBuffer);

    uniformFloats.set(modelMatrix, 0);                      // byte offset 0
    uniformFloats.set(viewMatrix, 16);                      // byte offset 64
    uniformFloats.set(perspectiveProjectionMatrix, 32);     // byte offset 128
    uniformFloats.set(normalMatrix, 48);                    // byte offset 192

    uniformFloats.set(lightAmbient, 64);                    // byte offset 256
    uniformFloats.set(lightDiffuse, 68);                    // byte offset 272
    uniformFloats.set(lightSpecular, 72);                   // byte offset 288
    uniformFloats.set(lightPosition, 76);                   // byte offset 304

    uniformFloats.set(materialAmbient, 80);                 // byte offset 320
    uniformFloats.set(materialDiffuse, 84);                 // byte offset 336
    uniformFloats.set(materialSpecular, 88);                // byte offset 352
    uniformFloats[92] = materialShininess;                  // byte offset 368

    // key press related uniform
    if(bLight == true)
    {
        uniformUints[93] = 1;                               // byte offset 372
    }
    else
    {
        uniformUints[93] = 0;
    }

    Queue.writeBuffer(buffer_MVPUniform, 0, uniformArrayBuffer, 0, uniformArrayBuffer.byteLength);


    // start the renderpass
    const renderPassEncoder = commandEncoder.beginRenderPass(renderPassDescriptor);

    // bind with the pipeline
    if(choosenShader == 'V')
    {
        renderPassEncoder.setPipeline(renderPipeline_PV);
    }
    else if(choosenShader == 'F')
    {
        renderPassEncoder.setPipeline(renderPipeline_PF);
    }

    renderPassEncoder.setViewport(0, 0, canvas.width, canvas.height, 0, 1);
    renderPassEncoder.setScissorRect(0, 0, canvas.width, canvas.height);
    renderPassEncoder.setVertexBuffer(0, buffer_Positions);
    renderPassEncoder.setVertexBuffer(1, buffer_Normals);
    renderPassEncoder.setIndexBuffer(buffer_Elements, "uint16");
    renderPassEncoder.setBindGroup(0, bindingGroup_MVPUniform);
    renderPassEncoder.drawIndexed(numMeshIndices); // indexed draw of the sphere

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
        renderPipeline_PV = null;
        renderPipeline_PF = null;
        buffer_MVPUniform = null;
        bindingGroup_MVPUniform = null;
        buffer_Positions = null;
        buffer_Normals = null;
        buffer_Texcoords = null;
        buffer_Elements = null;

    }

    Sphere = null;

    perspectiveProjectionMatrix = null;

    console.log("uninitialise() : WebGPUDevice is destroyed successfully.\n");

}



