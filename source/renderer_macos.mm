


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

// ------------------------------------------------------------
// Simple triangle data
// ------------------------------------------------------------

struct Vertex
{
    float pos[2];
    float color[3];
};

static Vertex g_triangle[3] =
{
    { {  0.0f,  0.5f }, { 1, 0, 0 } },
    { { -0.5f, -0.5f }, { 0, 1, 0 } },
    { {  0.5f, -0.5f }, { 0, 0, 1 } },
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
                       newBufferWithBytes:g_triangle
                       length:sizeof(g_triangle)
                       options:MTLResourceStorageModeShared];
    
    // Compile shaders
    NSError* error = nil;
    
    NSString* source = @R"(
        #include <metal_stdlib>
        using namespace metal;

        struct VertexIn {
            float2 pos [[attribute(0)]];
            float3 color [[attribute(1)]];
        };

        struct VertexOut {
            float4 position [[position]];
            float3 color;
        };

        vertex VertexOut vs_main(VertexIn in [[stage_in]])
        {
            VertexOut out;
            out.position = float4(in.pos, 0.0, 1.0);
            out.color = in.color;
            return out;
        }

        fragment float4 fs_main(VertexOut in [[stage_in]])
        {
            return float4(in.color, 1.0);
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
    vertexDesc.attributes[0].format = MTLVertexFormatFloat2;
    vertexDesc.attributes[0].offset = 0;
    vertexDesc.attributes[0].bufferIndex = 0;
    
    // Attribute 1 → color
    vertexDesc.attributes[1].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[1].offset = sizeof(float) * 2;
    vertexDesc.attributes[1].bufferIndex = 0;
    
    // Buffer layout
    vertexDesc.layouts[0].stride = sizeof(Vertex);
    vertexDesc.layouts[0].stepRate = 1;
    vertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    
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
    
    MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
    pass.colorAttachments[0].texture = drawable.texture;
    pass.colorAttachments[0].loadAction  = MTLLoadActionClear;
    pass.colorAttachments[0].storeAction = MTLStoreActionStore;
    pass.colorAttachments[0].clearColor  = MTLClearColorMake(0.1, 0.1, 0.1, 1.0);
    
    id<MTLCommandBuffer> cmd = [g_command_queue commandBuffer];
    id<MTLRenderCommandEncoder> enc =
        [cmd renderCommandEncoderWithDescriptor:pass];
    
    [enc setRenderPipelineState:g_pipeline];
    [enc setVertexBuffer:g_vertex_buffer offset:0 atIndex:0];
    [enc drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
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
