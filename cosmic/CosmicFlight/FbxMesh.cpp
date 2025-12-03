#include "FbxMesh.h"
#include "Memory/Memory.h"

#pragma comment( lib, "libfbxsdk.lib")

//****************************************************************
//
//	更新
//
//****************************************************************
//------------------------------------------------
//	更新
//------------------------------------------------
void FBXMesh::Update()
{
	transform.TRS(position, rotation, scale);
}

//------------------------------------------------
//	アニメーション
//------------------------------------------------
void FBXMesh::Animate(float sec)
{
	float DeltaTime = sec;
	if (IsMotionFinished)return;

	//	モーション時間の更新
	motionFrame += DeltaTime * 60 * 2.0f;
	//	ループチェック
	if (motionFrame >= motion[MotionName].NumFrame - 1)
	{
		if (IsLoop) {
			// ループ
			motionFrame = 0;		// 全体をループ
			IsMotionLooped = true;
		}
		else {
			motionFrame = motion[MotionName].NumFrame - 1.0f;
			IsMotionFinished = true;
		}
	}

	// ブレンド時間が残っていたら
	if (nowBlendTime > 0.0f)
	{
		// フレームの更新をしてブレンド時間を進める
		oldFrame += DeltaTime * 60;
		nowBlendTime -= DeltaTime;

		// 旧モーションのループ
		if (oldIsLoop)
		{
			if (oldFrame >= motion[oldMotionName].NumFrame - 2.0f)
			{
				oldFrame = 0;
			}
		}
		else
		{
			if (oldFrame >= motion[oldMotionName].NumFrame - 2.0f)
			{
				oldFrame = motion[oldMotionName].NumFrame - 2.0f;
			}
		}
	}


}

float FBXMesh::GetMotionTimeRate()
{
	// モーションのフレーム数
	float endF = motion[MotionName].NumFrame - 2.0f;
	return motionFrame / endF;
}

//****************************************************************
//
//	描画
//
//****************************************************************
//------------------------------------------------
//	描画
//------------------------------------------------
void FBXMesh::Render() {

	//	モーションが存在する場合はSkinning
	if (motion[MotionName].NumFrame > 0) {
		Skinning();
	}

	// 頂点バッファ設定
	UINT stride = sizeof(PolygonVertex);
	UINT offset = 0;
	DxSystem::DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	DxSystem::DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int start = 0;
	//	マテリアルに分けて描画

	for (int m = 0; m < NumMesh; m++) {
		int material_no = MeshMaterial[m];
		if (Textures[material_no] != NULL) {
			Textures[material_no]->Set();
		}
		else {

		}
		//	描画
		DxSystem::DeviceContext->DrawIndexed(MaterialFaces[m] * 3, start, 0);
		start += MaterialFaces[m] * 3;
	}
}


//****************************************************************
//
//	初期化
//
//****************************************************************
FBXMesh::FBXMesh()
{
	IsClone = false;
}

//****************************************************************
//
//	解放
//
//****************************************************************
FBXMesh::~FBXMesh()
{
	//	頂点情報解放
	delete[] Vertices;
	if (VertexBuffer) { VertexBuffer->Release(); VertexBuffer = NULL; }

	if (IsClone == false) {
		//	頂点情報解放
		delete[] Vertices;
		delete[] Indices;
		delete[] Weights;
		delete[] VerticesSrc;
		//	材質関連解放
		delete[] MaterialFaces;
		delete[] MeshMaterial;
		//	テクスチャ解放
		for (int i = 0; i < NumMesh; i++) {
			if (Textures[i] != NULL)
				delete Textures[i];
		}
		delete[] Textures;

		//	モーション関連解放
		for (auto i = motion.begin(); i != motion.end(); i++) {
			Motion* M = &i->second;
			for (int bone = 0; bone < NumBone; bone++) {
				delete[] M->key[bone];
			}
		}
		motion.clear();

		if (VertexBuffer) { VertexBuffer->Release(); VertexBuffer = NULL; }
		if (IndexBuffer) { IndexBuffer->Release(); IndexBuffer = NULL; }
	}
}

//****************************************************************
//
//	ファイル読み込み
//
//****************************************************************
void FBXMesh::Create(const char* filename)
{
	//ファイル名を取り除く
	strcpy(FBXDir, filename);
	for (int n = (int)strlen(FBXDir) - 1; n >= 0; n--)
	{
		if (FBXDir[n] == '/' || FBXDir[n] == '\\')
		{
			FBXDir[n + 1] = '\0';
			break;
		}
	}

	Load(filename);

	// 頂点バッファ生成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(PolygonVertex) * NumVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		// サブリソースの設定.
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = Vertices;
		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &initData, &VertexBuffer);
	}

	// インデックスバッファ生成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(DWORD) * NumFaces * 3;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = Indices;

		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, &IndexBuffer);
	}

	//	情報初期化
	motionFrame = 0;
	position = Vector3(0, 0, 0);
	rotation = Vector3(0, 0, 0);
	scale = Vector3(1, 1, 1);
	Update();
}

void FBXMesh::Copy(FBXMesh* mesh)
{
	Vertices = NEW PolygonVertex[mesh->NumVertices];
	memcpy(Vertices, mesh->VerticesSrc, sizeof(PolygonVertex) * mesh->NumVertices);

}

void FBXMesh::Clone(FBXMesh* source)
{
	IsClone = true;  // protectedにbool型

	//全要素コピー or 生成
	NumMesh = source->NumMesh;
	NumVertices = source->NumVertices; //全頂点数
	NumFaces = source->NumFaces;		 //全ポリゴン数

	// 頂点情報はキャラごとに生成
	Vertices = NEW PolygonVertex[NumVertices]; //頂点
	CopyMemory(Vertices,
		source->Vertices,
		sizeof(PolygonVertex) * NumVertices);

	VerticesSrc = source->VerticesSrc; //頂点元データ
	Indices = source->Indices;		 //三角形（頂点結び方）

	// 頂点バッファ生成(描画用のバッファ)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(PolygonVertex) * NumVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		// サブリソースの設定.
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = Vertices;
		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &initData, &VertexBuffer);
	}

	IndexBuffer = source->IndexBuffer; // インデックスバッファ

	MeshMaterial = source->MeshMaterial;
	MaterialFaces = source->MaterialFaces;
	Textures = source->Textures;

	NumBone = source->NumBone;
	CopyMemory(Bone, source->Bone, sizeof(Bone));

	Weights = source->Weights;

	NumMotion = source->NumMotion; // モーション数
	motion = source->motion; // モーションデータ

	StartFrame = source->StartFrame;
	motionFrame = 0;
	MotionName = source->MotionName;
}

//------------------------------------------------
//	ファイル読み込み
//------------------------------------------------
void FBXMesh::Load(const char* filename)
{

	//// amgファイル読み込みチェック
	//if (LoadAMG(filename)) {
	//	//	頂点元データ保存
	//	VerticesSrc = NEW PolygonVertex[NumVertices];
	//	memcpy(VerticesSrc, Vertices, sizeof(PolygonVertex) * NumVertices);
	//	Play("default");
	//	return;
	//}

	// ファイル名保存
	strcpy(FbxName, filename);

	FbxManager* manager = FbxManager::Create();
	scene = FbxScene::Create(manager, "");
	//	ファイルからシーンに読み込み
	FbxImporter* importer = FbxImporter::Create(manager, "");
	importer->Initialize(filename);
	importer->Import(scene);
	importer->Destroy();

	//	モーション情報取得
	FbxArray<FbxString*> names;
	scene->FillAnimStackNameArray(names);

	if (names != NULL) {
		//	モーションが存在するとき
		FbxTakeInfo* take = scene->GetTakeInfo(names[0]->Buffer());
		FbxLongLong start = take->mLocalTimeSpan.GetStart().Get();
		FbxLongLong stop = take->mLocalTimeSpan.GetStop().Get();
		FbxLongLong fps60 = FbxTime::GetOneFrameValue(FbxTime::eFrames60);
		StartFrame = (int)(start / fps60);
		motion["default"].NumFrame = (int)((stop - start) / fps60);
	}
	else {
		StartFrame = 0;
		motion["default"].NumFrame = 0;
	}

	//	ボーン情報取得
	int NumBone = scene->GetSrcObjectCount<FbxSkeleton>();
	InitializeBone(scene);

	//	モデルを材質ごとに分割
	FbxGeometryConverter fgc(manager);

	fgc.SplitMeshesPerMaterial(scene, true);
	fgc.Triangulate(scene, true);

	//	メッシュ数
	NumMesh = scene->GetSrcObjectCount<FbxMesh>();

	//	頂点数計算
	int work = 0;
	for (int m = 0; m < NumMesh; m++) {
		FbxMesh* mesh = scene->GetSrcObject<FbxMesh>(m);
		int num = mesh->GetPolygonVertexCount();
		work += num; // 合計頂点数
	}

	//	頂点確保
	Vertices = NEW PolygonVertex[work];
	Indices = NEW DWORD[work];
	Weights = NEW WEIGHT[work];

	NumVertices = 0;
	//	初期化
	for (int v = 0; v < work; v++) {
		Weights[v].count = 0;
	}

	//材質ごとのポリゴン頂点数
	MaterialFaces = NEW int[NumMesh];
	MeshMaterial = NEW int[NumMesh];
	Textures = NEW Texture * [NumMesh];
	for (int m = 0; m < NumMesh; m++)
	{
		Textures[m] = NULL;
	}

	//	頂点読み込み
	for (int m = 0; m < NumMesh; m++) {
		FbxMesh* mesh = scene->GetSrcObject<FbxMesh>(m);
		int num = mesh->GetPolygonVertexCount();

		//	頂点情報読み込み
		LoadPosition(mesh);		//	座標読み込み
		LoadNormal(mesh);		//	法線読み込み
		LoadUV(mesh);			//	テクスチャUV
		LoadTangent(mesh);
		LoadBinormal(mesh);		//従法線読み込み
		LoadVertexColor(mesh);	//	頂点カラー読み込み

		//	インデックス設定(三角形ごと)
		for (int i = 0; i < num; i += 3) {
			Indices[i + 0 + NumVertices] = i + 0 + NumVertices;
			Indices[i + 1 + NumVertices] = i + 1 + NumVertices;
			Indices[i + 2 + NumVertices] = i + 2 + NumVertices;
		}

		//	ボーン読み込み
		LoadBone(mesh);

		//	メッシュの使用材質取得
		FbxLayerElementMaterial* LEM = mesh->GetElementMaterial();
		if (LEM != NULL) {
			//	ポリゴンに貼られている材質番号
			int material_index = LEM->GetIndexArray().GetAt(0);
			//	メッシュ材質のmaterial_index番目を取得
			FbxSurfaceMaterial* material = mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(material_index);
			LoadMaterial(m, material);
		}
		//	使用材質設定
		MeshMaterial[m] = m;
		MaterialFaces[m] = num / 3;

		NumVertices += num;
	}

	NumFaces = NumVertices / 3;
	OptimizeVertices();

	//	頂点元データ保存
	VerticesSrc = NEW PolygonVertex[NumVertices];
	memcpy(VerticesSrc, Vertices, sizeof(PolygonVertex) * NumVertices);

	//	ウェイト正規化
	// ５本以上にまたっがてる場合のため
	for (int v = 0; v < NumVertices; v++) {
		float n = 0;
		//	頂点のウェイトの合計値
		for (int w = 0; w < Weights[v].count; w++) {
			n += Weights[v].weight[w];
		}
		//	正規化
		for (int w = 0; w < Weights[v].count; w++) {
			Weights[v].weight[w] /= n;
		}
	}

	//	解放
	scene->Destroy();
	manager->Destroy();

	Play("default");
	// 読み込み済みモデル保存
	SaveAMG(filename);
}

//****************************************************************
//
//	頂点情報読み込み
//
//****************************************************************
//------------------------------------------------
//	座標読み込み
//------------------------------------------------
void FBXMesh::LoadPosition(FbxMesh* mesh) {
	int* index = mesh->GetPolygonVertices();
	FbxVector4* source = mesh->GetControlPoints();
	// メッシュのトランスフォーム
	FbxVector4 T = mesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 R = mesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 S = mesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);
	FbxAMatrix TRS = FbxAMatrix(T, R, S);
	//	全頂点変換
	for (int v = 0; v < mesh->GetControlPointsCount(); v++) {
		source[v] = TRS.MultT(source[v]);
	}

	// 頂点座標読み込み
	int num = mesh->GetPolygonVertexCount();
	for (int v = 0; v < num; v++) {
		int vindex = index[v];

		Vertices[v + NumVertices].x = (float)source[vindex][0];
		Vertices[v + NumVertices].y = (float)source[vindex][1];
		Vertices[v + NumVertices].z = (float)source[vindex][2];

		Vertices[v + NumVertices].tu = 0;
		Vertices[v + NumVertices].tv = 0;
		Vertices[v + NumVertices].r = 1;
		Vertices[v + NumVertices].g = 1;
		Vertices[v + NumVertices].b = 1;
		Vertices[v + NumVertices].a = 1;
	}
}

//------------------------------------------------
//	法線読み込み
//------------------------------------------------
void FBXMesh::LoadNormal(FbxMesh* mesh) {
	FbxArray<FbxVector4> normal;
	mesh->GetPolygonVertexNormals(normal);
	for (int v = 0; v < normal.Size(); v++)
	{
		Vertices[v + NumVertices].nx = (float)normal[v][0];
		Vertices[v + NumVertices].ny = (float)normal[v][1];
		Vertices[v + NumVertices].nz = (float)normal[v][2];
	}

}

//------------------------------------------------
//	接線読み込み
//------------------------------------------------
void FBXMesh::LoadTangent(FbxMesh* mesh) {

	mesh->GenerateTangentsData(0, true);
	fbxsdk::FbxGeometryElementTangent* tangents = mesh->GetElementTangent(0);
	if (tangents == NULL) return;

	for (int v = 0; v < mesh->GetPolygonVertexCount(); v++)
	{
		FbxVector4 t = tangents->GetDirectArray().GetAt(v);
		Vertices[v + NumVertices].tanx = (float)t[0];
		Vertices[v + NumVertices].tany = (float)t[1];
		Vertices[v + NumVertices].tanz = (float)t[2];
	}
}

//------------------------------------------------
//	従法線読み込み
//------------------------------------------------
void FBXMesh::LoadBinormal(FbxMesh* mesh)
{
	FbxLayerElementArrayTemplate<FbxVector4>* binormals;
	mesh->GetBinormals(&binormals);
	int num = binormals->GetCount();
	for (int j = 0; j < num; j++) {
		FbxVector4 v = binormals->GetAt(j);
		Vertices[j + NumVertices].bx = (float)v[0];
		Vertices[j + NumVertices].by = (float)v[1];
		Vertices[j + NumVertices].bz = (float)v[2];
	}
}

//------------------------------------------------
//	ＵＶ読み込み
//------------------------------------------------
void FBXMesh::LoadUV(FbxMesh* mesh) {
	FbxStringList names;
	mesh->GetUVSetNames(names);
	FbxArray<FbxVector2> uv;
	mesh->GetPolygonVertexUVs(names.GetStringAt(0), uv);
	for (int v = 0; v < uv.Size(); v++) {
		Vertices[v + NumVertices].tu = (float)(uv[v][0]);
		Vertices[v + NumVertices].tv = (float)(1.0 - uv[v][1]);
	}
}

//------------------------------------------------
//	頂点カラー読み込み
//------------------------------------------------
void FBXMesh::LoadVertexColor(FbxMesh* mesh) {

	int vColorLayerCount = mesh->GetElementVertexColorCount();
	if (mesh->GetElementVertexColorCount() <= 0) return;
	//    頂点カラーレイヤー取得
	fbxsdk::FbxGeometryElementVertexColor* element = mesh->GetElementVertexColor(0);

	//  保存形式の取得
	FbxGeometryElement::EMappingMode mapmode = element->GetMappingMode();
	FbxGeometryElement::EReferenceMode refmode = element->GetReferenceMode();

	//    ポリゴン頂点に対するインデックス参照形式のみ対応
	if (mapmode == FbxGeometryElement::eByPolygonVertex)
	{
		if (refmode == FbxGeometryElement::eIndexToDirect)
		{
			FbxLayerElementArrayTemplate<int>* index = &element->GetIndexArray();
			int indexCount = index->GetCount();
			for (int j = 0; j < indexCount; j++) {
				// FbxColor取得
				FbxColor c = element->GetDirectArray().GetAt(index->GetAt(j));
				Vertices[j + NumVertices].r = (float)c.mRed;
				Vertices[j + NumVertices].g = (float)c.mGreen;
				Vertices[j + NumVertices].b = (float)c.mBlue;
				Vertices[j + NumVertices].a = (float)c.mAlpha;
			}
		}
	}
}

//****************************************************************
//	材質読み込み
//****************************************************************
void FBXMesh::LoadMaterial(int index, FbxSurfaceMaterial* material)
{
	TextureName[index][0] = '\0';
	FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

	//	テクスチャ読み込み
	const char* path = NULL;
	int fileTextureCount = prop.GetSrcObjectCount<FbxFileTexture>();
	if (fileTextureCount > 0) {
		FbxFileTexture* FileTex = prop.GetSrcObject<FbxFileTexture>(0);
		path = FileTex->GetFileName();
	}
	else {
		int numLayer = prop.GetSrcObjectCount<FbxLayeredTexture>();
		if (numLayer > 0) {
			FbxLayeredTexture* LayerTex = prop.GetSrcObject<FbxLayeredTexture>(0);
			FbxFileTexture* FileTex = LayerTex->GetSrcObject<FbxFileTexture>(0);
			path = FileTex->GetFileName();
		}
	}
	if (path == NULL) return;

	//  C:\\AAA\\BBB\\a.fbx  C:/AAA/BBB/a.fbx
	const char* name = &path[strlen(path)];
	for (int i = 0; i < (int)strlen(path); i++)
	{
		name--;
		if (name[0] == '/') { name++; break; }
		if (name[0] == '\\') { name++; break; }
	}
	char work[128];
	strcpy(work, FBXDir);		//"AAA/BBB/";
	//	strcat(work, "texture/");	//"AAA/BBB/texture/"
	strcat(work, name);			//"AAA/BBB/texture/a.png

	char filename[128];
	strcpy(filename, work);
	Textures[index] = NEW Texture();
	Textures[index]->Load(filename);
	// テクスチャ名保存
	strcpy(TextureName[index], name);

}

//****************************************************************
//
//		ボーン関連
//
//****************************************************************
//------------------------------------------------
//	ボーン情報初期化
//------------------------------------------------
void FBXMesh::InitializeBone(FbxScene* scene)
{
	NumBone = scene->GetSrcObjectCount<FbxSkeleton>();

	for (int bone_no = 0; bone_no < NumBone; bone_no++) {
		FbxSkeleton* skel = scene->GetSrcObject<FbxSkeleton>(bone_no);
		FbxNode* node = skel->GetNode();

		//	ボーン名取得
		const char* name = node->GetName();
		strcpy(Bone[bone_no].Name, name);

		//	親設定
		Bone[bone_no].parent = NULL;
		FbxNode* parent = node->GetParent();
		int parent_no = -1;
		if (parent) {
			const char* parent_name = parent->GetName();
			if (parent_name) {
				parent_no = FindBone(parent_name);
			}
		}
		Bone[bone_no].parent = parent_no;
	}

	//	メッシュ数
	int NumMesh = scene->GetSrcObjectCount<FbxMesh>();
	for (int m = 0; m < NumMesh; m++) {
		FbxMesh* mesh = scene->GetSrcObject<FbxMesh>(m);
		FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));
		if (!skin) continue;

		//	ボーン数
		int nBone = skin->GetClusterCount();

		//	全ボーン情報取得
		for (int bone = 0; bone < nBone; bone++)
		{
			//	ボーン情報取得
			FbxCluster* cluster = skin->GetCluster(bone);
			FbxAMatrix trans;
			cluster->GetTransformMatrix(trans);

			//	ボーン検索
			const char* name = cluster->GetLink()->GetName();
			int bone_no = FindBone(name);
			if (bone_no < 0) continue;

			//	オフセット行列作成
			FbxAMatrix LinkMatrix;
			cluster->GetTransformLinkMatrix(LinkMatrix);

			FbxAMatrix Offset = LinkMatrix.Inverse() * trans;
			FbxDouble* OffsetM = (FbxDouble*)Offset;
			//	オフセット行列保存
			for (int i = 0; i < 16; i++) {
				Bone[bone_no].OffsetMatrix.m[i] = (float)OffsetM[i];
			}
		}
	}


}

//------------------------------------------------
//	ボーン読み込み
//------------------------------------------------
void FBXMesh::LoadBone(FbxMesh* mesh)
{
	FbxAnimStack* lAnimStack = scene->GetSrcObject<FbxAnimStack>(0);
	if (!lAnimStack) return;
	FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>();

	//	メッシュ頂点数
	int num = mesh->GetPolygonVertexCount();

	//	スキン情報の有無
	int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
	if (skinCount <= 0) {
		LoadMeshAnim(mesh);
		return;
	}
	FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));
	//	ボーン数
	int nBone = skin->GetClusterCount();
	//	全ボーン情報取得
	for (int bone = 0; bone < nBone; bone++)
	{
		//	ボーン情報取得
		FbxCluster* cluster = skin->GetCluster(bone);

		//	ボーン名取得
		const char* name = cluster->GetLink()->GetName();
		//	ボーン検索
		bool isNewBone = false;
		int bone_no = FindBone(name);
		if (bone_no < 0) {
			continue;
		}

		if (isNewBone) {
			//	キーフレーム読み込み
			LoadKeyFrames("default", bone_no, cluster->GetLink(), lAnimLayer);
		}

		//	ウェイト読み込み
		int wgtcount = cluster->GetControlPointIndicesCount();
		int* wgtindex = cluster->GetControlPointIndices();
		double* wgt = cluster->GetControlPointWeights();

		int* index = mesh->GetPolygonVertices();

		for (int i = 0; i < wgtcount; i++) {
			int wgtindex2 = wgtindex[i];
			//	全ポリゴンからwgtindex2番目の頂点検索
			for (int v = 0; v < num; v++) {
				if (index[v] != wgtindex2) continue;
				//	頂点にウェイト保存
				int w = Weights[v + NumVertices].count;
				if (w >= 4) {
					continue;
				}
				Weights[v + NumVertices].bone[w] = bone_no;
				Weights[v + NumVertices].weight[w] = (float)wgt[i];
				Weights[v + NumVertices].count++;
			}
		}
	}
}

//------------------------------------------------
//	ボーン検索
//------------------------------------------------
int FBXMesh::FindBone(const char* name)
{
	int bone = -1; // 見つからない
	for (int i = 0; i < NumBone; i++) {
		if (strcmp(name, Bone[i].Name) == 0) {
			bone = i;
			break;
		}
	}
	return bone;
}


//****************************************************************
//	モーション関連
//****************************************************************
//------------------------------------------------
//	モーション追加
//------------------------------------------------
void FBXMesh::AddMotion(std::string name, const char* filename)
{
	FbxManager* manager = FbxManager::Create();
	scene = FbxScene::Create(manager, "");
	//	ファイルからシーンに読み込み
	FbxImporter* importer = FbxImporter::Create(manager, "");
	importer->Initialize(filename);
	importer->Import(scene);
	importer->Destroy();

	//	モーション情報取得
	FbxArray<FbxString*> names;
	scene->FillAnimStackNameArray(names);

	FbxTakeInfo* take = scene->GetTakeInfo(names[0]->Buffer());
	FbxLongLong start = take->mLocalTimeSpan.GetStart().Get();
	FbxLongLong stop = take->mLocalTimeSpan.GetStop().Get();
	FbxLongLong fps60 = FbxTime::GetOneFrameValue(FbxTime::eFrames60);

	StartFrame = (int)(start / fps60);
	motion[name].NumFrame = (int)((stop - start) / fps60);
	//	ルートノード取得
	FbxNode* root = scene->GetRootNode();


	FbxAnimStack* lAnimStack = scene->GetSrcObject<FbxAnimStack>(0);
	FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>();
	//	全ボーン読み込み
	for (int b = 0; b < NumBone; b++) {
		//	ボーンノード検索
		FbxNode* bone = root->FindChild(Bone[b].Name);
		if (bone == NULL) continue;

		//	キーフレーム読み込み
		LoadKeyFrames(name, b, bone, lAnimLayer);
	}

	//	解放
	scene->Destroy();
	manager->Destroy();
}


//------------------------------------------------
//	ボーンのないメッシュのアニメーション
//------------------------------------------------
void FBXMesh::LoadMeshAnim(FbxMesh* mesh)
{
	FbxNode* node = mesh->GetNode();

	int bone_no = NumBone;
	strcpy(Bone[bone_no].Name, node->GetName());
	Bone[bone_no].parent = NULL;

	//	オフセット行列
	Bone[bone_no].OffsetMatrix.identity();

	//	ウェイト設定
	int num = mesh->GetPolygonVertexCount();
	for (int i = 0; i < num; i++) {
		Weights[i + NumVertices].bone[0] = bone_no;
		Weights[i + NumVertices].weight[0] = 1.0f;
		Weights[i + NumVertices].count = 1;
	}

	NumBone++;
}

void FBXMesh::SaveAMG(const char* name)
{
	char file[256];
	sprintf(file, "%s.amg", name);
	FILE* fp = fopen(file, "wb");

	//fbxファイルの作成時間保存
	wchar_t fullPath[256];
	mbstowcs(fullPath, name, strlen(name));
	// ファイル情報
	WIN32_FIND_DATA findData;
	HANDLE hFile = FindFirstFile(fullPath, &findData);
	if (hFile != INVALID_HANDLE_VALUE) {
		FindClose(hFile);
	}
	fwrite(&findData.ftCreationTime, sizeof(FILETIME), 1, fp);

	// 頂点数
	fwrite(&NumVertices, sizeof(int), 1, fp);
	// 頂点保存
	fwrite(Vertices, sizeof(PolygonVertex), NumVertices, fp);

	// ポリゴン数 NumFaces
	fwrite(&NumFaces, sizeof(int), 1, fp);
	// インデックス Indices 個数NumFaces*3
	fwrite(Indices, sizeof(DWORD), NumFaces * 3, fp);
	// メッシュ数　NumMesh
	fwrite(&NumMesh, sizeof(int), 1, fp);
	// 材質関連　MaterialFaces　MeshMaterial
	fwrite(MaterialFaces, sizeof(int), NumMesh, fp);
	fwrite(MeshMaterial, sizeof(int), NumMesh, fp);
	fwrite(TextureName, sizeof(char), 32 * 128, fp);

	fwrite(Weights, sizeof(WEIGHT), NumVertices, fp);
	fwrite(&NumBone, sizeof(int), 1, fp);
	fwrite(Bone, sizeof(BONE), NumBone, fp);

	fclose(fp);
}

bool FBXMesh::LoadAMG(const char* name)
{
	//if (UseAMG == false) return false;
	char file[256];
	sprintf(file, "%s.amg", name);

	FILE* fp = fopen(file, "rb");
	// ファイルがなかったら失敗
	if (fp == NULL) return false;

	//fbxファイルの作成時間保存
	wchar_t fullPath[256];
	mbstowcs(fullPath, name, strlen(name));
	// ファイル情報
	WIN32_FIND_DATA findData;
	HANDLE hFile = FindFirstFile(fullPath, &findData);
	if (hFile != INVALID_HANDLE_VALUE) {
		FindClose(hFile);
	}
	//元データの更新時間読み込み
	FILETIME saveTime;
	fread(&saveTime, sizeof(FILETIME), 1, fp);

	if (findData.ftLastWriteTime.dwHighDateTime != saveTime.dwHighDateTime ||
		findData.ftLastWriteTime.dwLowDateTime != saveTime.dwLowDateTime) {
		/*fclose(fp);
		return false;*/
	}

	// 頂点数読み込み
	fread(&NumVertices, sizeof(int), 1, fp);
	Vertices = NEW PolygonVertex[NumVertices];
	// 頂点読み込み
	fread(Vertices, sizeof(PolygonVertex), NumVertices, fp);

	// ポリゴン数 NumFaces
	fread(&NumFaces, sizeof(int), 1, fp);
	Indices = NEW DWORD[NumFaces * 3];
	// インデックス Indices 個数NumFaces*3
	fread(Indices, sizeof(DWORD), NumFaces * 3, fp);
	// メッシュ数　NumMesh
	fread(&NumMesh, sizeof(int), 1, fp);
	MaterialFaces = NEW int[NumMesh];
	MeshMaterial = NEW int[NumMesh];
	// 材質関連　MaterialFaces　MeshMaterial
	fread(MaterialFaces, sizeof(int), NumMesh, fp);
	fread(MeshMaterial, sizeof(int), NumMesh, fp);
	fread(TextureName, sizeof(char), 32 * 128, fp);

	Textures = NEW Texture * [NumMesh];
	for (int i = 0; i < NumMesh; i++) {
		if (TextureName[i][0] == '\0') {
			Textures[i] = NULL;
			continue;
		}
		//  C:\\AAA\\BBB\\a.fbx  C:/AAA/BBB/a.fbx
		const char* name = &TextureName[i][strlen(TextureName[i])];
		int a = (int)strlen(TextureName[i]);
		for (int j = 0; j < (int)strlen(TextureName[i]); j++)
		{
			name--;
			if (name[0] == '/') { name++; break; }
			if (name[0] == '\\') { name++; break; }
		}
		char work[128];
		strcpy(work, FBXDir);		//"AAA/BBB/";
		strcat(work, name);			//"AAA/BBB/texture/a.png

		char filename[128];
		strcpy(filename, work);
		Textures[i] = NEW Texture();
		Textures[i]->Load(filename);
	}
	Weights = NEW WEIGHT[NumVertices];
	fread(Weights, sizeof(WEIGHT), NumVertices, fp);

	fread(&NumBone, sizeof(int), 1, fp);
	fread(Bone, sizeof(BONE), NumBone, fp);

	fclose(fp);
	return true;
}

float FBXMesh::GetCurveValue(FbxAnimCurve* curve, FbxTime T)
{
	float value = 0;
	if (curve->KeyGetInterpolation(0) != FbxAnimCurveDef::eInterpolationConstant)
	{
		value = curve->Evaluate(T, 0);
	}
	else {
		FbxAnimCurveKey lKey = curve->KeyGet(0);
		value = lKey.GetValue();
	}
	return value;
}

//------------------------------------------------
//	キーフレーム読み込み
//------------------------------------------------
void FBXMesh::LoadKeyFrames(std::string name, int bone, FbxNode* bone_node, FbxAnimLayer* AnimLayer) {

	FbxAnimCurve* AnimCurveX = bone_node->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* AnimCurveY = bone_node->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* AnimCurveZ = bone_node->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	FbxAnimCurve* AnimCurvePX = bone_node->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* AnimCurvePY = bone_node->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* AnimCurvePZ = bone_node->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	FbxAnimCurve* AnimCurveSX = bone_node->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* AnimCurveSY = bone_node->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* AnimCurveSZ = bone_node->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	FbxDouble3 rot = bone_node->LclRotation.Get();
	FbxDouble3 pos = bone_node->LclTranslation.Get();
	FbxDouble3 scale = bone_node->LclScaling.Get();
	FbxDouble3 prerot = bone_node->PreRotation.Get();
	Vector3 PreR = Vector3((float)prerot[0], (float)prerot[1], (float)prerot[2]) * 3.1415926535f / 180;

	float x, y, z;
	float px, py, pz;
	float sx, sy, sz;
	x = (float)rot[0]; y = (float)rot[1]; z = (float)rot[2];
	px = (float)pos[0]; py = (float)pos[1]; pz = (float)pos[2];
	sx = (float)scale[0]; sy = (float)scale[1]; sz = (float)scale[2];

	// メモリ確保
	Motion* M = &motion[name];
	M->key[bone] = NEW Matrix[M->NumFrame + 1];

	double time = StartFrame * (1.0 / 60);
	FbxTime T = 0;

	for (int f = 0; f < motion[name].NumFrame; f++)
	{
		T.SetSecondDouble(time);
		time += (1.0 / 60);

		// カーブデータ取得
		if (AnimCurveX) x = GetCurveValue(AnimCurveX, T);
		if (AnimCurveY) y = GetCurveValue(AnimCurveY, T);
		if (AnimCurveZ) z = GetCurveValue(AnimCurveZ, T);
		if (AnimCurvePX) px = GetCurveValue(AnimCurvePX, T);
		if (AnimCurvePY) py = GetCurveValue(AnimCurvePY, T);
		if (AnimCurvePZ) pz = GetCurveValue(AnimCurvePZ, T);
		if (AnimCurveSX) sx = GetCurveValue(AnimCurveSX, T);
		if (AnimCurveSY) sy = GetCurveValue(AnimCurveSY, T);
		if (AnimCurveSZ) sz = GetCurveValue(AnimCurveSZ, T);

		Vector3 R = Vector3(x, y, z) * 3.1415926535f / 180;
		Matrix mat;
		Quaternion q, preq;

		float xx = sinf(R.x / 2.0f);
		float xw = cosf(R.x / 2.0f);
		float yy = sinf(R.y / 2.0f);
		float yw = cosf(R.y / 2.0f);
		float zz = sinf(R.z / 2.0f);
		float zw = cosf(R.z / 2.0f);
		q.x = (zw * yw) * xx - (zz * yy) * xw;
		q.y = (zz * yw) * xx + (zw * yy) * xw;
		q.z = -(zw * yy) * xx + (zz * yw) * xw;
		q.w = (zz * yy) * xx + (zw * yw) * xw;

		xx = sinf(PreR.x / 2.0f);
		xw = cosf(PreR.x / 2.0f);
		yy = sinf(PreR.y / 2.0f);
		yw = cosf(PreR.y / 2.0f);
		zz = sinf(PreR.z / 2.0f);
		zw = cosf(PreR.z / 2.0f);
		preq.x = (zw * yw) * xx - (zz * yy) * xw;
		preq.y = (zz * yw) * xx + (zw * yy) * xw;
		preq.z = -(zw * yy) * xx + (zz * yw) * xw;
		preq.w = (zz * yy) * xx + (zw * yw) * xw;

		Quaternion qRot = preq * q;
		qRot.toMatrix(mat);

		mat._11 *= sx; mat._12 *= sx; mat._13 *= sx;
		mat._21 *= sy; mat._22 *= sy; mat._23 *= sy;
		mat._31 *= sz; mat._32 *= sz; mat._33 *= sz;

		mat._41 = px; mat._42 = py; mat._43 = pz;
		M->key[bone][f] = mat;
	}
}
//------------------------------------------------
//	スキニング
//------------------------------------------------

void FBXMesh::Skinning()
{
	Motion* M = &motion[MotionName];
	if (M == NULL) return;
	if (M->key[0] == NULL) return;

	//	配列用変数
	int f = (int)motionFrame;


	// 軽量化
	Vector3 pos;
	Quaternion nowKey;
	Quaternion nextKey;
	Quaternion oldKey;
	Quaternion oldNextKey;
	Matrix nowKeyMat;
	// ブレンド用モーション(nullの場合があるのでポインタ)
	Motion* oldMotion;

	//	行列準備
	Matrix KeyMatrix[256];
	for (int b = 0; b < NumBone; b++)
	{
		Matrix& motionKey = M->key[b][f];

		//位置保存
		nowKey.fromMatrix(motionKey);
		pos.x = motionKey._41;
		pos.y = motionKey._42;
		pos.z = motionKey._43;

		nextKey.fromMatrix(M->key[b][f + 1]);
		// 球面線形補間
		nowKey.slerp(nowKey, nextKey, motionFrame - f);

		// ブレンド用モーション(nullの場合があるのでポインタ渡し)
		oldMotion = &motion[oldMotionName];

		if (oldMotion != nullptr && oldMotion->key[0] != nullptr && nowBlendTime > 0.0f)
		{
			// 新旧のモーション進行度を合わせる
			int of = static_cast<int>(oldFrame);// int型にした前モーションのフレーム

			oldKey.fromMatrix(oldMotion->key[b][of]);

			oldNextKey.fromMatrix(oldMotion->key[b][of + 1]);
			oldKey.slerp(oldKey, oldNextKey, oldFrame - of);

			// maxBlendTime ~ 0にクランプ
			if (nowBlendTime > maxBlendTime)nowBlendTime = maxBlendTime;
			if (nowBlendTime < 0.0f)nowBlendTime = 0.0f;
			// 新旧モーションのブレンド
			// nowBlendTimeは減っていく為旧モーションの比率が減っていく
			nowKey.slerp(nowKey, oldKey, nowBlendTime / maxBlendTime);
		}

		nowKey.toMatrix(nowKeyMat);
		nowKeyMat._41 = pos.x;
		nowKeyMat._42 = pos.y;
		nowKeyMat._43 = pos.z;

		if (Bone[b].parent >= 0)
		{
			nowKeyMat.multiply(nowKeyMat, Bone[Bone[b].parent].transform);
		}
		Bone[b].transform = nowKeyMat;

		//	キーフレーム
		KeyMatrix[b].multiply(Bone[b].OffsetMatrix, nowKeyMat);
	}

	//	頂点変形
	for (int v = 0; v < NumVertices; v++)
	{
		//	頂点 * ボーン行列
		// b = v番目の頂点の影響ボーン[n]
		if (Weights[v].count <= 0) continue;

		Vertices[v].x = 0;
		Vertices[v].y = 0;
		Vertices[v].z = 0;

		Vertices[v].nx = 0;
		Vertices[v].ny = 0;
		Vertices[v].nz = 0;

		Vertices[v].tanx = 0;
		Vertices[v].tany = 0;
		Vertices[v].tanz = 0;

		//	影響個数分ループ
		for (int n = 0; n < Weights[v].count; n++) {
			int b = Weights[v].bone[n];

			float x = VerticesSrc[v].x;
			float y = VerticesSrc[v].y;
			float z = VerticesSrc[v].z;
			//	座標を影響力分移動
			Vertices[v].x += (x * KeyMatrix[b]._11 + y * KeyMatrix[b]._21 + z * KeyMatrix[b]._31 + 1 * KeyMatrix[b]._41) * Weights[v].weight[n];
			Vertices[v].y += (x * KeyMatrix[b]._12 + y * KeyMatrix[b]._22 + z * KeyMatrix[b]._32 + 1 * KeyMatrix[b]._42) * Weights[v].weight[n];
			Vertices[v].z += (x * KeyMatrix[b]._13 + y * KeyMatrix[b]._23 + z * KeyMatrix[b]._33 + 1 * KeyMatrix[b]._43) * Weights[v].weight[n];

			float nx = VerticesSrc[v].nx;
			float ny = VerticesSrc[v].ny;
			float nz = VerticesSrc[v].nz;
			//	法線を影響力分変換
			Vertices[v].nx += (nx * KeyMatrix[b]._11 + ny * KeyMatrix[b]._21 + nz * KeyMatrix[b]._31) * Weights[v].weight[n];
			Vertices[v].ny += (nx * KeyMatrix[b]._12 + ny * KeyMatrix[b]._22 + nz * KeyMatrix[b]._32) * Weights[v].weight[n];
			Vertices[v].nz += (nx * KeyMatrix[b]._13 + ny * KeyMatrix[b]._23 + nz * KeyMatrix[b]._33) * Weights[v].weight[n];

			float tx = VerticesSrc[v].tanx;
			float ty = VerticesSrc[v].tany;
			float tz = VerticesSrc[v].tanz;
			//	法線を影響力分変換
			Vertices[v].tanx += (tx * KeyMatrix[b]._11 + ty * KeyMatrix[b]._21 + tz * KeyMatrix[b]._31) * Weights[v].weight[n];
			Vertices[v].tany += (tx * KeyMatrix[b]._12 + ty * KeyMatrix[b]._22 + tz * KeyMatrix[b]._32) * Weights[v].weight[n];
			Vertices[v].tanz += (tx * KeyMatrix[b]._13 + ty * KeyMatrix[b]._23 + tz * KeyMatrix[b]._33) * Weights[v].weight[n];


		}
	}
	DxSystem::DeviceContext->UpdateSubresource(VertexBuffer, 0, NULL, Vertices, 0, 0);
}

//****************************************************************
//	頂点最適化
//****************************************************************
void FBXMesh::OptimizeVertices()
{
	int currentNum = 0;
	for (int v = 0; v < NumVertices; v++) {
		int sameIndex = -1;
		//	同一頂点検索
		for (int old = 0; old < currentNum; old++) {
			if (Vertices[v].x != Vertices[old].x ||
				Vertices[v].y != Vertices[old].y ||
				Vertices[v].z != Vertices[old].z ||
				Vertices[v].nx != Vertices[old].nx ||
				Vertices[v].ny != Vertices[old].ny ||
				Vertices[v].nz != Vertices[old].nz ||
				Vertices[v].tu != Vertices[old].tu ||
				Vertices[v].tv != Vertices[old].tv/* ||
				Vertices[v].color != Vertices[old].color*/) continue;

			sameIndex = old;
			break;
		}

		int target = v;
		if (sameIndex == -1) {
			//	新規頂点
			CopyMemory(&Vertices[currentNum], &Vertices[v], sizeof(PolygonVertex));
			CopyMemory(&Weights[currentNum], &Weights[v], sizeof(WEIGHT));
			target = currentNum;
			currentNum++;
		}
		else {
			target = sameIndex;
		}
		//	インデックス更新
		for (int i = 0; i < NumVertices; i++) {
			if (Indices[i] == v) Indices[i] = target;
		}
	}

	//	新バッファ確保
	PolygonVertex* buf = NEW PolygonVertex[currentNum];
	CopyMemory(buf, Vertices, sizeof(PolygonVertex) * currentNum);
	NumVertices = currentNum;

	delete[] Vertices;
	Vertices = buf;
}


// レイキャスト
int FBXMesh::Raycast(
	Vector3& pos, Vector3& v,
	Vector3& out,
	Vector3& normal)
{
	int ret = -1; // ヒットした面番号
	// 一番近いヒット面までの距離
	float min_dist = 10000;

	// レイ方向正規化
	Vector3 V = v;
	V.Normalize();

	// 全ポリゴンと判定
	for (int n = 0; n < NumFaces; n++)
	{
		//n番目の三角形の0番目の点
		int index = Indices[n * 3 + 0];
		Vector3 v0;
		v0.x = Vertices[index].x;
		v0.y = Vertices[index].y;
		v0.z = Vertices[index].z;
		//n番目の三角形の1番目の点
		int index1 = Indices[n * 3 + 1];
		Vector3 v1;
		v1.x = Vertices[index1].x;
		v1.y = Vertices[index1].y;
		v1.z = Vertices[index1].z;
		//n番目の三角形の2番目の点
		int index2 = Indices[n * 3 + 2];
		Vector3 v2;
		v2.x = Vertices[index2].x;
		v2.y = Vertices[index2].y;
		v2.z = Vertices[index2].z;

		// 三角形の3辺
		Vector3 l0 = v1 - v0; // ０→１
		Vector3 l1 = v2 - v1; // １→２
		Vector3 l2 = v0 - v2; // ２→０

		// 面法線(2辺の外積)
		Vector3 N;
		Vector3::cross(N, l0, l1); // l0とl1の外積 = 法線N
		N.Normalize(); // 正規化(長さ１に)

		// 発射位置から3角形の一点を法線に射影
		Vector3 work = v0 - pos; // posからv0へのベクトル
		// 内積で射影
		float L = Vector3::dot(work, -N);
		if (L < 0.0f) continue; // 逆方向

		// 交点
		float L2 = Vector3::dot(V, -N);
		if (L2 <= 0.0f) continue; // 平行or反対向き

		float scale = L / L2; // LはL2の？倍
		if (scale > min_dist) continue; // 今までで一番近い？

		Vector3 P = pos + V * scale; // posからV方向にscale倍

		// 内点判定
		// l0とP
		Vector3 vp0 = P - v1; //v1からPへのベクトル
		Vector3 N0;
		Vector3::cross(N0, l0, vp0); //▲v0-v1-Pの法線
		// NとN0の方向判定
		float d0 = Vector3::dot(N, N0);
		if (d0 < 0) continue; //反対向き = 内点ではない！

		// l1とP
		Vector3 vp1 = P - v2; //v2からPへのベクトル
		Vector3 N1;
		Vector3::cross(N1, l1, vp1); //▲v1-v2-Pの法線
		// NとN1の方向判定
		float d1 = Vector3::dot(N, N1);
		if (d1 < 0) continue; //反対向き = 内点ではない！

		// l2とP
		Vector3 vp2 = P - v0; //v2からPへのベクトル
		Vector3 N2;
		Vector3::cross(N2, l2, vp2); //▲v2-v0-Pの法線
		// NとN2の方向判定
		float d2 = Vector3::dot(N, N2);
		if (d2 < 0) continue; //反対向き = 内点ではない！

		// 交点決定
		out = P; //当たった位置
		normal = N; //面法線
		ret = n; //n番目のポリゴンにヒット
		min_dist = scale; //距離保存
	}
	return ret;
}


// トランスフォームを考慮したレイキャスト
int FBXMesh::RaycastTRS(Vector3& pos, Vector3& v, Vector3& out, Vector3& normal)
{
	//Transform
	Matrix inv = transform;
	inv.inverse();
	//posとvを逆Transform変換		pos * inv
	//x = x * B11 + y * B21 + z * B31 + w * B41;
	//y = x * B12 + y * B22 + z * B32 + w * B42;
	//z = x * B13 + y * B23 + z * B33 + w * B43;
	//w = x * B14 + y * B24 + z * B34 + w * B44;

	Vector3 p;
	p.x = pos.x * inv._11 + pos.y * inv._21 + pos.z * inv._31 + 1 * inv._41;
	p.y = pos.x * inv._12 + pos.y * inv._22 + pos.z * inv._32 + 1 * inv._42;
	p.z = pos.x * inv._13 + pos.y * inv._23 + pos.z * inv._33 + 1 * inv._43;

	Vector3 v2;
	v2.x = v.x * inv._11 + v.y * inv._21 + v.z * inv._31;// + 0 * inv._41;
	v2.y = v.x * inv._12 + v.y * inv._22 + v.z * inv._32;// + 0 * inv._42;
	v2.z = v.x * inv._13 + v.y * inv._23 + v.z * inv._33;// + 0 * inv._43;

	//Reycast
	Vector3 o, n;
	int index = Raycast(p, v2, o, n);
	if (index < 0)return index;		//当たらなかった

	//outとnormalをTransform変換
	out.x = o.x * transform._11 + o.y * transform._21 + o.z * transform._31 + 1 * transform._41;
	out.y = o.x * transform._12 + o.y * transform._22 + o.z * transform._32 + 1 * transform._42;
	out.z = o.x * transform._13 + o.y * transform._23 + o.z * transform._33 + 1 * transform._43;
	normal.x = n.x * transform._11 + n.y * transform._21 + n.z * transform._31;// + 0 * transform._41;
	normal.y = n.x * transform._12 + n.y * transform._22 + n.z * transform._32;// + 0 * transform._42;
	normal.z = n.x * transform._13 + n.y * transform._23 + n.z * transform._33;// + 0 * transform._43;
	return index;
}

void FBXMesh::SetHeight(Texture* HeightMap)
{
	BYTE* pixels =
		HeightMap->GetPixels();
	int w = HeightMap->GetWidth();
	int h = HeightMap->GetHeight();

	for (int v = 0; v < NumVertices; v++)
	{
		// 値vx(0~500) を 0~1023 に
		// 値vx(0~500)　を 0~1.0　に
		//     0~1.0　を　0~１０２３
		int x = (int)((Vertices[v].x / 500)
			* (w - 1));
		int z = (int)((Vertices[v].z / 500)
			* (h - 1));
		float height =
			pixels[(z * w + x) * 4] * 0.4f;

		Vertices[v].y = height;
	}
	delete[] pixels;

	// 全法線再計算

	for (int v = 0; v < NumVertices; v++)
	{
		Vertices[v].nx = 0;
		Vertices[v].ny = 0;
		Vertices[v].nz = 0;
	}
	// 全ポリゴン法線計算
	for (int f = 0; f < NumFaces; f++) {
		// 3頂点
		int a = Indices[f * 3 + 0];
		int b = Indices[f * 3 + 1];
		int c = Indices[f * 3 + 2];
		Vector3 va = Vector3(Vertices[a].x, Vertices[a].y, Vertices[a].z);
		Vector3 vb = Vector3(Vertices[b].x, Vertices[b].y, Vertices[b].z);
		Vector3 vc = Vector3(Vertices[c].x, Vertices[c].y, Vertices[c].z);
		Vector3 l1 = vb - va;
		Vector3 l2 = vc - vb;
		// 面法線
		Vector3 N;
		Vector3::cross(N, l1, l2);
		N.Normalize();
		//
		Vertices[a].nx += N.x; Vertices[a].ny += N.y; Vertices[a].nz += N.z;
		Vertices[b].nx += N.x; Vertices[b].ny += N.y; Vertices[b].nz += N.z;
		Vertices[c].nx += N.x; Vertices[c].ny += N.y; Vertices[c].nz += N.z;
	}
	// 全法線正規化
	for (int v = 0; v < NumVertices; v++)
	{
		Vector3 N =
			Vector3(Vertices[v].nx, Vertices[v].ny, Vertices[v].nz);
		N.Normalize();
		Vertices[v].nx = N.x;
		Vertices[v].ny = N.y;
		Vertices[v].nz = N.z;
	}


	DxSystem::DeviceContext->UpdateSubresource(
		VertexBuffer, 0, NULL, Vertices, 0, 0);
}
