#include <d3d11.h>
#include "Texture.h"
#include "Memory/Memory.h"

#if _DEBUG
#pragma comment ( lib, "DirectXTex/DirectXTexD.lib")
#else
#pragma comment ( lib, "DirectXTex/DirectXTex.lib")
#endif

ID3D11ShaderResourceView* Texture::Default_Normal;

Texture::Texture()
{
}


Texture::~Texture()
{
	//ShaderResourceView->Release();
}

bool Texture::LoadNormal(const char* filename)
{
	// assets/abc.png
	char name[256]; // "assets/abc"
	//char* ext;	// ".png"
	// "." を探す
	int end_of_name;
	for (int i = static_cast<int>(strlen(filename)) - 1; i >= 0; i--) {
		if (filename[i] == '.') { end_of_name = i; break; }
	}

	// filenameをnameにコピー
	strcpy_s(name, sizeof(name), filename);
	name[end_of_name] = '\0'; // end_of_nameで文字列を終了

	// 拡張子
	const char* ext = &filename[end_of_name]; // ".png

	// ファイル名作成
	char normalName[256];
	sprintf_s(normalName, sizeof(normalName), "%s_n%s", name, ext); // ファイル名と拡張子の間に"_n"を入れる

	wchar_t	wchar[256];
	size_t wLen = 0;
	errno_t err = 0;

	//変換
	mbstowcs_s(&wLen, wchar, 256, normalName, _TRUNCATE);

	// 画像ファイル読み込み DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	//HRESULT hr = LoadFromWICFile(wchar, 0, &metadata, image);
	HRESULT hr;
	if (_strcmpi(&normalName[strlen(normalName) - 3], "DDS") == 0) {
		hr = LoadFromDDSFile(wchar, DirectX::DDS_FLAGS_NONE, &metadata, image);
	}
	else if (_strcmpi(&normalName[strlen(normalName) - 3], "TGA") == 0) {
		hr = LoadFromTGAFile(wchar, &metadata, image);
	}
	else {
		hr = LoadFromWICFile(wchar, DirectX::WIC_FLAGS_NONE, &metadata, image);
	}

	if (FAILED(hr)) {
		return false;
	}

	// 画像からシェーダリソースView
	hr = CreateShaderResourceView(DxSystem::Device, image.GetImages(), image.GetImageCount(), metadata, &SRV_Normal);
	if (FAILED(hr)) {
		return false;
	}
	image.Release();

	return true;
}

bool Texture::Load(const char* filename)
{
	wchar_t	wchar[256];
	size_t wLen = 0;
	errno_t err = 0;

	//変換
	mbstowcs_s(&wLen, wchar, 256, filename, _TRUNCATE);

	// 画像ファイル読み込み DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	//HRESULT hr = LoadFromWICFile(wchar, 0, &metadata, image);
	HRESULT hr;
	if (_strcmpi(&filename[strlen(filename) - 3], "DDS") == 0) {
		hr = LoadFromDDSFile(wchar, DirectX::DDS_FLAGS_NONE, &metadata, image);
	}
	else if (_strcmpi(&filename[strlen(filename) - 3], "TGA") == 0) {
		hr = LoadFromTGAFile(wchar, &metadata, image);
	}
	else {
		hr = LoadFromWICFile(wchar, DirectX::WIC_FLAGS_NONE, &metadata, image);
	}

	if (FAILED(hr)) {
		return false;
	}

	width = (int)metadata.width;
	height = (int)metadata.height;

	// 画像からシェーダリソースView
	hr = CreateShaderResourceView(DxSystem::Device, image.GetImages(), image.GetImageCount(), metadata, &ShaderResourceView);
	if (FAILED(hr)) {
		return false;
	}
	image.Release();

	return true;
}

bool Texture::LoadMipmap(const char* filename)
{
	wchar_t	wchar[256];
	size_t wLen = 0;
	errno_t err = 0;

	//変換
	mbstowcs_s(&wLen, wchar, 256, filename, _TRUNCATE);

	// 画像ファイル読み込み DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	//HRESULT hr = LoadFromWICFile(wchar, 0, &metadata, image);
	HRESULT hr;
	if (_strcmpi(&filename[strlen(filename) - 3], "DDS") == 0) {
		hr = LoadFromDDSFile(wchar, DirectX::DDS_FLAGS_NONE, &metadata, image);
	}
	else if (_strcmpi(&filename[strlen(filename) - 3], "TGA") == 0) {
		hr = LoadFromTGAFile(wchar, &metadata, image);
	}
	else {
		hr = LoadFromWICFile(wchar, DirectX::WIC_FLAGS_NONE, &metadata, image);
	}

	if (FAILED(hr)) {
		return false;
	}

	width = (int)metadata.width;
	height = (int)metadata.height;
	// ミップマップ生成
	DirectX::ScratchImage chain;
	DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_DEFAULT,
		0, chain);
	// 画像からシェーダリソースView
	hr = CreateShaderResourceView(DxSystem::Device,
		chain.GetImages(), chain.GetImageCount(),
		chain.GetMetadata(), &ShaderResourceView);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

void Texture::Set(int slot)
{
	DxSystem::DeviceContext->PSSetShaderResources(slot, 1, &ShaderResourceView);
	DxSystem::DeviceContext->VSSetShaderResources(slot, 1, &ShaderResourceView);
	// 指定slot0の場合､法線マップも登録
	if (slot == 0) {
		// slot1に法線マップを登録する
		if (SRV_Normal != nullptr) DxSystem::DeviceContext->PSSetShaderResources(1, 1, &SRV_Normal);
		else DxSystem::DeviceContext->PSSetShaderResources(1, 1, &Default_Normal);
		//// slot2にラフネスを登録する
		//if (SRV_Roughness != nullptr) DxSystem::DeviceContext->PSSetShaderResources(2, 1, &SRV_Roughness);
		//else DxSystem::DeviceContext->PSSetShaderResources(2, 1, &Default_Roughness);
		//// slot3にメタリックを登録する
		//if (SRV_Metallic != nullptr) DxSystem::DeviceContext->PSSetShaderResources(3, 1, &SRV_Metallic);
		//else DxSystem::DeviceContext->PSSetShaderResources(3, 1, &Default_Metallic);
	}
}

BYTE* Texture::GetPixels()
{
	DirectX::ScratchImage image;
	ID3D11Resource* source;
	ShaderResourceView->GetResource(&source);
	HRESULT hr = CaptureTexture(DxSystem::Device, DxSystem::DeviceContext, source, image);
	uint8_t* ptr = image.GetImage(0, 0, 0)->pixels;

	BYTE* buf = NEW BYTE[width * height * 4];
	CopyMemory(buf, ptr, width * height * 4);
	return buf;
}

void Texture::LoadDefault() {
	wchar_t	wchar[256];
	size_t wLen = 0;
	errno_t err = 0;

	// 変換
	mbstowcs_s(&wLen, wchar, 256, "Assets/default_n.png", _TRUNCATE);

	// 画像ファイル読み込み DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;

	HRESULT hr;
	hr = LoadFromWICFile(wchar, DirectX::WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr)) return;

	// 画像からシェーダリソースView
	hr = CreateShaderResourceView(DxSystem::Device, image.GetImages(), image.GetImageCount(), metadata, &Default_Normal);

	if (FAILED(hr)) return;
	image.Release();
}
