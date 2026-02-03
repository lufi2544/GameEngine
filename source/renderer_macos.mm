


#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

// ------------------------------------------------------------
// Metal state
// ------------------------------------------------------------

static id<MTLDevice>         g_device;
static id<MTLCommandQueue>  g_command_queue;
static CAMetalLayer*        g_metal_layer;

static id<MTLRenderPipelineState> g_pipeline;
static id<MTLBuffer>              g_vertex_buffer;
static id<MTLBuffer> g_index_buffer;

// ------------------------------------------------------------
// Simple triangle data
// ------------------------------------------------------------

struct Vertex
{
    float pos[3];
    float color[4];
};

static Vertex g_triangle[3] =
{
    { {  0.0f,  0.5f }, { 1, 0, 0 } },
    { { -0.5f, -0.5f }, { 0, 1, 0 } },
    { {  0.5f, -0.5f }, { 0, 0, 1 } },
};


struct Uniforms {
    mat4_t mvp;
};




// Simple cube vertices
static Vertex g_cube_vertices[] = {
    {{-0.5, -0.5,  0.5}, {1, 0, 0, 1}}, {{ 0.5, -0.5,  0.5}, {0, 1, 0, 1}},
    {{ 0.5,  0.5,  0.5}, {0, 0, 1, 1}}, {{-0.5,  0.5,  0.5}, {1, 1, 1, 1}},
    {{-0.5, -0.5, -0.5}, {1, 0, 0, 1}}, {{ 0.5, -0.5, -0.5}, {0, 1, 0, 1}},
    {{ 0.5,  0.5, -0.5}, {0, 0, 1, 1}}, {{-0.5,  0.5, -0.5}, {1, 1, 1, 1}}
};

static uint16_t g_cube_indices[] = {
    0, 1, 2, 2, 3, 0, // Front
    1, 5, 6, 6, 2, 1, // Right
    7, 6, 5, 5, 4, 7, // Back
    4, 0, 3, 3, 7, 4, // Left
    4, 5, 1, 1, 0, 4, // Bottom
    3, 2, 6, 6, 7, 3  // Top
};



// ------------------------------------------------------------
// Metal init
// ------------------------------------------------------------

bool Metal_Init()
{
    g_device = MTLCreateSystemDefaultDevice();
    if(!g_device) return false;
    
    g_command_queue = [g_device newCommandQueue];
    
    // Attach CAMetalLayer to your existing NSWindow
    NSWindow* window = (__bridge NSWindow*)g_engine->main_window.handle;
    
    g_metal_layer = [CAMetalLayer layer];
    g_metal_layer.device = g_device;
    g_metal_layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    g_metal_layer.framebufferOnly = YES;
    
    window.contentView.wantsLayer = YES;
    window.contentView.layer = g_metal_layer;
    
    // Create vertex buffer
    g_vertex_buffer = [g_device
                       newBufferWithBytes:g_cube_vertices
                       length:sizeof(g_cube_vertices)
                       options:MTLResourceStorageModeShared];
    
    g_index_buffer  = [g_device newBufferWithBytes:g_cube_indices 
                       length:sizeof(g_cube_indices) 
                       options:MTLResourceStorageModeShared];
    
    
    
    
    
    // Compile shaders
    NSError* error = nil;
    NSString* source = @R"(
    #include <metal_stdlib>
    using namespace metal;

    struct VertexIn {
        float3 pos [[attribute(0)]];
        float4 color [[attribute(1)]];
    };

    struct VertexOut {
        float4 position [[position]];
        float4 color;
    };

    struct Uniforms {
        float4x4 mvp;
    };

    vertex VertexOut vs_main(VertexIn in [[stage_in]], constant Uniforms &u [[buffer(1)]]) {
        VertexOut out;
        out.position = u.mvp * float4(in.pos, 1.0);
        out.color = in.color;
        return out;
    }

    fragment float4 fs_main(VertexOut in [[stage_in]]) {
        return in.color;
    }
)";
    
    id<MTLLibrary> library =
        [g_device newLibraryWithSource:source options:nil error:&error];
    
    if(!library)
    {
        NSLog(@"Metal shader error: %@", error);
        return false;
    }
    
    MTLRenderPipelineDescriptor* desc =
        [[MTLRenderPipelineDescriptor alloc] init];
    
    desc.vertexFunction   = [library newFunctionWithName:@"vs_main"];
    desc.fragmentFunction = [library newFunctionWithName:@"fs_main"];
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    MTLVertexDescriptor* vertexDesc = [[MTLVertexDescriptor alloc] init];
    
    // Attribute 0 → position
    vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[0].offset = 0;
    vertexDesc.attributes[0].bufferIndex = 0;
    
    // Attribute 1 → color
    vertexDesc.attributes[1].format = MTLVertexFormatFloat4;
    vertexDesc.attributes[1].offset = sizeof(float) * 2;
    vertexDesc.attributes[1].bufferIndex = 0;
    
    // Buffer layout
    vertexDesc.layouts[0].stride = sizeof(Vertex);
    
    desc.vertexDescriptor = vertexDesc;
    
    
    g_pipeline = [g_device newRenderPipelineStateWithDescriptor:desc error:&error];
    if(!g_pipeline)
    {
        NSLog(@"Pipeline error: %@", error);
        return false;
    }
    
    return true;
}

void Metal_BeginFrame() {}


void Metal_DrawTriangle() 
{
    id<CAMetalDrawable> drawable = [g_metal_layer nextDrawable];
    if(!drawable) return;
    
    // 1. Math using your library
    static float angle = 0.0f;
    angle += 0.01f;
    
    mat4_t view, proj, model, projView, mvp;
    
    // Setup Camera (Matching your Windows logic)
    Mat4LookAtLH(&view, {0, 0, -5}, {0, 0, 0}, {0, 1, 0});
    
    float aspect = (float)g_engine->main_window.width / (float)g_engine->main_window.height;
    Mat4PerspectiveLH(&proj, 0.785f, aspect, 0.1f, 100.0f);
    
    // Setup Transform
    transform_t t = {};
    t.position = {0, 0, 0};
    t.rotation = {angle, angle, 0};
    t.scale    = {1, 1, 1};
    TransformToMatrix(&model, t);
    
    // Combine: MVP = Proj * View * Model
    Mat4Mul(&projView, &proj, &view);
    Mat4Mul(&mvp, &projView, &model);
    
    Uniforms u;
    u.mvp = mvp;
    
    // 2. Command Encoding
    MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
    pass.colorAttachments[0].texture = drawable.texture;
    pass.colorAttachments[0].loadAction = MTLLoadActionClear;
    pass.colorAttachments[0].clearColor = MTLClearColorMake(0.1, 0.2, 0.4, 1.0);
    
    id<MTLCommandBuffer> cmd = [g_command_queue commandBuffer];
    id<MTLRenderCommandEncoder> enc = [cmd renderCommandEncoderWithDescriptor:pass];
    
    [enc setRenderPipelineState:g_pipeline];
    
    // Wireframe mode
    [enc setTriangleFillMode:MTLTriangleFillModeLines];
    
    [enc setVertexBuffer:g_vertex_buffer offset:0 atIndex:0];
    [enc setVertexBytes:&u length:sizeof(u) atIndex:1];
    
    [enc drawIndexedPrimitives:MTLPrimitiveTypeTriangle 
     indexCount:36 
     indexType:MTLIndexTypeUInt16 
     indexBuffer:g_index_buffer 
     indexBufferOffset:0];
    
    [enc endEncoding];
    [cmd presentDrawable:drawable];
    [cmd commit];
}

void Metal_EndFrame() {}

global_f bool
RendererInit()
{ 
    Metal_Init();
    
    return true;
}

global_f void
RendererUpdate(engine_shared_data_t *engine_data)
{
    Metal_BeginFrame();
    Metal_DrawTriangle();
    Metal_EndFrame();
}

global_f void 
RendererComputeImportedMesh(engine_shared_data_t *engine_data, mesh_t *_mesh, const char* _texture_name){}
