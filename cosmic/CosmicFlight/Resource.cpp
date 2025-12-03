#include "Resource.h"
#include "Memory.h"


Resource::Resource()
{
	meshList.clear();
}

bool Resource::Load()
{
	// Œ³ƒ‚ƒfƒ‹“Ç‚Ýž‚Ý
	meshList[MeshType::Bat] = NEW FBXMesh();
	meshList[MeshType::Bat]->Create("Assets/Monster Bat/Monster_Bat.FBX");
	meshList[MeshType::Bat]->AddMotion("Idle", "Assets/iga/iga@01wait.fbx");

	//meshList[MeshType::Sword] = new FBXMesh();
	//meshList[MeshType::Sword]->Create("Assets/sword/MagicSword.fbx");

	return true;
}

void Resource::Release()
{
	//delete meshList[MeshType::Iga];
	//delete meshList[MeshType::Sword];

	meshList.clear();
}