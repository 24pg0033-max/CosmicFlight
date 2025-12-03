#include "DxSystem.h"
#include "Render2D.h"
#include "Memory/Memory.h"
#include "RenderState.h"

Render2D::Render2D() {
	texture = nullptr;
	shader = nullptr;
	vertices = nullptr;
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;
}
Render2D::~Render2D()
{
	if (vertices) { delete[] vertices; vertices = nullptr; }
	if (texture) { delete texture; texture = nullptr; }
	if (shader) { delete shader; shader = nullptr; }
	if (VertexBuffer) { VertexBuffer->Release(); VertexBuffer = nullptr; }
	if (IndexBuffer) { IndexBuffer->Release(); IndexBuffer = nullptr; }
}
//初期化
void Render2D::Initialize(const char* filename, Shader* shader)
{
	bool result = false;
	if (filename != nullptr) {
		texture = NEW Texture();
		result = texture->Load(filename);
		if (!result)return;
	}
	if (shader != nullptr) {
		this->shader = shader;
	}
	else {
		this->shader = NEW Shader();
		if (texture != nullptr) {
			//テクスチャを貼る場合
			result = this->shader->Create(L"Assets/shader/2D.fx", "VSMain", "PSTex");
		}
		else {
			//テクスチャを貼らない場合
			result = this->shader->Create(L"Assets/shader/2D.fx", "VSMain", "PSBase");
		}
		if (!result)return;
	}
	//	頂点初期化
	vertices = NEW Vertex[4];
	for (int i = 0; i < 4; i++) {
		vertices[i].r = 1;
		vertices[i].g = 1;
		vertices[i].b = 1;
		vertices[i].a = 1;
	}
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	// サブリソースの設定
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = vertices;
	HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &initData, &VertexBuffer);
	if (FAILED(hr))return;
	int* indices = NEW int[6]();
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 3;
	indices[5] = 2;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = DxSystem::Device->CreateBuffer(&bd, &InitData, &IndexBuffer);
	delete[] indices;
	if (FAILED(hr))return;
	float sx = texture != nullptr ? static_cast<float>(texture->GetWidth()) : 64.f;
	float sy = texture != nullptr ? static_cast<float>(texture->GetHeight()) : 64.f;
	position = Vector3(0, 0, 0);
	width = sx;
	height = sy;
	u = v = 0;
	w = h = 1;
	cx = cy = 0;
	r = g = b = a = 1.0f;
	rotation = 0;
}
//--------------------------------------------------
// 描画
//--------------------------------------------------
void Render2D::Render()
{
	RenderState::DepthEnable(false);
	UpdateVertices();
	shader->Activate();
	if (texture != nullptr) {
		texture->Set();
	}
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	DxSystem::DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	DxSystem::DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//	描画
	DxSystem::DeviceContext->DrawIndexed(6, 0, 0);
	RenderState::DepthEnable(true);
}
void Render2D::SetPosition(const Vector3& pos)
{
	position = pos;
}
//void Render2D::SetPosition(float x, float y) {
//	position.x = x;
//	position.y = y;
//	position.z = 0;
//}
void Render2D::SetPivot(float x, float y)
{
	cx = x;
	cy = y;
}
void Render2D::SetSize(float width, float height)
{
	this->width = width;
	this->height = height;
}
void Render2D::SetUV(float u, float v, float w, float h)
{
	this->u = u;
	this->v = v;
	this->w = w;
	this->h = h;
}
void Render2D::SetColor(float r, float g, float b, float a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}
//	全頂点更新
void Render2D::UpdateVertices()
{
	//左上
	vertices[0].x = -cx;
	vertices[0].y = -cy;
	vertices[0].tu = u;
	vertices[0].tv = v;
	//左下
	vertices[1].x = -cx;
	vertices[1].y = -cy + height;
	vertices[1].tu = u;
	vertices[1].tv = v + h;
	//右上
	vertices[2].x = -cx + width;
	vertices[2].y = -cy;
	vertices[2].tu = u + w;
	vertices[2].tv = v;
	//右下
	vertices[3].x = -cx + width;
	vertices[3].y = -cy + height;
	vertices[3].tu = u + w;
	vertices[3].tv = v + h;
	//回転
	float fsin = sinf(rotation);
	float fcos = cosf(rotation);
	for (int i = 0; i < 4; i++) {
		float dx = vertices[i].x * fcos - vertices[i].y * fsin;
		float dy = vertices[i].x * fsin + vertices[i].y * fcos;
		vertices[i].x = dx;
		vertices[i].y = dy;
		vertices[i].x += position.x;
		vertices[i].y += position.y;
		vertices[i].x /= DxSystem::ScreenWidth;
		vertices[i].y /= DxSystem::ScreenHeight;
		vertices[i].x = vertices[i].x * 2 - 1;
		vertices[i].y = vertices[i].y * 2 - 1;
		vertices[i].y *= -1;
	}
	//RGBA更新
	for (int i = 0; i < 4; i++) {
		vertices[i].r = r;
		vertices[i].g = g;
		vertices[i].b = b;
		vertices[i].a = a;
	}
	vertices[0].z = vertices[1].z = vertices[2].z = vertices[3].z = position.z;
	DxSystem::DeviceContext->UpdateSubresource(VertexBuffer, 0, NULL, vertices, 0, 0);
}

void Render2D::Initialize()
{
	bool result = baseShader.Create(L"Assets/shader/2D.fx", "VSMain", "PSTex");
	assert(result);

	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(CBParam);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	Matrix matrix;
	Vector3 p = Vector3(-1, 1, 0);
	Vector3 r = Vector3(0, 0, 0);
	Vector3 s = Vector3(1, -1, 1);
	matrix.TRS(p, r, s);
	CBParam param;
	param.proj.Ortho(1280, 720, 0, 1000);
	param.proj *= matrix;

	// サブリソースの設定.
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = &param;

	DxSystem::Device->CreateBuffer(&bd, &initData, &ConstantBuffer);
}

Shader Render2D::baseShader;
ID3D11Buffer* Render2D::ConstantBuffer;