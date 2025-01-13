#include "rendering/nora_renderer_d3d11.h"
#include "rendering/testcube.h"

namespace nora
{
    void RendererD3D11::Initialize()
    {
        if (window_handle == nullptr)
        {
            return;
        }

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // DEVICE AND SWAPCHAIN CREATION
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };

        DXGI_SWAP_CHAIN_DESC swapchain_desc = {};
        swapchain_desc.BufferDesc.Width = 0;  // Use window width, will get later
        swapchain_desc.BufferDesc.Height = 0; // Use window height, will get later
        swapchain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.BufferCount = 2;
        swapchain_desc.OutputWindow = *window_handle;
        swapchain_desc.Windowed = TRUE;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        ID3D11Device *device;

        const UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                                      feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION,
                                      &swapchain_desc, &swapchain, &device, nullptr,
                                      &device_context);

        // This updates the swapchainDesc with the actual HWND window size
        swapchain->GetDesc(&swapchain_desc);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // FRAMEBUFFER AND RENDER TARGET
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        // Grab framebuffer from swapchain
        swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&framebuffer);

        // Needed for SRGB framebuffer when using FLIP model swap effect
        D3D11_RENDER_TARGET_VIEW_DESC framebuffer_RTV_desc = {};
        framebuffer_RTV_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        framebuffer_RTV_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        device->CreateRenderTargetView(framebuffer, &framebuffer_RTV_desc, &framebuffer_render_target_view);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // DEPTH AND STENCIL BUFFERS
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_TEXTURE2D_DESC depth_buffer_desc;

        // Copy framebuffer properties, they are mostly the same
        framebuffer->GetDesc(&depth_buffer_desc);

        depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        device->CreateTexture2D(&depth_buffer_desc, nullptr, &depth_buffer);

        device->CreateDepthStencilView(depth_buffer, nullptr, &depth_buffer_depth_stencil_view);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // CREATE VERTEX SHADER
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        ID3DBlob *vertex_shader_cso;

        D3DCompileFromFile(L"C:\\work\\cpp\\nora\\engine\\engine\\shaders\\test.hlsl", nullptr,
                           nullptr, "VertexShaderMain", "vs_5_0", 0, 0, &vertex_shader_cso,
                           nullptr);

        device->CreateVertexShader(vertex_shader_cso->GetBufferPointer(),
                                   vertex_shader_cso->GetBufferSize(), nullptr, &vertex_shader);

        D3D11_INPUT_ELEMENT_DESC input_element_desc[] = {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
             D3D11_INPUT_PER_VERTEX_DATA,                                                                          0 },
            { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,
             D3D11_INPUT_PER_VERTEX_DATA,                                                                          0 },
            { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
             D3D11_INPUT_PER_VERTEX_DATA,                                                                          0 },
        };

        device->CreateInputLayout(input_element_desc, ARRAYSIZE(input_element_desc),
                                  vertex_shader_cso->GetBufferPointer(),
                                  vertex_shader_cso->GetBufferSize(), &input_layout);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // CREATE PIXEL SHADER
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        ID3DBlob *pixel_shader_cso;

        D3DCompileFromFile(L"C:\\work\\cpp\\nora\\engine\\engine\\shaders\\test.hlsl", nullptr,
                           nullptr, "PixelShaderMain", "ps_5_0", 0, 0, &pixel_shader_cso, nullptr);

        device->CreatePixelShader(pixel_shader_cso->GetBufferPointer(),
                                  pixel_shader_cso->GetBufferSize(), nullptr, &pixel_shader);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // RASTERIZER STATE
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_RASTERIZER_DESC rasterizer_desc = {};
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_BACK;

        device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // SAMPLER STATE
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_SAMPLER_DESC sampler_desc = {};
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        device->CreateSamplerState(&sampler_desc, &sampler_state);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // DEPTH STENCIL STATE
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
        depth_stencil_desc.DepthEnable = TRUE;
        depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

        device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // CONSTANT BUFFER SETUP
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_BUFFER_DESC constant_buffer_desc = {};
        constant_buffer_desc.ByteWidth =
            sizeof(RenderConstants) + 0xf &
            0xfffffff0; // ensure constant buffer size is multiple of 16 bytes
        constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
        constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        device->CreateBuffer(&constant_buffer_desc, nullptr, &constant_buffer);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // TEXTURE DESCRIPTION AND SHADER RESOURCE VIEW
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_TEXTURE2D_DESC texture_desc = {};
        texture_desc.Width = TEST_CUBE_TEXTURE_WIDTH;
        texture_desc.Height = TEST_CUBE_TEXTURE_HEIGHT;
        texture_desc.MipLevels = 1;
        texture_desc.ArraySize = 1;
        texture_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // same as framebuffer(view)
        texture_desc.SampleDesc.Count = 1;
        texture_desc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated
        texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA texture_subresource_data = {};
        texture_subresource_data.pSysMem = testCubeTextureData;
        texture_subresource_data.SysMemPitch = TEST_CUBE_TEXTURE_WIDTH * sizeof(UINT);

        device->CreateTexture2D(&texture_desc, &texture_subresource_data, &texture);

        device->CreateShaderResourceView(texture, nullptr, &texture_shader_resource_view);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // VERTEX BUFFER
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_BUFFER_DESC vertex_buffer_desc = {};
        vertex_buffer_desc.ByteWidth = sizeof(test_cube_vertex_data);
        vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated
        vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertex_buffer_subresource_data = { test_cube_vertex_data };

        device->CreateBuffer(&vertex_buffer_desc, &vertex_buffer_subresource_data, &vertex_buffer);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // INDEX BUFFER
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_BUFFER_DESC index_buffer_desc = {};
        index_buffer_desc.ByteWidth = sizeof(testCubeIndexData);
        index_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated
        index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA index_buffer_subresource_data = { testCubeIndexData };

        device->CreateBuffer(&index_buffer_desc, &index_buffer_subresource_data, &index_buffer);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // VIEWPORT
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        viewport = { 0.0f,
                     0.0f,
                     (float)swapchain_desc.BufferDesc.Width,
                     (float)swapchain_desc.BufferDesc.Height,
                     0.0f,
                     1.0f };

        viewport_width = viewport.Width / viewport.Height; // width (aspect ratio)
    }

    void RendererD3D11::ProcessFrame()
    {
        if (window_handle == nullptr)
        {
            return;
        }

        Matrix4x4 rotate_x = { 1,
                               0,
                               0,
                               0,
                               0,
                               (float)cos(model_rotation.x),
                               -(float)sin(model_rotation.x),
                               0,
                               0,
                               (float)sin(model_rotation.x),
                               (float)cos(model_rotation.x),
                               0,
                               0,
                               0,
                               0,
                               1 };
        Matrix4x4 rotate_y = {
            (float)cos(model_rotation.y),  0, (float)sin(model_rotation.y), 0, 0, 1, 0, 0,
            -(float)sin(model_rotation.y), 0, (float)cos(model_rotation.y), 0, 0, 0, 0, 1
        };
        Matrix4x4 rotate_z = { (float)cos(model_rotation.z),
                               -(float)sin(model_rotation.z),
                               0,
                               0,
                               (float)sin(model_rotation.z),
                               (float)cos(model_rotation.z),
                               0,
                               0,
                               0,
                               0,
                               1,
                               0,
                               0,
                               0,
                               0,
                               1 };
        Matrix4x4 scale = {
            model_scale.x, 0, 0, 0, 0, model_scale.y, 0, 0, 0, 0, model_scale.z, 0, 0, 0, 0, 1
        };
        Matrix4x4 translate = { 1,
                                0,
                                0,
                                0,
                                0,
                                1,
                                0,
                                0,
                                0,
                                0,
                                1,
                                0,
                                model_translation.x,
                                model_translation.y,
                                model_translation.z,
                                1 };

        model_rotation.x += 0.005f;
        model_rotation.y += 0.009f;
        model_rotation.z += 0.001f;

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // MAP DEVICE CONTEXT
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_MAPPED_SUBRESOURCE constant_buffer_mapped_subresource;

        // update constant buffer every frame
        device_context->Map(constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constant_buffer_mapped_subresource);
        {
            RenderConstants *constants = (RenderConstants *)constant_buffer_mapped_subresource.pData;
            constants->transform = rotate_x * rotate_y * rotate_z * scale * translate;
            constants->projection = { 2 * viewport_near_clip / viewport_width,
                                      0,
                                      0,
                                      0,
                                      0,
                                      2 * viewport_near_clip / viewport_height,
                                      0,
                                      0,
                                      0,
                                      0,
                                      viewport_far_clip / (viewport_far_clip - viewport_near_clip),
                                      1,
                                      0,
                                      0,
                                      viewport_near_clip * viewport_far_clip /
                                          (viewport_near_clip - viewport_far_clip),
                                      0 };
            constants->light_direction = { 1.0f, -1.0f, 1.0f };
        }
        device_context->Unmap(constant_buffer, 0);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // CLEAR AND SETUP FRAME
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        device_context->ClearRenderTargetView(framebuffer_render_target_view, clear_color);
        device_context->ClearDepthStencilView(depth_buffer_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

        device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        device_context->IASetInputLayout(input_layout);
        device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
        device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

        device_context->VSSetShader(vertex_shader, nullptr, 0);
        device_context->VSSetConstantBuffers(0, 1, &constant_buffer);

        device_context->RSSetViewports(1, &viewport);
        device_context->RSSetState(rasterizer_state);

        device_context->PSSetShader(pixel_shader, nullptr, 0);
        device_context->PSSetShaderResources(0, 1, &texture_shader_resource_view);
        device_context->PSSetSamplers(0, 1, &sampler_state);

        device_context->OMSetRenderTargets(1, &framebuffer_render_target_view, depth_buffer_depth_stencil_view);
        device_context->OMSetDepthStencilState(depth_stencil_state, 0);
        device_context->OMSetBlendState(nullptr, nullptr,
                                        0xffffffff); // use default blend mode (no blending)

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // DRAW FRAME AND PRESENT SWAPCHAIN
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        device_context->DrawIndexed(ARRAYSIZE(testCubeIndexData), 0, 0);
        swapchain->Present(1, 0);
    }

    void RendererD3D11::Shutdown()
    {
        index_buffer->Release();
        index_buffer = nullptr;

        vertex_buffer->Release();
        vertex_buffer = nullptr;
    }
} // namespace nora