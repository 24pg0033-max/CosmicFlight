#pragma once
#include <map>
#include "FbxMesh.h"


enum class MeshType
{
	Bat,
	Meteor,
	Star,
};

class Resource
{
public:
	Resource();

	bool Load();
	void Release();

public:
	FBXMesh* GetMesh(MeshType type) { return meshList[type]; }

private:
	std::map<MeshType, FBXMesh*> meshList;
};