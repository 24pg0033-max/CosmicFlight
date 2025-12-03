#include "Loading.h"
#include "../Input/Input.h"
#include "../Memory/Memory.h"
#include "../global.h"

Loading::Loading(){
	Initialize();
}

Loading::~Loading(){
}

bool Loading::Initialize() {
	loading = std::make_unique<Render2D>();
	loading->Initialize("Assets/scene/load.png");
	loading->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

	loadingUI = std::make_unique<_2DUI>();
	loadingUI->Initialize("Assets/UI/pressstart.png", 480, 180);
	loadingUI->SetPosition(Vector3(400.0f, 500.0f, 0.0f));

	nLoadingUI = std::make_unique<_2DUI>();
	nLoadingUI->Initialize("Assets/UI/nowLoading.png", 480, 180, Vector3(700.0f, 580.0f, 0.0f));

	circle = std::make_unique<_2DUI>();
	circle->Initialize("Assets/UI/circle.png", 30, 30, Vector3(1100.0f, 656.0f, 0.0f));

	circle2 = std::make_unique<_2DUI>();
	circle2->Initialize("Assets/UI/circle.png", 30, 30, Vector3(1150.0f, 656.0f, 0.0f));

	circle3 = std::make_unique<_2DUI>();
	circle3->Initialize("Assets/UI/circle.png", 30, 30, Vector3(1200.0f, 656.0f, 0.0f));

	eLoad = std::make_unique<Easing>(Easing::STATE::EASEOUTCUBIC, LoadSpeed, 0.0f, true);

	loadEnd = false;
	Time = 0.0f;
	barLength = 0.0f;
	checkPoint = 0;
	barLengthMax = 600.0f;
	length = 0.0f;

	return true;
}

void Loading::Update(){
	eLoadRate = eLoad->RateUpdate();
	loadingUI->SetAlpha(eLoadRate * LoadAlphaMax);

	if (eLoad->GetIsMaxRate() || eLoad->GetIsMinRate()) {
		eLoad->ChangeIsOn();
		eLoad->ResetRateFlags();
	}

	if (!loadEnd)Time += DeltaTime;
	switch (checkPoint)
	{
	case 1:
		barLength = barLengthMax / 5.0f;
		break;
	case 2:
		barLength = barLengthMax / 4.0f;
		break;
	case 3:
		barLength = barLengthMax / 3.0f;
		break;
	case 4:
		barLength = barLengthMax / 2.0f;
		break;
	case 5:
		barLength = barLengthMax / 1.0f;
		break;
	}
}

void Loading::Render() {
	loading->Render();

	if(loadEnd){
		loadingUI->Render();
	}
	else {
		nLoadingUI->Render();
		const int Interval = 480;
		count++;
		if (count >= Interval / 3 && cFlag) {
			circle->Render();
			if (count >= Interval / 1.5f && cFlag) {
				circle2->Render();
				if (count >= Interval && cFlag) {
					circle3->Render();
				}
			}
		}
	}

	if (barLength > length) {
		length += 180.0f * DeltaTime;
	}
	else {
		length = barLength;
	}

	if (length >= barLengthMax) {
		loadEnd = true;
	}
}

void Loading::ChecKPoint(){
	checkPoint++;
}




