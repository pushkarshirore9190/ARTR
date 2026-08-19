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
let buffer_Texcoords = null;
let renderPipeline = null;
let buffer_MVPUniform = null;
let bindingGroup_MVPUniform = null;
let perspectiveProjectionMatrix = null;

let depthTexture = null;
let texture_Smiley = null;
let sampler_Smiley = null;
let bindGroup_Texture_And_Sampler = null;



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
    console.warning("WebGPUDeviceLostReason: ", info.reason , "WebGPUDeviceLostMessage: ", info.message);
    Device = null;
    Queue = null;
    buffer_Positions = null;
    buffer_Texcoords = null;
    renderPipeline = null;
    buffer_MVPUniform = null;
    bindingGroup_MVPUniform = null;
    perspectiveProjectionMatrix = null;
    depthTexture = null;
    texture_Smiley = null;
    sampler_Smiley = null;
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

    // declare position array for rectangle
    const vertex_Position = new Float32Array([
        // First Triangle
        -1.0,  1.0, 0.0, 1.0,   // Top Left
        -1.0, -1.0, 0.0, 1.0,   // Bottom Left
        1.0, -1.0, 0.0, 1.0,   // Bottom Right

        // Second Triangle
        -1.0,  1.0, 0.0, 1.0,   // Top Left
        1.0, -1.0, 0.0, 1.0,   // Bottom Right
        1.0,  1.0, 0.0, 1.0    // Top Right
    ]);

   // declare Texture array for rectangle
    const vertex_Texcoords = new Float32Array([
        // First Triangle
        0.0, 0.0,   // Top Left
        0.0, 1.0,   // Bottom Left
        1.0, 1.0,   // Bottom Right

        // Second Triangle
        0.0, 0.0,   // Top Left
        1.0, 1.0,   // Bottom Right
        1.0, 0.0    // Top Right
    ]);

    // create vertex buffer for rectangle position
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

    console.log("initialise() : buffer_Positions is successfully created and data is copied to it.\n");

    // create vertex buffer for rectangle color
    const bufferDescriptor_Texcoords = 
    {
        size: vertex_Texcoords.byteLength,
        usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST
    };

    buffer_Texcoords = Device.createBuffer(bufferDescriptor_Texcoords);

    if(buffer_Texcoords == null)
    {
        console.log("initialise() : buffer_Texcoords is not supported by your browser.\n");
        throw Error("initialise() : buffer_Texcoords is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Texcoords is supported by your browser.\n");
    }

    Queue.writeBuffer(buffer_Texcoords, 0, vertex_Texcoords, 0, vertex_Texcoords.length);

    console.log("initialise() : buffer_Texcoords is successfully created and data is copied to it.\n");

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

    buffer_MVPUniform = createUniformBuffer(mvpUniformSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    // using UDF 3: createBindGroupForUniform
    bindingGroup_MVPUniform = createBindGroupForUniform(buffer_MVPUniform, 0, mvpUniformSize, 0, bindGroupLayout_mvpUniform);

    // load the image to create the texture
    texture_Smiley = await loadTexture("Smiley.png");

    if(texture_Smiley == null)
    {
        console.log("initialise() : texture_Smiley is not supported by your browser.\n");
        throw Error("initialise() : texture_Smiley is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : texture_Smiley is supported by your browser.\n");
    }

    // texture sampler descriptor
    const samplerDescriptor = 
    {
        magFilter: "linear",
        minFilter: "linear",
    }

    // create texture sampler
    sampler_Smiley = Device.createSampler(samplerDescriptor);

    if(sampler_Smiley == null)
    {
        console.log("initialise() : sampler_Smiley is not supported by your browser.\n");
        throw Error("initialise() : sampler_Smiley is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : sampler_Smiley is supported by your browser.\n"); 
    }

    // create texure and sampler bind group
    bindGroup_Texture_And_Sampler = createBindGroupForTextureAndSampler(
        0,
        texture_Smiley,
        1,
        sampler_Smiley,
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

    const modelViewMatrix = mat4.create();
    const modelViewProjectionMatrix = mat4.create();
    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -4.0]);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    Queue.writeBuffer(buffer_MVPUniform, 0, modelViewProjectionMatrix, 0, modelViewProjectionMatrix.length);


    // start the renderpass
    const renderPassEncoder = commandEncoder.beginRenderPass(renderPassDescriptor);

    renderPassEncoder.setPipeline(renderPipeline);
    renderPassEncoder.setViewport(0, 0, canvas.width, canvas.height, 0, 1);
    renderPassEncoder.setScissorRect(0, 0, canvas.width, canvas.height);
    renderPassEncoder.setVertexBuffer(0, buffer_Positions);
    renderPassEncoder.setVertexBuffer(1, buffer_Texcoords);
    renderPassEncoder.setBindGroup(0, bindingGroup_MVPUniform);
    renderPassEncoder.setBindGroup(1, bindGroup_Texture_And_Sampler);
    renderPassEncoder.draw(6);  // 6 vertices for rectangle (2 triangles)

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

    // destroy texture and sampler
    if(texture_Smiley != null)
    {
        texture_Smiley.destroy();
        texture_Smiley = null;
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
        buffer_Texcoords = null;
        renderPipeline = null;
        buffer_MVPUniform = null;
        bindingGroup_MVPUniform = null;
        sampler_Smiley = null;
        bindGroup_Texture_And_Sampler = null;
    }

    perspectiveProjectionMatrix = null;

    console.log("uninitialise() : WebGPUDevice is destroyed successfully.\n");

}



