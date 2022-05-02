#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "../../Rendering/RenderTarget.h"

class D3D12RenderTarget :
    public RenderTarget
{
public:

    D3D12RenderTarget(const char* resourceName, const Int2& resourceSize, ResourceFormat format);
    ~D3D12RenderTarget();

    bool Create(ID3D12Device* device);

    ID3D12Resource* GetResource()const;

private:

    Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargetResource;
};

