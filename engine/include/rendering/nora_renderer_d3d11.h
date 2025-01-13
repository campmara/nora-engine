#ifndef NORA_RENDERER_D3D11_H
#define NORA_RENDERER_D3D11_H

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "nora_renderer.h"
#include "math/nora_math.h"

namespace nora
{
    struct RendererD3D11 final : public RendererBase
    {
        RendererD3D11(HWND *window) : RendererBase()
        {
            window_handle = window;
        }
        virtual ~RendererD3D11() {}
        RendererD3D11(const RendererD3D11 &) = delete;
        RendererD3D11 &operator=(const RendererD3D11 &) = delete;

        struct RenderConstants
        {
            Matrix4x4 transform;
            Matrix4x4 projection;
            Vector3 light_direction;
        };

        void Initialize();
        void ProcessFrame();
        void Shutdown();

    private:
        HWND *window_handle = nullptr;

        // D3D11 Resources
        ID3D11DeviceContext *device_context = nullptr;
        IDXGISwapChain *swapchain = nullptr;
        ID3D11Texture2D *framebuffer = nullptr;
        ID3D11RenderTargetView *framebuffer_render_target_view = nullptr;
        ID3D11Texture2D *depth_buffer = nullptr;
        ID3D11DepthStencilView *depth_buffer_depth_stencil_view = nullptr;
        ID3D11VertexShader *vertex_shader = nullptr;
        ID3D11PixelShader *pixel_shader = nullptr;
        ID3D11InputLayout *input_layout = nullptr;
        ID3D11RasterizerState *rasterizer_state = nullptr;
        ID3D11SamplerState *sampler_state = nullptr;
        ID3D11DepthStencilState *depth_stencil_state = nullptr;
        ID3D11Buffer *constant_buffer = nullptr;
        ID3D11Texture2D *texture = nullptr;
        ID3D11ShaderResourceView *texture_shader_resource_view = nullptr;
        ID3D11Buffer *vertex_buffer = nullptr;
        UINT stride = 11 * sizeof(float); // vertex size (11 floats: float3 position, float3
                                          // normal, float2 texcoord, float3 color)
        UINT offset = 0;
        ID3D11Buffer *index_buffer = nullptr;

        // Viewport Properties
        D3D11_VIEWPORT viewport = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
        FLOAT clear_color[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
        float viewport_width = 0.0f;    // width (aspect ratio)
        float viewport_height = 1.0f;   // height
        float viewport_near_clip = 1.0f; // near clipping plane
        float viewport_far_clip = 9.0f;  // far clipping plane

        // Model Transform
        Vector3 model_translation = { 0.0f, 0.0f, 4.0f };
        Vector3 model_rotation = { 0.0f, 0.0f, 0.0f };
        Vector3 model_scale = { 1.0f, 1.0f, 1.0f };
    };
} // namespace nora

#endif