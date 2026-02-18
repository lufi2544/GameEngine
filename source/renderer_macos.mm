#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

// ------------------------------------------------------------
// Data Structures matching Windows Renderer
// ------------------------------------------------------------

struct gpu_vertex_t {
    f32 x, y, z;
    f32 r, g, b, a;
    f32 u, v;
};

struct gpu_mesh_t {
    id<MTLBuffer> vertex_buffer;
    id<MTLBuffer> index_buffer;
    id<MTLTexture> texture_srv;
    u32 vertex_count;
    u32 indexes_count;
};

struct constant_buffer_mvp {
    mat4_t mvp;
};

struct renderer_macos_t {
    id<MTLDevice> device;
    id<MTLTexture> depthTexture;
    id<MTLCommandQueue> command_queue;
    CAMetalLayer* metal_layer;
    id<MTLRenderPipelineState> pipeline;
    id<MTLDepthStencilState> depth_state;
    list_t gpu_meshes; 
};

static renderer_macos_t g_renderer;
static NSString* shader_source = @R"(
    #include <metal_stdlib>
    using namespace metal;

    // Matching gpu_vertex_t layout
    struct VertexIn {
        float3 pos   [[attribute(0)]];
        float4 color [[attribute(1)]];
        float2 uv    [[attribute(2)]];
    };

    struct VertexOut {
        float4 position [[position]];
        float4 color;
        float2 uv;
    };

    struct SceneUniforms {
        float4x4 mvp;
    };

    // Matches constant_buffer_debug
    struct DebugUniforms {
        float4 debugColor;
        int useDebugColor;
        float3 padding; 
    };

    // --- Vertex Shader ---
    vertex VertexOut vs_main(VertexIn in [[stage_in]], 
                            constant SceneUniforms &u [[buffer(1)]]) 
    {
        VertexOut out;
        // Transform local space to clip space using MVP
        out.position = u.mvp * float4(in.pos, 1.0);
        out.color = in.color;
        out.uv = in.uv;
        return out;
    }

    // --- Fragment Shader ---
    fragment float4 fs_main(VertexOut in [[stage_in]],
                            texture2d<float> tex [[texture(0)]],
                            sampler samp [[sampler(0)]]) 
    {

        // Standard pass: combine texture and vertex color
        float4 texColor = tex.sample(samp, in.uv);
        return texColor * in.color;
    }
)";

// ------------------------------------------------------------
// Metal Implementation
// ------------------------------------------------------------

bool Metal_Init() {
    g_renderer.device = MTLCreateSystemDefaultDevice();
    if(!g_renderer.device) return false;
    
    g_renderer.command_queue = [g_renderer.device newCommandQueue];
    
    NSWindow* window = (__bridge NSWindow*)g_engine->main_window.handle;
    g_renderer.metal_layer = [CAMetalLayer layer];
    g_renderer.metal_layer.device = g_renderer.device;
    g_renderer.metal_layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    window.contentView.wantsLayer = YES;
    window.contentView.layer = g_renderer.metal_layer;
    
    NSError* error = nil;
    id<MTLLibrary> library = [g_renderer.device newLibraryWithSource:shader_source options:nil error:&error];
    if(!library) {
        NSLog(@"Shader Error: %@", error);
        return false;
    }
    
    MTLRenderPipelineDescriptor* desc = [[MTLRenderPipelineDescriptor alloc] init];
    desc.vertexFunction = [library newFunctionWithName:@"vs_main"];
    desc.fragmentFunction = [library newFunctionWithName:@"fs_main"];
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    MTLVertexDescriptor* vDesc = [[MTLVertexDescriptor alloc] init];
    // Position: float3 (x,y,z)
    vDesc.attributes[0].format = MTLVertexFormatFloat3;
    vDesc.attributes[0].offset = 0;
    vDesc.attributes[0].bufferIndex = 0;
    
    // Color: float4 (r,g,b,a)
    vDesc.attributes[1].format = MTLVertexFormatFloat4;
    vDesc.attributes[1].offset = sizeof(float) * 3;
    vDesc.attributes[1].bufferIndex = 0;
    
    // UV: float2 (u,v)
    vDesc.attributes[2].format = MTLVertexFormatFloat2;
    vDesc.attributes[2].offset = sizeof(float) * 7;
    vDesc.attributes[2].bufferIndex = 0;
    
    vDesc.layouts[0].stride = sizeof(gpu_vertex_t);
    desc.vertexDescriptor = vDesc;
    
    g_renderer.pipeline = [g_renderer.device newRenderPipelineStateWithDescriptor:desc error:&error];
    
    MTLTextureDescriptor* depthDesc =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
         width:g_engine->main_window.width
         height:g_engine->main_window.height
         mipmapped:NO];
    depthDesc.usage = MTLTextureUsageRenderTarget;
    
    id<MTLTexture> depthTexture =
        [g_renderer.device newTextureWithDescriptor:depthDesc];
    
    g_renderer.depthTexture = depthTexture;
    {
        MTLDepthStencilDescriptor* depthDesc = [[MTLDepthStencilDescriptor alloc] init];
        depthDesc.depthCompareFunction = MTLCompareFunctionLess;
        depthDesc.depthWriteEnabled = YES;
        
        g_renderer.depth_state =
            [g_renderer.device newDepthStencilStateWithDescriptor:depthDesc];
    }
    
    return (g_renderer.pipeline != nil);
}

void RenderMeshes() 
{
    id<CAMetalDrawable> drawable = [g_renderer.metal_layer nextDrawable];
    if(!drawable) return;
    
    // Setup Camera Logic
    mat4_t view, proj, projView;
    Mat4LookAtLH(&view, g_engine_camera->position, g_engine_camera->target, g_engine_camera->up);
    
    float aspect = (f32)g_engine->main_window.width / (f32)g_engine->main_window.height;
    Mat4PerspectiveLH(&proj, g_engine_camera->fov, aspect, g_engine_camera->near_z, g_engine_camera->far_z);
    Mat4Mul(&projView, &proj, &view);
    
    MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
    pass.colorAttachments[0].texture = drawable.texture;
    pass.colorAttachments[0].loadAction = MTLLoadActionClear;
    pass.colorAttachments[0].clearColor = MTLClearColorMake(0.1, 0.2, 0.4, 1.0);
    
    pass.depthAttachment.texture = g_renderer.depthTexture;
    pass.depthAttachment.loadAction = MTLLoadActionClear;
    pass.depthAttachment.storeAction = MTLStoreActionDontCare;
    pass.depthAttachment.clearDepth = 1.0;
    
    id<MTLCommandBuffer> cmd = [g_renderer.command_queue commandBuffer];
    id<MTLRenderCommandEncoder> enc = [cmd renderCommandEncoderWithDescriptor:pass];
    [enc setRenderPipelineState:g_renderer.pipeline];
    
    [enc setDepthStencilState:g_renderer.depth_state];
    [enc setCullMode:MTLCullModeBack];
    [enc setFrontFacingWinding:MTLWindingClockwise];
    
    gpu_mesh_t* mesh = 0;
    LIST_FOREACH(gpu_mesh_t, mesh, g_renderer.gpu_meshes) {
        transform_t transform = {};
        transform.position = {0, 0, 1/1000};
        transform.scale = {1, 1, 1};
        static float angle = 0.0f;
        angle += 0.01f;
        transform.rotation = {0, angle, 0}; // Match rotation animation
        
        mat4_t world, mvp;
        TransformToMatrix(&world, transform);
        Mat4Mul(&mvp, &projView, &world);
        
        constant_buffer_mvp cb;
        cb.mvp = mvp;
        
        [enc setVertexBuffer:mesh->vertex_buffer offset:0 atIndex:0];
        [enc setVertexBytes:&cb length:sizeof(cb) atIndex:1];
        
        // SOLID PASS
        [enc setTriangleFillMode:MTLTriangleFillModeFill];
        [enc setFragmentTexture:mesh->texture_srv atIndex:0];
        [enc drawIndexedPrimitives:MTLPrimitiveTypeTriangle 
         indexCount:mesh->indexes_count 
         indexType:MTLIndexTypeUInt32 
         indexBuffer:mesh->index_buffer 
         indexBufferOffset:0];
        
    }
    
    [enc endEncoding];
    [cmd presentDrawable:drawable];
    [cmd commit];
}

// ------------------------------------------------------------
// Exported Engine Functions
// ------------------------------------------------------------

global_f bool RendererInit() { 
    return Metal_Init(); 
}

global_f void RendererUpdate(engine_shared_data_t *engine_data) {
    RenderMeshes();
}



internal_f u32
hash_function_gpu_vertex(void *key, u32 key_size)
{
	gpu_vertex_t *vertex = (gpu_vertex_t*)key;	
	return vertex->u + vertex->v  + vertex->x + vertex->y + vertex->z;
}

global_f void 
RendererComputeImportedMesh(engine_shared_data_t *engine_data, mesh_t *asset, const char* _texture_name) {
    S_SCRATCH(engine_data->memory);	
    
    u32 asset_max_verteces = asset->face_num * 3;
    gpu_mesh_t result = {};		
    
    // Hash map for vertex de-duplication (matching Windows logic)
    hash_map_t vertex_to_index_map = HASH_MAP(temp_arena, asset_max_verteces, gpu_vertex_t, u32, hash_function_gpu_vertex);
    
    gpu_vertex_t *temp_verteces = push_array(temp_arena, asset_max_verteces, gpu_vertex_t);
    u32 unique_verteces = 0;
    
    u32 *index_buffer_data = push_array(temp_arena, asset_max_verteces, u32);
    u32 indexes_num = 0;
    
    for(u32 face_idx = 0; face_idx < asset->face_num; ++face_idx) {
        face_t *face = &asset->faces[face_idx];
        
        // Process each of the 3 vertices in the face (A, B, C)
        u32 face_indices[3] = { (u32)face->a, (u32)face->b, (u32)face->c };
        texture_uv_t face_uvs[3] = { face->a_uv, face->b_uv, face->c_uv };
        
        for(int i = 0; i < 3; ++i) {
            vec3_t v_pos = asset->verteces[face_indices[i]];
            gpu_vertex_t gv = {};
            gv.x = v_pos.x; gv.y = v_pos.y; gv.z = v_pos.z;
            gv.u = face_uvs[i].u; gv.v = face_uvs[i].v;
            gv.r = 1.0f; gv.g = 1.0f; gv.b = 1.0f; gv.a = 1.0f; // Default vertex color
            
            u32 *found_idx = (u32*)hash_map_find(&vertex_to_index_map, &gv, sizeof(gpu_vertex_t));
            if(!found_idx) {
                temp_verteces[unique_verteces] = gv;
                HASH_MAP_ADD(vertex_to_index_map, gpu_vertex_t, u32, gv, unique_verteces);
                index_buffer_data[indexes_num++] = unique_verteces;
                unique_verteces++;
            } else {
                index_buffer_data[indexes_num++] = *found_idx;
            }
        }
    }
    
    // Create Metal Vertex Buffer
    result.vertex_buffer = [g_renderer.device newBufferWithBytes:temp_verteces 
                            length:unique_verteces * sizeof(gpu_vertex_t) 
                            options:MTLResourceStorageModeShared];
    
    // Create Metal Index Buffer
    result.index_buffer = [g_renderer.device newBufferWithBytes:index_buffer_data 
                           length:indexes_num * sizeof(u32) 
                           options:MTLResourceStorageModeShared];
    
    result.vertex_count = unique_verteces;
    result.indexes_count = indexes_num;
    
    // Texture Loading (matching Windows logic using upng)
    upng_t *png = upng_new_from_file(&engine_data->memory->permanent, _texture_name);
    if(png) {
        upng_decode(engine_data, png);
        u32 width = upng_get_width(png);
        u32 height = upng_get_height(png);
        void* pixels = (void*)upng_get_buffer(png);
        
        MTLTextureDescriptor *texDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm 
                                         width:width 
                                         height:height 
                                         mipmapped:NO];
        result.texture_srv = [g_renderer.device newTextureWithDescriptor:texDesc];
        
        MTLRegion region = MTLRegionMake2D(0, 0, width, height);
        [result.texture_srv replaceRegion:region 
         mipmapLevel:0 
         withBytes:pixels 
         bytesPerRow:width * 4];
    }
    
    // Add to global list for rendering
    LIST_ADD(&engine_data->memory->permanent, g_renderer.gpu_meshes, result, gpu_mesh_t);
}
