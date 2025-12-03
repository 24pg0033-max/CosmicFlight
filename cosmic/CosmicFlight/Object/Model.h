#pragma once
#include "../DxSystem.h"

class Model
{
public:
    Model();
    ~Model();

    void Initialize();
    void Draw();

private:
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        float u, v;
    };

    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    int indexCount;
};
