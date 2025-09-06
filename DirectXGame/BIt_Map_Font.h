#pragma once
#include "KamataEngine.h"

class BIt_Map_Font {
public:

	void Initialize();
	void Set(int value);
	void Draw();

private:

	static constexpr int kNumDigits = 5;
	KamataEngine::Sprite* numberSprite_[kNumDigits] = {};
	uint32_t numberTextureHandle_ = 0;
	const KamataEngine::Vector2 kFontSize = {32.0f, 64.0f};

};