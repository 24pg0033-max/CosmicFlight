#include "Model.h"

Model::Model()
    : vertexBuffer(nullptr), indexBuffer(nullptr), indexCount(0)
{}

Model::~Model()
{
    if (vertexBuffer) vertexBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
}

void Model::Initialize()
{
    // 板ポリ：左上から時計回り
    Vertex vertices[] =
    {
        { {-0.5f, 0.5f, 0}, {0, 0, -1}, 0, 0 },
        { { 0.5f, 0.5f, 0}, {0, 0, -1}, 1, 0 },
        { { 0.5f,-0.5f, 0}, {0, 0, -1}, 1, 1 },
        { {-0.5f,-0.5f, 0}, {0, 0, -1}, 0, 1 },
    };

    DWORD indices[] = { 0, 1, 2, 2, 3, 0 };
    indexCount = 6;

    // 頂点バッファ
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices;
    DxSystem::Device->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);

    // インデックスバッファ
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices;
    DxSystem::Device->CreateBuffer(&ibDesc, &ibData, &indexBuffer);
}

void Model::Draw()
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    DxSystem::DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    DxSystem::DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    DxSystem::DeviceContext->DrawIndexed(indexCount, 0, 0);
}
