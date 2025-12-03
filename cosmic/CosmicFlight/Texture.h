#pragma once
#include "DxSystem.h"
#include "DirectXTex\DirectXTex.h"

class Texture
{
private:
	static ID3D11ShaderResourceView* Default_Normal;
	ID3D11ShaderResourceView* SRV_Normal = nullptr;
	bool LoadNormal(const char* filename);
private:
	ID3D11ShaderResourceView* ShaderResourceView;
	int width;
	int height;

public:
	Texture();
	~Texture();

	bool Load(const char* filename);
	bool LoadMipmap(const char* filename);
	void Set(int slot = 0);

	int GetWidth() { return width; }
	int GetHeight() { return height; }

	BYTE* GetPixels();

	static void LoadDefault();
};
