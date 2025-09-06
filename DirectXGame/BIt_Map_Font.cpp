#include "BIt_Map_Font.h"
#include <cmath>
#include <string>

using namespace KamataEngine;

void BIt_Map_Font::Initialize() {
	numberTextureHandle_ = TextureManager::Load("number.png");
	for (int i = 0; i < kNumDigits; ++i) {
		numberSprite_[i] = Sprite::Create(numberTextureHandle_, {1100.0f + kFontSize.x * i, 10.0f});
		numberSprite_[i]->SetSize(kFontSize);
		numberSprite_[i]->SetTextureRect({0, 0}, kFontSize);
	}
}

void BIt_Map_Font::Set(int value) {
	for (int i = 0; i < kNumDigits; ++i) {
		int digit = (value / static_cast<int>(std::pow(10, kNumDigits - 1 - i))) % 10;

		Vector2 texBase = {kFontSize.x * digit, 0};
		numberSprite_[i]->SetTextureRect(texBase, kFontSize);
	}
}

void BIt_Map_Font::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Sprite::PreDraw(dxCommon->GetCommandList());

	for (int i = 0; i < kNumDigits; ++i) {
		numberSprite_[i]->Draw();
	}

	Sprite::PostDraw();
}