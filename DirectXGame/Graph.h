#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Graph {
public:
	// デストラクタ
	~Graph();

	// 初期化
	void Initialize();

	// 更新
	void Update(int hp);

	// 描画
	void Draw();

private:
	// HP表示用
	uint32_t hpTextureHandle_ = 0;
	Sprite* hpSprites_[3] = {nullptr, nullptr, nullptr}; // ハートを3つ管理

	// 現在のHP
	int currentHP_ = 3;
};