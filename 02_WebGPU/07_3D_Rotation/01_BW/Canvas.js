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
let buffer_Positions_Cube = null;
let renderPipeline = null;
let buffer_MVPUniform_Pyramid = null;
let buffer_MVPUniform_Cube = null;
let bindingGroup_MVPUniform_Pyramid = null;
let bindingGroup_MVPUniform_Cube = null;
let perspectiveProjectionMatrix = null;

var angle_Pyramid = 0.0;
var angle_Cube = 0.0;

let depthTexture = null;



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
    buffer_Positions_Pyramid = null;
    buffer_Positions_Cube = null;
    renderPipeline = null;
    buffer_MVPUniform_Pyramid = null;
    buffer_MVPUniform_Cube = null;
    bindingGroup_MVPUniform_Pyramid = null;
    bindingGroup_MVPUniform_Cube = null;
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
   const vertexShaderSourceCode =
    "struct MVPUniform\n" +
    "{\n" +
    "    MVPMatrix: mat4x4<f32>\n" +
    "};\n" +
    "\n" +
    "@group(0) @binding(0)\n" +
    "var<uniform> mvpUniform: MVPUniform;\n" +
    "\n" +
    "@vertex\n" +
    "fn main(\n" +
    "    @location(0) pos: vec4<f32>\n" +
    ") -> @builtin(position) vec4<f32>\n" +
    "{\n" +
    "    let vPosition = mvpUniform.MVPMatrix * pos;\n" +
    "    return vPosition;\n" +
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
    "@fragment\n" +
    "fn main() -> @location(0) vec4<f32>\n" +
    "{\n" +
    "    return vec4<f32>(1.0, 1.0, 1.0, 1.0);\n" +
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

    // declare postion array for triangle

   const pyramidVertices = new Float32Array([
    // Front face
     0.0,  1.0,  0.0, 1.0,
    -1.0, -1.0,  1.0, 1.0,
     1.0, -1.0,  1.0, 1.0,

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


   const cubeVertices = new Float32Array([
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
    
    // as we have only one uniform MVP its binding layout is common for both 

    const bindGroupLayout_mvpUniform = createBindGroupLayoutUniform(0, GPUShaderStage.VERTEX , "uniform");

    // triangle : its postion buffer , its uniform buffer, its bind group

    // position buffer

    // create vertex buffer for triangle position
    const bufferDescriptor_Positions_Triangle = 
    {
        size: pyramidVertices.byteLength,
        usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST
    };

    buffer_Positions_Pyramid = Device.createBuffer(bufferDescriptor_Positions_Triangle);

    if(buffer_Positions_Pyramid == null)
    {
        console.log("initialise() : buffer_Positions_Pyramid is not supported by your browser.\n");
        throw Error("initialise() : buffer_Positions_Pyramid is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Positions_Pyramid is supported by your browser.\n");
    }

    Queue.writeBuffer(buffer_Positions_Pyramid, 0, pyramidVertices, 0, pyramidVertices.length);

    console.log("initialise() : buffer_Positions_Pyramid is successfully created and data is copied to it.\n");

    const mvpUniformSize = 16 * 4; // 4x4 matrix of float32

    // mvp uniform buffer for triangle
    buffer_MVPUniform_Pyramid = createUniformBuffer(mvpUniformSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    // bind group for triangle
    bindingGroup_MVPUniform_Pyramid = createBindGroupForUniform(buffer_MVPUniform_Pyramid, 0, mvpUniformSize,0, bindGroupLayout_mvpUniform);


    // Rectangle : its postion buffer , its uniform buffer, its bind group

    // position buffer

    // create vertex buffer for triangle position
    const bufferDescriptor_Positions_Rectangle = 
    {
        size: cubeVertices.byteLength,
        usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST
    };

    buffer_Positions_Cube = Device.createBuffer(bufferDescriptor_Positions_Rectangle);

    if(buffer_Positions_Cube == null)
    {
        console.log("initialise() : buffer_Positions_Cube is not supported by your browser.\n");
        throw Error("initialise() : buffer_Positions_Cube is not supported by your browser.\n");
    }
    else
    {
        console.log("initialise() : buffer_Positions_Cube is supported by your browser.\n");
    }

    Queue.writeBuffer(buffer_Positions_Cube, 0, cubeVertices, 0, cubeVertices.length);

    console.log("initialise() : buffer_Positions_Cube is successfully created and data is copied to it.\n");

    // mvp uniform buffer for rectangle
    buffer_MVPUniform_Cube = createUniformBuffer(mvpUniformSize, GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST);

    // bind group for rectangle
    bindingGroup_MVPUniform_Cube = createBindGroupForUniform(buffer_MVPUniform_Cube, 0, mvpUniformSize,0, bindGroupLayout_mvpUniform);
    
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

    const vertexShaderState =
    {
        module: shaderModule_VertexShader,
        entryPoint: "main",
        buffers: [positionVetexBufferLayout]
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


    // define the clearcolor
    clearColor = { r: 0.0, g: 0.0, b: 1.0, a: 1.0 };

}

// user define functions

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

    // Pyramid

    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();
    mat4.translate(modelViewMatrix, modelViewMatrix, [-2.0, 0.0, -6.0]);
    mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angle_Pyramid));
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    Queue.writeBuffer(buffer_MVPUniform_Pyramid, 0, modelViewProjectionMatrix, 0, modelViewProjectionMatrix.length);

    renderPassEncoder.setVertexBuffer(0, buffer_Positions_Pyramid);
    renderPassEncoder.setBindGroup(0, bindingGroup_MVPUniform_Pyramid);
    renderPassEncoder.draw(12);

    // Cube

    modelViewMatrix = mat4.create();
    modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, -7.0]);

    mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(angle_Cube));
    mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angle_Cube));
    mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(angle_Cube));

    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    Queue.writeBuffer(buffer_MVPUniform_Cube, 0, modelViewProjectionMatrix, 0, modelViewProjectionMatrix.length);

    renderPassEncoder.setVertexBuffer(0, buffer_Positions_Cube);
    renderPassEncoder.setBindGroup(0, bindingGroup_MVPUniform_Cube);
    renderPassEncoder.draw(36);


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
        buffer_Positions_Cube = null;

        renderPipeline = null;
        buffer_MVPUniform_Pyramid = null;
        buffer_MVPUniform_Cube = null;
        bindingGroup_MVPUniform_Pyramid = null;
        bindingGroup_MVPUniform_Cube = null;
    }

    perspectiveProjectionMatrix = null;

    console.log("uninitialise() : WebGPUDevice is destroyed successfully.\n");

}

