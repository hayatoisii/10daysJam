#include "BIt_Map_Font.h"
#include <cmath>
#include <string>

using namespace KamataEngine;

// ▼▼▼ デストラクタを実装 ▼▼▼
BIt_Map_Font::~BIt_Map_Font() {
	// 数字スプライトの解放
	for (int i = 0; i < kNumDigits; ++i) {
		delete numberSprite_[i];
	}
	// Mスプライトの解放
	delete mSprite_;
}

void BIt_Map_Font::Initialize() {
	numberTextureHandle_ = TextureManager::Load("number.png");
	for (int i = 0; i < kNumDigits; ++i) {
		// ここのX座標は元のままでOKです
		numberSprite_[i] = Sprite::Create(numberTextureHandle_, {1064.0f + kFontSize.x * i, 10.0f});
		numberSprite_[i]->SetSize(kFontSize);
		numberSprite_[i]->SetTextureRect({0, 0}, kFontSize);
	}

	mTextureHandle_ = TextureManager::Load("M.png");
	mSprite_ = Sprite::Create(mTextureHandle_, {0.0f, 0.0f});

	// ▼▼▼ ここのブロックを修正します ▼▼▼

	// 1. Mのサイズを縦横比を維持したまま設定します (例: 50x50ピクセル)
	//    この 'mSize' の値を変更すると、Mの大きさを自由に変えられます。
	const float mSize = 30.0f;
	mSprite_->SetSize({mSize, mSize});

	// 2. Mの位置を再計算します
	//    X座標: 数字の右端に、少しだけ間隔をあけて配置
	float mPositionX = 1064.0f + (kFontSize.x * kNumDigits) + -5.0f; // 5.0fは数字との間隔

	//    Y座標: 数字の上下中央に配置されるように調整
	float mPositionY = 16.0f + (kFontSize.y - mSize) / 2.0f;

	mSprite_->SetPosition({mPositionX, mPositionY});

	// ▲▲▲ 修正はここまで ▲▲▲
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

	// 数字を描画
	for (int i = 0; i < kNumDigits; ++i) {
		numberSprite_[i]->Draw();
	}

	// ▼▼▼ Mのスプライト描画処理を追加 ▼▼▼
	if (mSprite_) {
		mSprite_->Draw();
	}
	// ▲▲▲ ここまで追加 ▲▲▲

	Sprite::PostDraw();
}