#include "rendering/nora_renderer_d3d11.h"
#include "rendering/testcube.h"

namespace nora
{
    void RendererD3D11::Initialize()
    {
        if (windowHandle == nullptr)
        {
            return;
        }

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // DEVICE AND SWAPCHAIN CREATION
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

        DXGI_SWAP_CHAIN_DESC swapchainDesc = {};
        swapchainDesc.BufferDesc.Width = 0;  // Use window width, will get later
        swapchainDesc.BufferDesc.Height = 0; // Use window height, will get later
        swapchainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapchainDesc.SampleDesc.Count = 1;
        swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchainDesc.BufferCount = 2;
        swapchainDesc.OutputWindow = *windowHandle;
        swapchainDesc.Windowed = TRUE;
        swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        ID3D11Device *device;

        const UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                                      featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
                                      &swapchainDesc, &swapchain, &device, nullptr, &deviceContext);

        // This updates the swapchainDesc with the actual HWND window size
        swapchain->GetDesc(&swapchainDesc);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // FRAMEBUFFER AND RENDER TARGET
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        // Grab framebuffer from swapchain
        swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&framebuffer);

        // Needed for SRGB framebuffer when using FLIP model swap effect
        D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVDesc = {};
        framebufferRTVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        framebufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        device->CreateRenderTargetView(framebuffer, &framebufferRTVDesc, &framebufferRTV);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // DEPTH AND STENCIL BUFFERS
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_TEXTURE2D_DESC depthBufferDesc;

        // Copy framebuffer properties, they are mostly the same
        framebuffer->GetDesc(&depthBufferDesc);

        depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        device->CreateTexture2D(&depthBufferDesc, nullptr, &depthBuffer);

        device->CreateDepthStencilView(depthBuffer, nullptr, &depthBufferDSV);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // CREATE VERTEX SHADER
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        ID3DBlob *vertexShaderCSO;

        D3DCompileFromFile(L"C:\\work\\cpp\\nora\\engine\\engine\\shaders\\test.hlsl", nullptr, nullptr,
                           "VertexShaderMain", "vs_5_0", 0, 0, &vertexShaderCSO, nullptr);

        device->CreateVertexShader(vertexShaderCSO->GetBufferPointer(),
                                   vertexShaderCSO->GetBufferSize(), nullptr, &vertexShader);

        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
             D3D11_INPUT_PER_VERTEX_DATA,                                                                          0 },
            { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,
             D3D11_INPUT_PER_VERTEX_DATA,                                                                          0 },
            { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
             D3D11_INPUT_PER_VERTEX_DATA,                                                                          0 },
        };

        device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc),
                                  vertexShaderCSO->GetBufferPointer(),
                                  vertexShaderCSO->GetBufferSize(), &inputLayout);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // CREATE PIXEL SHADER
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        ID3DBlob *pixelShaderCSO;

        D3DCompileFromFile(L"C:\\work\\cpp\\nora\\engine\\engine\\shaders\\test.hlsl", nullptr, nullptr,
                           "PixelShaderMain", "ps_5_0", 0, 0, &pixelShaderCSO, nullptr);

        device->CreatePixelShader(pixelShaderCSO->GetBufferPointer(),
                                  pixelShaderCSO->GetBufferSize(), nullptr, &pixelShader);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // RASTERIZER STATE
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_RASTERIZER_DESC rasterizerDesc = {};
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;

        device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // SAMPLER STATE
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        device->CreateSamplerState(&samplerDesc, &samplerState);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // DEPTH STENCIL STATE
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
        depthStencilDesc.DepthEnable = TRUE;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

        device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // CONSTANT BUFFER SETUP
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_BUFFER_DESC constantBufferDesc = {};
        constantBufferDesc.ByteWidth =
            sizeof(RenderConstants) + 0xf &
            0xfffffff0; // ensure constant buffer size is multiple of 16 bytes
        constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
        constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // TEXTURE DESCRIPTION AND SHADER RESOURCE VIEW
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = TEST_CUBE_TEXTURE_WIDTH;
        textureDesc.Height = TEST_CUBE_TEXTURE_HEIGHT;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // same as framebuffer(view)
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA textureSRD = {};
        textureSRD.pSysMem = testCubeTextureData;
        textureSRD.SysMemPitch = TEST_CUBE_TEXTURE_WIDTH * sizeof(UINT);

        device->CreateTexture2D(&textureDesc, &textureSRD, &texture);

        device->CreateShaderResourceView(texture, nullptr, &textureSRV);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // VERTEX BUFFER
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.ByteWidth = sizeof(testCubeVertexData);
        vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexBufferSRD = { testCubeVertexData };

        device->CreateBuffer(&vertexBufferDesc, &vertexBufferSRD, &vertexBuffer);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // INDEX BUFFER
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_BUFFER_DESC indexBufferDesc = {};
        indexBufferDesc.ByteWidth = sizeof(testCubeIndexData);
        indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexBufferSRD = { testCubeIndexData };

        device->CreateBuffer(&indexBufferDesc, &indexBufferSRD, &indexBuffer);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // VIEWPORT
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        viewport = { 0.0f,
                     0.0f,
                     (float)swapchainDesc.BufferDesc.Width,
                     (float)swapchainDesc.BufferDesc.Height,
                     0.0f,
                     1.0f };

        viewportWidth = viewport.Width / viewport.Height; // width (aspect ratio)
    }

    void RendererD3D11::ProcessFrame()
    {
        if (windowHandle == nullptr)
        {
            return;
        }

        Matrix4x4 rotateX = { 1,
                              0,
                              0,
                              0,
                              0,
                              (float)cos(modelRotation.x),
                              -(float)sin(modelRotation.x),
                              0,
                              0,
                              (float)sin(modelRotation.x),
                              (float)cos(modelRotation.x),
                              0,
                              0,
                              0,
                              0,
                              1 };
        Matrix4x4 rotateY = {
            (float)cos(modelRotation.y),  0, (float)sin(modelRotation.y), 0, 0, 1, 0, 0,
            -(float)sin(modelRotation.y), 0, (float)cos(modelRotation.y), 0, 0, 0, 0, 1
        };
        Matrix4x4 rotateZ = { (float)cos(modelRotation.z),
                              -(float)sin(modelRotation.z),
                              0,
                              0,
                              (float)sin(modelRotation.z),
                              (float)cos(modelRotation.z),
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
            modelScale.x, 0, 0, 0, 0, modelScale.y, 0, 0, 0, 0, modelScale.z, 0, 0, 0, 0, 1
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
                                modelTranslation.x,
                                modelTranslation.y,
                                modelTranslation.z,
                                1 };

        modelRotation.x += 0.005f;
        modelRotation.y += 0.009f;
        modelRotation.z += 0.001f;

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // MAP DEVICE CONTEXT
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        D3D11_MAPPED_SUBRESOURCE constantBufferMSR;

        // update constant buffer every frame
        deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantBufferMSR);
        {
            RenderConstants *constants = (RenderConstants *)constantBufferMSR.pData;
            constants->transform = rotateX * rotateY * rotateZ * scale * translate;
            constants->projection = { 2 * viewportNearClip / viewportWidth,
                                      0,
                                      0,
                                      0,
                                      0,
                                      2 * viewportNearClip / viewportHeight,
                                      0,
                                      0,
                                      0,
                                      0,
                                      viewportFarClip / (viewportFarClip - viewportNearClip),
                                      1,
                                      0,
                                      0,
                                      viewportNearClip * viewportFarClip /
                                          (viewportNearClip - viewportFarClip),
                                      0 };
            constants->lightDirection = { 1.0f, -1.0f, 1.0f };
        }
        deviceContext->Unmap(constantBuffer, 0);

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // CLEAR AND SETUP FRAME
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        deviceContext->ClearRenderTargetView(framebufferRTV, clearColor);
        deviceContext->ClearDepthStencilView(depthBufferDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        deviceContext->IASetInputLayout(inputLayout);
        deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
        deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

        deviceContext->VSSetShader(vertexShader, nullptr, 0);
        deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

        deviceContext->RSSetViewports(1, &viewport);
        deviceContext->RSSetState(rasterizerState);

        deviceContext->PSSetShader(pixelShader, nullptr, 0);
        deviceContext->PSSetShaderResources(0, 1, &textureSRV);
        deviceContext->PSSetSamplers(0, 1, &samplerState);

        deviceContext->OMSetRenderTargets(1, &framebufferRTV, depthBufferDSV);
        deviceContext->OMSetDepthStencilState(depthStencilState, 0);
        deviceContext->OMSetBlendState(nullptr, nullptr,
                                       0xffffffff); // use default blend mode (no blending)

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
        // DRAW FRAME AND PRESENT SWAPCHAIN
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

        deviceContext->DrawIndexed(ARRAYSIZE(testCubeIndexData), 0, 0);
        swapchain->Present(1, 0);
    }

    void RendererD3D11::Shutdown() 
    {
        indexBuffer->Release();
        indexBuffer = nullptr;

        vertexBuffer->Release();
        vertexBuffer = nullptr;
    }
} // namespace nora