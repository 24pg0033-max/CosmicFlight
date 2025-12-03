// FBXMesh.h
#define _CRT_SECURE_NO_WARNINGS

#pragma once
#include <d3d11.h>
#include <iostream>
#include "Texture.h"
#include "Math\math.h"

#include <map>

#include <fbxsdk.h>

class FBXMesh
{
protected:
	char FbxName[128]; // ファイル名

	//	頂点構造体（シェーダーと一致）
	struct PolygonVertex {
		float x, y, z;		// 座標
		float nx, ny, nz;	// 法線
		float tu, tv;		// UV
		float r, g, b, a;		// 頂点カラー
		float tanx, tany, tanz;	// 接線
		float bx, by, bz;	//従法線　1020
	};

	int NumMesh;
	int NumVertices;	//全頂点数
	int NumFaces;		//全ポリゴン数
	PolygonVertex* Vertices;	//頂点
	PolygonVertex* VerticesSrc; //頂点元データ
	DWORD* Indices;		//三角形（頂点結び方）

	ID3D11Buffer* VertexBuffer;	// 頂点バッファ
	ID3D11Buffer* IndexBuffer;		// インデックスバッファ

	char FBXDir[128];
	int* MeshMaterial;
	int* MaterialFaces;
	Texture** Textures;

	//	ボーン関連
	struct BONE {
		char	Name[128];
		Matrix	OffsetMatrix;
		Matrix	transform;
		int		parent;
	};
	int NumBone;
	BONE Bone[256];

	void InitializeBone(FbxScene* scene);
	int FindBone(const char* name);
	void LoadBone(FbxMesh* mesh);
	void LoadMeshAnim(FbxMesh* mesh);

	//FBX読み込み最適化用
	void SaveAMG(const char* name);
	bool LoadAMG(const char* name);
	//	ウェイト関連
	struct WEIGHT {
		int count;
		int bone[4];
		float weight[4];
	};
	WEIGHT* Weights;

	int StartFrame;
	void Skinning();	// ボーンによる変形

	static const int MOTION_MAX = 256;
	static const int BONE_MAX = 256;

	//	アニメーション
	struct Motion {
		int NumFrame;	// フレーム数	
		//D3DXMATRIX key[BONE_MAX][120];	// キーフレーム
		Matrix* key[BONE_MAX];	// キーフレーム
		Matrix* key2[BONE_MAX];	// キーフレーム
	};
	// int MotionNo;

	int NumMotion;		// モーション数
	std::map<std::string, Motion> motion;	// モーションデータ
	void LoadKeyFrames(std::string name, int bone, FbxNode* bone_node, FbxAnimLayer* AnimLayer);
	float GetCurveValue(FbxAnimCurve* curve, FbxTime T);

	FbxScene* scene;
	void Load(const char* filename);
	void LoadMaterial(int index, FbxSurfaceMaterial* material);

	void LoadPosition(FbxMesh* mesh);
	void LoadNormal(FbxMesh* mesh);
	void LoadTangent(FbxMesh* mesh);
	void LoadBinormal(FbxMesh* mesh);
	void LoadUV(FbxMesh* mesh);
	void LoadVertexColor(FbxMesh* mesh);

	void OptimizeVertices();

public:
	int Raycast(Vector3& pos, Vector3& v, Vector3& out, Vector3& normal);
	int RaycastTRS(Vector3& pos, Vector3& v, Vector3& out, Vector3& normal);
	void SetHeight(Texture* HeightMap);
	void ReplaceTexture(int index, Texture* tex) {
		Textures[index] = tex;
	}
	FBXMesh();
	virtual ~FBXMesh();

	void Update();

	void Render();	//	描画

	bool IsMotionLooped;
	bool IsMotionFinished;
	bool IsLoop = false;	//!<ループするかどうか
	void Play(std::string name, bool isLoop = true, float BlendTime = 0.0f) {
		// 古いデータを保存して新モーションと同時再生
		oldMotionName = MotionName;
		oldIsLoop = IsLoop;
		oldFrame = motionFrame;
		nowBlendTime = maxBlendTime = BlendTime;

		//新しいモーションに変更
		MotionName = name;
		motionFrame = 1.0f;
		IsMotionLooped = false;
		IsMotionFinished = false;
		IsLoop = isLoop;
		lastMotionFrame = static_cast<float>(motion[name].NumFrame);
	}
	//1つ前のモーション情報
	std::string oldMotionName;		//!<古いモーション
	float oldFrame = 0;				//!< 前モーションのフレーム
	bool oldIsLoop = false;			//!< 前モーションをループするか
	float nowBlendTime = 0;			//!< 現在のブレンドの進行時間
	float maxBlendTime = 0;			//!< ブレンドにかける最大時間

	void Animate(float sec);	// モーション再生
	float GetMotionTimeRate(); // 現在の再生時間の割合(始め0--->1終了)
	void Create(const char* filename);
	void Copy(FBXMesh* mesh);
	bool IsClone;
	void Clone(FBXMesh* source);
	void AddMotion(std::string name, const char* filename);

	//	モーション情報
	float motionFrame;		// 現在のフレーム
	std::string MotionName;		// 現在のモーション
	float lastMotionFrame = 0;		//!<現在のアニメーションのフレーム数

	//mixamoのモーション補正用
	const int JumpYCorrection = 150;		//ジャンプアニメーション中のY座標補正値


	//	姿勢情報
	Matrix transform;
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
	//	ボーン行列取得
	Matrix GetBoneMatrix(int bone) {
		return Bone[bone].transform;
	}

	char TextureName[32][128];
};


