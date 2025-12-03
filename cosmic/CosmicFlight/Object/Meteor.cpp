#include "Meteor.h"
#include "../DXShader.h"
#include "../global.h"

Meteor::Meteor(){
	flag = true;
}

Meteor::~Meteor(){
	delete mesh;
}

void Meteor::Initialize(FBXMesh* pMesh){
	mesh = NEW FBXMesh();
	mesh->Clone(pMesh);
	scale = firstMeteorScale;
}

void Meteor::Update(){
	motionTime += DeltaTime;

    if (arcState == ArcState::Arc) {
        ArcMotion();
        arcElapsed += DeltaTime;
        if (arcElapsed >= arcDuration) {
            // 弧終了から通常モーションに切り替え
            arcState = ArcState::NormalMotion;
        }
    }
    else {
        // 通常のモーション
        switch (motionType) {
        case MotionType::Straight:
            MotionStraight();
            break;
        case MotionType::SpiralR:
            MotionSpiralR();
            break;
        case MotionType::SpiralL:
            MotionSpiralL();
            break;
        case MotionType::Snake:
            MotionSnake();
            break;
        case MotionType::Wave:
            MotionWave();
            break;
        case MotionType::Diagonal:
            MotionDiagonal();
            break;
        default:
            MotionStraight();
            break;
        }
    }

	rotation += rotationSpeed * DeltaTime;

	Object::Update();
}

void Meteor::ArcMotion() {
    if (!arcInit) {
        arcStartPos = position;
        arcAngle = 0.0f;
        arcInit = true;
    }

    arcAngle -= arcSpeed * DeltaTime;

    position.x = arcStartPos.x + arcRadius * cos(arcAngle);
    position.z = arcStartPos.z + arcRadius * sin(arcAngle);
}

void Meteor::MotionStraight(){
	position += velocity * DeltaTime;
}

void Meteor::MotionSpiralBase(){
	move = Vector3(spiralX, spiralY, velocity.z) * DeltaTime;
	position += move;
}

void Meteor::MotionSpiralR(){
	spiralX = cosf(motionTime * spiralSpeed) * spiralRadius;
	spiralY = sinf(motionTime * spiralSpeed) * spiralRadius;
	MotionSpiralBase();
}

void Meteor::MotionSpiralL(){
	spiralX = cosf(-motionTime * spiralSpeed) * spiralRadius;
	spiralY = sinf(-motionTime * spiralSpeed) * spiralRadius;
	MotionSpiralBase();
}

void Meteor::MotionSnake() {
	snakeX = sinf(motionTime * waveFrequency) * waveAmplitude;
	move = Vector3(snakeX, 0.0f, velocity.z) * DeltaTime;
	position += move;
}

void Meteor::MotionWave() {
	waveX = sinf(motionTime * waveFrequency) * waveAmplitude;
	waveY = cosf(motionTime * waveFrequency * 0.5f) * waveAmplitude * 0.5f;
	position += Vector3(waveX, waveY, velocity.z) * DeltaTime;
}

void Meteor::MotionDiagonal(){
	diagonalX = sinf(motionTime * waveFrequency) * waveAmplitude;
	diagonalY = cosf(motionTime * waveFrequency * 0.5f) * waveAmplitude * 0.5f;
	position += Vector3(waveX, waveY, velocity.z) * DeltaTime;
}