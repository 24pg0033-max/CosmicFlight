#include "Easing.h"
#include <math.h>

Easing::Easing(Easing::STATE st, float speed, float start, bool firstIsOn){
	changeSpeed = speed;
	changeRate = start;
	isOn = firstIsOn;
	easeState = st;
	nowFrame = 0.0f;
	isMaxRate = false;
	isMinRate = false;
	changeRate_Max = 1.0f;
	changeRate_Min = 0.0f;

	switch (easeState) {
	case LINEAR: {
		changeRate_Max = 1.0f;
		changeRate_Min = 0.0f;
	}break;
	case EASEOUTQUAD: {
		changeRate_Max = 1.0f;
		changeRate_Min = 0.0f;
		nowFrame = start;
	}break;
	case EASEINQUAD: {
		changeRate_Max = 1.0f;
		changeRate_Min = 0.0f;
		nowFrame = start;
	}break;
	case EASEOUTCUBIC: {
		changeRate_Max = 1.0f;
		changeRate_Min = 0.0f;
		nowFrame = start;
	}break;
	case EASEOUTQUINT: {
		changeRate_Max = 1.0f;
		changeRate_Min = 0.0f;
		nowFrame = start;
	}break;
	case EASEINQUINT: {
		changeRate_Max = 1.0f;
		changeRate_Min = 0.0f;
		nowFrame = start;
	}break;
	case EASEINCUBIC: {
		changeRate_Max = 1.0f;
		changeRate_Min = 0.0f;
		nowFrame = start;
	}break;
	case EASEINSINE: {
		changeRate_Max = 1.0f;
		changeRate_Min = 0.0f;
		nowFrame = start;
	}break;
	case EASEOUTEXPO: {
		changeRate_Max = 1.0f;
		changeRate_Min = 0.0f;
		nowFrame = start;
	}break;
	}
}

void Easing::ChangeIsOn(){
	if (isOn)isOn = false;
	else isOn = true;
}

float Easing::RateUpdate(){
	isMaxRate = false;
	isMinRate = false;
	switch (easeState) {
	case LINEAR: {//@0`1‚Ì•Ï‰»
		Linear();
	}break;
	case EASEOUTQUAD: {//@1-(1-x)^3    x:0.0f`1.0f
		EaseOutQuad();
	}break;
	case EASEINQUAD: {
		EaseInQuad();
	}break;
	case EASEOUTCUBIC: {//1 - pow(1 - x, 3);
		EaseOutCubic();
	}break;
	case EASEOUTQUINT: {
		EaseOutQuint();
	}break;
	case EASEINQUINT: {
		EaseInQuint();
	}break;
	case EASEINCUBIC: {
	EaseInCubic();
	}break;
	case EASEINSINE: {
	EaseInSine();
	}break;
	case EASEOUTEXPO: {
	EaseOutExpo();
	}break;
	}
	return changeRate;
}

void Easing::SetSpeed(float speed) {
	changeSpeed = speed;
}

void Easing::Linear(){
	if (isOn) {
		changeRate += changeSpeed;
		if (changeRate > changeRate_Max) {
			changeRate = changeRate_Max;
			isMaxRate = true;
		}
	}
	else {
		changeRate -= changeSpeed;
		if (changeRate < changeRate_Min) {
			changeRate = changeRate_Min;
			isMinRate = true;
		}
	}
}


void Easing::EaseOutQuad(){
	if (isOn) {
		nowFrame += changeSpeed;
		if (nowFrame > changeRate_Max) {
			nowFrame = changeRate_Max;
			isMaxRate = true;
		}
	}
	else {
		nowFrame -= changeSpeed;
		if (nowFrame < changeRate_Min) {
			nowFrame = changeRate_Min;
			isMinRate = true;
		}
	}
	changeRate = 1 - (1 - nowFrame) * (1 - nowFrame);
}

void Easing::EaseInQuad(){
	if (isOn) {
		nowFrame += changeSpeed;
		if (nowFrame > changeRate_Max) {
			nowFrame = changeRate_Max;
			isMaxRate = true;
		}
	}
	else {
		nowFrame -= changeSpeed;
		if (nowFrame < changeRate_Min) {
			nowFrame = changeRate_Min;
			isMinRate = true;
		}
	}
	changeRate = nowFrame * nowFrame;
}

void Easing::EaseOutCubic(){
	if (isOn) {
		nowFrame += changeSpeed;
		if (nowFrame >= changeRate_Max) {
			nowFrame = changeRate_Max;
			isMaxRate = true;
		}
	}
	else {
		nowFrame -= changeSpeed;
		if (nowFrame <= changeRate_Min) {
			nowFrame = changeRate_Min;
			isMinRate = true;
		}
	}
	changeRate = 1 - pow(1 - nowFrame, 3);
}

void Easing::EaseOutQuint(){
	if (isOn) {//On‚È‚çFrame‚ğ‚P‚Ö
		nowFrame += changeSpeed;
		if (nowFrame > changeRate_Max) {//ãŒÀ‚É—ˆ‚½‚©‚Ç‚¤‚©
			nowFrame = changeRate_Max;
			isMaxRate = true;
		}
	}
	else {//Off‚È‚çFrame‚ğ0‚Ö
		nowFrame -= changeSpeed;
		if (nowFrame < 0.0f)nowFrame = 0;//‰ºŒÀ‚É—ˆ‚½‚©‚Ç‚¤‚©
	}
	changeRate = 1 - pow(1 - nowFrame, 5);
}

void Easing::EaseInQuint(){
	if (isOn) {//On‚È‚çFrame‚ğ‚P‚Ö
		nowFrame += changeSpeed;
		if (nowFrame > changeRate_Max) {//ãŒÀ‚É—ˆ‚½‚©‚Ç‚¤‚©
			nowFrame = changeRate_Max;
			isMaxRate = true;
		}
	}
	else {//Off‚È‚çFrame‚ğ0‚Ö
		nowFrame -= changeSpeed;
		if (nowFrame < 0.0f)nowFrame = 0;//‰ºŒÀ‚É—ˆ‚½‚©‚Ç‚¤‚©
	}
	changeRate = nowFrame * nowFrame * nowFrame * nowFrame * nowFrame;//5æ
}

void Easing::EaseInCubic() {
	if (isOn) {//On‚È‚çFrame‚ğ‚P‚Ö
		nowFrame += changeSpeed;
		if (nowFrame > changeRate_Max) {//ãŒÀ‚É—ˆ‚½‚©‚Ç‚¤‚©
			nowFrame = changeRate_Max;
			isMaxRate = true;
		}
	}
	else {//Off‚È‚çFrame‚ğ0‚Ö
		nowFrame -= changeSpeed;
		if (nowFrame < 0.0f)nowFrame = 0;//‰ºŒÀ‚É—ˆ‚½‚©‚Ç‚¤‚©
	}
	changeRate = nowFrame * nowFrame * nowFrame;
}

void Easing::EaseInSine() {
	if (isOn) {//On‚È‚çFrame‚ğ‚P‚Ö
		nowFrame += changeSpeed;
		if (nowFrame > changeRate_Max) {//ãŒÀ‚É—ˆ‚½‚©‚Ç‚¤‚©
			nowFrame = changeRate_Max;
			isMaxRate = true;
		}
	}
	else {//Off‚È‚çFrame‚ğ0‚Ö
		nowFrame -= changeSpeed;
		if (nowFrame < 0.0f)nowFrame = 0;//‰ºŒÀ‚É—ˆ‚½‚©‚Ç‚¤‚©
	}
	changeRate = 1 - cos((nowFrame * PI) / 2);
}

void Easing::EaseOutExpo(){
	if (isOn) {//On‚È‚çFrame‚ğ‚P‚Ö
		nowFrame += changeSpeed;
		if (nowFrame > changeRate_Max) {//ãŒÀ‚É—ˆ‚½‚©‚Ç‚¤‚©
			nowFrame = changeRate_Max;
			isMaxRate = true;
		}
	}
	else {//Off‚È‚çFrame‚ğ0‚Ö
		nowFrame -= changeSpeed;
		if (nowFrame < 0.0f)nowFrame = 0;//‰ºŒÀ‚É—ˆ‚½‚©‚Ç‚¤‚©
	}
	changeRate = nowFrame == 1 ? 1 : 1 - pow(2, -10 * nowFrame);
}

void Easing::Reset() {
	nowFrame = 0.0f;
	changeRate = 0.0f;
	isOn = true;
	isMaxRate = false;
	isMinRate = false;
}