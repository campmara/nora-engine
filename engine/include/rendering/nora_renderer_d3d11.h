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
        RendererD3D11(HWND *hwnd) : RendererBase()
        {
            windowHandle = hwnd;
        }
        virtual ~RendererD3D11() {}
        RendererD3D11(const RendererD3D11 &) = delete;
        RendererD3D11 &operator=(const RendererD3D11 &) = delete;

        struct RenderConstants
        {
            Matrix4x4 transform;
            Matrix4x4 projection;
            Vector3 lightDirection;
        };

        void Initialize();
        void ProcessFrame();
        void Shutdown();

    private:
        HWND *windowHandle = nullptr;

        // D3D11 Resources
        ID3D11DeviceContext *deviceContext = nullptr;
        IDXGISwapChain *swapchain = nullptr;
        ID3D11Texture2D *framebuffer = nullptr;
        ID3D11RenderTargetView *framebufferRTV = nullptr;
        ID3D11Texture2D *depthBuffer = nullptr;
        ID3D11DepthStencilView *depthBufferDSV = nullptr;
        ID3D11VertexShader *vertexShader = nullptr;
        ID3D11PixelShader *pixelShader = nullptr;
        ID3D11InputLayout *inputLayout = nullptr;
        ID3D11RasterizerState *rasterizerState = nullptr;
        ID3D11SamplerState *samplerState = nullptr;
        ID3D11DepthStencilState *depthStencilState = nullptr;
        ID3D11Buffer *constantBuffer = nullptr;
        ID3D11Texture2D *texture = nullptr;
        ID3D11ShaderResourceView *textureSRV = nullptr;
        ID3D11Buffer *vertexBuffer = nullptr;
        UINT stride = 11 * sizeof(float); // vertex size (11 floats: float3 position, float3
                                          // normal, float2 texcoord, float3 color)
        UINT offset = 0;
        ID3D11Buffer *indexBuffer = nullptr;

        // Viewport Properties
        D3D11_VIEWPORT viewport = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
        FLOAT clearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
        float viewportWidth = 0.0f;    // width (aspect ratio)
        float viewportHeight = 1.0f;   // height
        float viewportNearClip = 1.0f; // near clipping plane
        float viewportFarClip = 9.0f;  // far clipping plane

        // Model Transform
        Vector3 modelTranslation = { 0.0f, 0.0f, 4.0f };
        Vector3 modelRotation = { 0.0f, 0.0f, 0.0f };
        Vector3 modelScale = { 1.0f, 1.0f, 1.0f };
    };
} // namespace nora

#endif